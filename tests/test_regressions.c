/*
Title: test_regressions.c
Description: One test per bug fixed in the memory-leak pass. The leaks themselves
only show up under valgrind (make test-memcheck), so these pin down the behaviour
around them: the paths that used to leak still return what callers expect, and the
two memory-safety bugs are gone.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/flwc/includes/Challenges.h"
#include "../src/flwg/includes/FLWGGame.h"
#include "../src/flwg/includes/Hints2.h"
#include "../src/flwp/includes/GameFunctions.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/WordSet.h"
#include "../src/structs/includes/IntLinkedList.h"

/*initDataStructures used to hand its descriptor to fdopen and then fclose it,
so every close(fd) that followed was a stale double close on a number the next
open() could hand back to something else*/
static void test_init_leaves_the_caller_owning_the_fd(void){
	int fd = open("docs/2.txt", O_RDONLY);
	struct DataStructures* data;
	int closeResult;

	CHECK(fd != -1);
	data = initDataStructures(fd, 2);
	CHECK_NOT_NULL(data);

	/*The descriptor is still the caller's to close*/
	closeResult = close(fd);
	CHECK_INT(closeResult, 0);

	/*and closing it does not disturb the structures that were built from it*/
	CHECK_INT(data->I2W->numWords, declared_word_count("docs/2.txt"));
	CHECK_NOT_NULL(Convert_IntToWord(0, data->I2W));

	freeDataStructures(data);
}

/*The hint guards tested solution->list, which malloc(0) makes non-NULL, so a
goal that could not be reached fell through to a read at index -1*/
static void test_flwc_hints_handle_an_unreachable_goal(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* noWords[] = {NULL};
	struct WordSet* empty = convertCharPtrPtrToWordSet(noWords, data);
	struct GameComponentsFLWC components;
	char* path;

	/*An empty goal set means the search can never succeed*/
	components.wordId = 0;
	components.goalWords = empty;
	components.avoidWords = empty;

	CHECK_NULL(hintGoalWordFLWC(&components, data));
	CHECK_NULL(hintAdjacencyTowardsGoalFLWC(&components, data));
	path = hintPathToGoalFLWC(&components, data);
	CHECK_NULL(path);
	free(path);
	CHECK_INT(hintMinDistanceToGoalFLWC(&components, data), -1);
	CHECK_INT(hintDistanceFromNearestAvoidWordFLWIC(&components, data), -1);

	free_WordSet(empty);
	freeDataStructures(data);
}

/*A goal word that is reachable still produces a usable hint, so the guard above
did not simply switch the hints off*/
static void test_flwc_hints_still_answer_a_reachable_goal(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"care", NULL};
	char* noWords[] = {NULL};
	struct GameComponentsFLWC components;
	char* path;

	components.wordId = Convert_WordToInt("ware", data);
	components.goalWords = convertCharPtrPtrToWordSet(goalWords, data);
	components.avoidWords = convertCharPtrPtrToWordSet(noWords, data);
	CHECK(components.wordId != -1);

	/*ware -> care is a single substitution*/
	CHECK_STR(hintGoalWordFLWC(&components, data), "care");
	CHECK_STR(hintAdjacencyTowardsGoalFLWC(&components, data), "care");
	CHECK_INT(hintMinDistanceToGoalFLWC(&components, data), 1);

	path = hintPathToGoalFLWC(&components, data);
	CHECK_NOT_NULL(path);
	if(path != NULL){
		CHECK(strstr(path, "ware") != NULL);
		CHECK(strstr(path, "care") != NULL);
	}
	free(path);

	free_WordSet(components.goalWords);
	free_WordSet(components.avoidWords);
	freeDataStructures(data);
}

/*Both start pickers leaked their array list on this path. They no longer take
it: an adjacency count is a preference, so a count nothing has is widened until
something does rather than handed back as -1*/
static void test_choose_start_widens_rather_than_reporting_no_match(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	/*No word has anywhere near this many adjacencies, so both give the count up
	and answer with a word that exists*/
	int stretched = ChooseStart_Range(data->I2W, 999, 1000);
	CHECK(stretched != -1);
	CHECK(getNumAdjacencies(stretched, data) > 0);

	int exact = ChooseStart(data->I2W, 999);
	CHECK(exact != -1);
	CHECK(getNumAdjacencies(exact, data) > 0);

	/*A range that does match is honoured exactly - widening is the last resort,
	not the first*/
	for(int attempt = 0; attempt < 20; attempt++){
		int chosen = ChooseStart_Range(data->I2W, 4, 4);
		CHECK(chosen != -1);
		if(chosen != -1){
			CHECK_INT(getNumAdjacencies(chosen, data), 4);
		}
	}

	/*And an exact count that exists comes back exactly*/
	for(int attempt = 0; attempt < 20; attempt++){
		int chosen = ChooseStart(data->I2W, 6);
		CHECK(chosen != -1);
		if(chosen != -1){
			CHECK_INT(getNumAdjacencies(chosen, data), 6);
		}
	}

	freeDataStructures(data);
}
/*botPly returned -1 before freeing its three minimax nodes*/
static void test_botply_gives_up_when_every_word_is_used(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int start = Convert_WordToInt("ware", data);
	int i;

	/*With the whole dictionary claimed there is nowhere to move*/
	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}
	CHECK_INT(isTrapped(start, data), 1);
	CHECK_INT(botPly(start, 4, data->I2W, data->wordSet), -1);

	/*With a clear board it finds a legal move again*/
	reset_WordSet(data->wordSet);
	markUsed_WordSet(start, data->wordSet);
	int played = botPly(start, 4, data->I2W, data->wordSet);
	CHECK(played != -1);
	if(played != -1){
		CHECK_INT(letters_that_differ(Convert_IntToWord(start, data->I2W),
			Convert_IntToWord(played, data->I2W), 4), 1);
	}

	freeDataStructures(data);
}

/*Fill_HashMaps dropped the wordStruct for every duplicate key. Reading a four
letter dictionary as three letters truncates words into duplicates, which is the
cheapest way to drive that path*/
static void test_duplicate_keys_do_not_break_the_map(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 3);
	int id;

	/*Every word is truncated to three letters, which is the point: a four letter
	list read as three letters is full of duplicates by construction*/
	const char* first;
	CHECK_INT(data->I2W->numWords, declared_word_count("docs/4.txt"));
	CHECK_INT(data->I2W->numLetters, 3);
	first = Convert_IntToWord(0, data->I2W);
	CHECK_NOT_NULL(first);
	CHECK_INT((int)strlen(first), 3);

	/*The first of the duplicates is the one the tree kept, and it still resolves.
	Read back rather than named: which word lands at index nought is whichever
	has the most connections, and that moves when the word list does*/
	id = Convert_WordToInt(first, data);
	CHECK(id != -1);
	if(id != -1){
		CHECK_STR(Convert_IntToWord(id, data->I2W), first);
	}

	freeDataStructures(data);
}

/*removeWord_FLWP dropped the string RemoveWord_Struct hands back*/
static void test_remove_word_rewinds_the_path(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	int secondWord = gc->solution->next->next->data;
	char* secondWordText = Convert_IntToWord(secondWord, data->I2W);

	CHECK_INT(userEntersWord_FLWP(secondWordText, gc, data), VALID);
	CHECK_INT(getPrevWordFLWP(gc), secondWord);

	/*Removing the word puts the path back to the start word*/
	removeWord_FLWP(secondWordText, gc, data);
	CHECK_INT(getPrevWordFLWP(gc), gc->start);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/*The letter hint took the first unused neighbour and named whatever letter it
substituted in, without asking whether the word already had one. About one
three-letter word in twenty was answered with a letter sitting in front of the
player -- "try a P" on PIG*/
static void test_letter_hint_prefers_a_letter_the_word_does_not_have(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int i;
	int checked = 0;

	for(i = 0; i < data->I2W->numWords; i++){
		char* word = Convert_IntToWord(i, data->I2W);
		struct intList* c;
		int aFreshLetterExists = 0;
		char hinted;

		/*Only words that have a better answer available can demand one*/
		for(c = getConnections(i, data->I2W)->next; c != NULL; c = c->next){
			char* neighbour = Convert_IntToWord(c->data, data->I2W);
			int k;
			for(k = 0; k < 3; k++){
				if(neighbour[k] != word[k]){
					if(strchr(word, neighbour[k]) == NULL){
						aFreshLetterExists = 1;
					}
					break;
				}
			}
		}
		if(!aFreshLetterExists){
			continue;
		}

		hinted = letterToConsiderHint(i, data);
		checked++;
		CHECK(hinted != '?');
		CHECK(strchr(word, hinted) == NULL);
	}

	/*A sweep that looked at nothing would pass for the wrong reason*/
	CHECK(checked > 400);

	freeDataStructures(data);
}

/*With every neighbour spent the hint kept a word id of -1, and
Convert_IntToWord hands back NULL for a negative id -- which the letter
comparison then read through, one character at a time*/
static void test_letter_hint_answers_when_every_neighbour_is_spent(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int start = Convert_WordToInt("bag", data);
	int i;

	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}
	CHECK_INT(letterToConsiderHint(start, data), '?');

	/*and with the board cleared it has something to say again*/
	reset_WordSet(data->wordSet);
	markUsed_WordSet(start, data->wordSet);
	CHECK(letterToConsiderHint(start, data) != '?');

	freeDataStructures(data);
}

/* A hint hands over the commonest word it could, not a random one.
 *
 * It used to pick uniformly among the unused neighbours, so on a four letter
 * board it had about a one in eleven chance of naming the most obscure word
 * within reach. A hint is bought, usually while stuck and usually against a
 * clock, and naming a word the player has never met is the one thing it must
 * not do - they cannot act on it, cannot check it, and have paid for it.
 */
static void test_a_hint_hands_over_the_commonest_word_within_reach(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int i, checked = 0, wrong = 0, wouldHaveDiffered = 0;

	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	for(i = 0; i < data->I2W->numWords; i++){
		struct intList* c;
		int best = -1, worst = -1, hinted;

		for(c = getConnections(i, data->I2W)->next; c != NULL; c = c->next){
			int n = c->data;
			if(best == -1 || getObscurity(n, data) < getObscurity(best, data)){
				best = n;
			}
			if(worst == -1 || getObscurity(n, data) > getObscurity(worst, data)){
				worst = n;
			}
		}
		if(best == -1){
			continue;
		}

		hinted = directAdjacencyHint(i, data);
		checked++;
		if(getObscurity(hinted, data) != getObscurity(best, data)){
			wrong++;
		}
		/*Words where the old random pick could have answered differently, so
		the check above is not passing because every neighbour is alike*/
		if(getObscurity(worst, data) > getObscurity(best, data)){
			wouldHaveDiffered++;
		}
	}

	CHECK(checked > 3500);
	CHECK_INT(wrong, 0);
	CHECK(wouldHaveDiffered > 3000);

	freeDataStructures(data);
}

/* And it is never REFUSED for being obscure.
 *
 * The cap is what a board is dealt and what a bot may answer with. A hint is
 * neither: it is sorted rather than filtered, so a word the ranking undersells
 * loses a place in a queue rather than its existence. The player has already
 * paid, and silence is a worse answer than an odd word.
 */
static void test_a_hint_is_never_refused_for_being_obscure(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int i, askable = 0, refused = 0;

	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	/*Tighter than any board the game deals - only the commonest word in
	english would be inside it*/
	setObscurityCap(data, 1);

	for(i = 0; i < data->I2W->numWords; i++){
		if(getConnections(i, data->I2W)->next == NULL){
			continue;
		}
		askable++;
		if(directAdjacencyHint(i, data) == -1){
			refused++;
		}
	}

	CHECK(askable > 3500);
	CHECK_INT(refused, 0);

	freeDataStructures(data);
}

/* The letter hint points at the commonest word it could too.
 *
 * It keeps its older rule - prefer a letter the word does not already carry,
 * because "try a P" on PIG is not a hint - and among the neighbours that offer
 * a fresh letter it now names the one belonging to the commonest.
 */
static void test_the_letter_hint_points_at_the_commonest_word(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int i, checked = 0, wrong = 0;

	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	for(i = 0; i < data->I2W->numWords; i++){
		char* word = Convert_IntToWord(i, data->I2W);
		struct intList* c;
		int best = -1;
		char bestLetter = '?';

		for(c = getConnections(i, data->I2W)->next; c != NULL; c = c->next){
			char* neighbour = Convert_IntToWord(c->data, data->I2W);
			int k;
			for(k = 0; k < 4; k++){
				if(neighbour[k] != word[k]){
					if(strchr(word, neighbour[k]) == NULL
						&& (best == -1 || getObscurity(c->data, data) < getObscurity(best, data))){
						best = c->data;
						bestLetter = neighbour[k];
					}
					break;
				}
			}
		}
		if(best == -1){
			continue;
		}

		checked++;
		if(letterToConsiderHint(i, data) != bestLetter){
			wrong++;
		}
	}

	CHECK(checked > 3000);
	CHECK_INT(wrong, 0);

	freeDataStructures(data);
}

/* The count is of every legal move, and the cap does not touch it.
 *
 * This is a claim about the PLAYER's position and the player may type anything
 * in the dictionary, so the true number of ways out is all of them. Telling
 * somebody they have five when they can see six is how a hint stops being
 * believed - a number they can check has to be the one they would get.
 */
static void test_the_count_hint_counts_every_legal_move(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int i, moved = 0, sampled = 0;

	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	for(i = 0; i < data->I2W->numWords; i += 3){
		int wideOpen, capped;

		setObscurityCap(data, OBSCURITY_UNKNOWN);
		wideOpen = numOptionsHint(i, data);

		setObscurityCap(data, 800);
		capped = numOptionsHint(i, data);

		sampled++;
		if(wideOpen != capped){
			moved++;
		}
	}

	CHECK(sampled > 1000);
	CHECK_INT(moved, 0);

	freeDataStructures(data);
}

/* The ordering every one of those hints is built on.
 *
 * Four FLWC hints and the FLWP road all read one search apiece, and what they
 * name is decided by the order the neighbours come back in - so that order is
 * worth a test of its own rather than being inferred from a board.
 */
static void test_neighbours_come_back_commonest_first(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int i, checked = 0, outOfOrder = 0, lost = 0;

	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	for(i = 0; i < data->I2W->numWords; i++){
		int n = getNumAdjacencies(i, data);
		if(n <= 1){
			continue;
		}
		int got[n];
		int count = Neighbours_ByObscurity(i, got, n, data->I2W);
		int k;

		/*Nothing dropped on the way through*/
		if(count != n){
			lost++;
		}
		for(k = 1; k < count; k++){
			if(getObscurity(got[k - 1], data) > getObscurity(got[k], data)){
				outOfOrder++;
			}
		}
		checked++;
	}

	CHECK(checked > 3000);
	CHECK_INT(lost, 0);
	CHECK_INT(outOfOrder, 0);

	/*and it survives being asked about a word that is not one*/
	CHECK_INT(Neighbours_ByObscurity(-1, NULL, 0, data->I2W), 0);

	freeDataStructures(data);
}

/*The same null, reached the other way: a word the dictionary gives no
neighbours at all*/
static void test_letter_hint_answers_for_a_word_with_no_neighbours(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int i;
	int lonely = -1;

	for(i = 0; i < data->I2W->numWords && lonely == -1; i++){
		if(data->I2W->array[i]->numConnections == 0){
			lonely = i;
		}
	}

	CHECK(lonely != -1);
	if(lonely != -1){
		CHECK_INT(letterToConsiderHint(lonely, data), '?');
	}

	freeDataStructures(data);
}

/* A board that could not be dealt at all.
 *
 * The pathfinder's screen asks how long the route is as it opens, which is
 * before anything has told it the board could not be built - so on a set of
 * parameters no word satisfies it asks about a game that does not exist. The
 * null check only covered a built game with no solution in it, so a null game
 * read its solution field off address nothing and took the process with it.
 *
 * Both of these have to answer rather than crash: -1 is "no route", which is
 * the truth about a board that was never dealt.
 */
void test_flwp_hints_survive_a_game_that_was_never_built(void){
	CHECK_INT(hintGetMinAdjacenciesFLWP(NULL, NULL), -1);
	CHECK_INT(distanceToGoalFLWP(NULL, NULL), -1);
}

/* Asking for advice must not cost you the word.
 *
 * hintSafeMoveFLWG finds its answer by running the bot's own search from the
 * player's seat, and botPly PLAYS what it finds - it marks the word spent. A
 * hint that forgot to give it back would take the best move on the board away
 * from the player at the exact moment they paid to be told about it, and the
 * board would look normal afterwards: one word quietly missing from a set
 * nobody prints.
 *
 * So: the word set is counted before and after, the answer is checked to be a
 * word the player could still type, and the whole thing is done twice - asking
 * the same question twice has to give the same answer, which it would not if
 * the first ask had eaten its own suggestion.
 */
void test_the_safe_move_hint_does_not_spend_the_word(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWG(data, 4, 30);
	CHECK(game != NULL);

	if(game != NULL){
		int i, usedBefore = 0, usedAfter = 0;
		char* first;
		char* second;

		for(i = 0; i < data->I2W->numWords; i++){
			if(checkIfUsed_WordSet(i, data->wordSet)){
				usedBefore++;
			}
		}

		first = hintSafeMoveFLWG(game, 2, data);
		CHECK(first != NULL);

		for(i = 0; i < data->I2W->numWords; i++){
			if(checkIfUsed_WordSet(i, data->wordSet)){
				usedAfter++;
			}
		}
		CHECK_INT(usedAfter, usedBefore);

		if(first != NULL){
			/*It is a move the player can make: one letter off, and not spent*/
			char* here = getCurrWord(game, data);
			int differ = 0;
			for(i = 0; i < 4; i++){
				if(here[i] != first[i]){
					differ++;
				}
			}
			CHECK_INT(differ, 1);
			CHECK_INT(checkIfUsed_WordSet(convertWordToInt(first, data), data->wordSet), 0);

			/*And the same question still has the same answer*/
			second = hintSafeMoveFLWG(game, 2, data);
			CHECK(second != NULL);
			if(second != NULL){
				CHECK_INT(strcmp(first, second), 0);
			}
		}

		freeGameComponentsFLWG(game);
	}

	freeDataStructures(data);
}

void suite_regressions(void){
	printf("\n-- regressions --\n");
	RUN_TEST(test_init_leaves_the_caller_owning_the_fd);
	RUN_TEST(test_flwc_hints_handle_an_unreachable_goal);
	RUN_TEST(test_flwc_hints_still_answer_a_reachable_goal);
	RUN_TEST(test_choose_start_widens_rather_than_reporting_no_match);
	RUN_TEST(test_botply_gives_up_when_every_word_is_used);
	RUN_TEST(test_duplicate_keys_do_not_break_the_map);
	RUN_TEST(test_remove_word_rewinds_the_path);
	RUN_TEST(test_letter_hint_prefers_a_letter_the_word_does_not_have);
	RUN_TEST(test_letter_hint_answers_when_every_neighbour_is_spent);
	RUN_TEST(test_letter_hint_answers_for_a_word_with_no_neighbours);
	RUN_TEST(test_a_hint_hands_over_the_commonest_word_within_reach);
	RUN_TEST(test_a_hint_is_never_refused_for_being_obscure);
	RUN_TEST(test_the_letter_hint_points_at_the_commonest_word);
	RUN_TEST(test_the_count_hint_counts_every_legal_move);
	RUN_TEST(test_neighbours_come_back_commonest_first);
	RUN_TEST(test_flwp_hints_survive_a_game_that_was_never_built);
	RUN_TEST(test_the_safe_move_hint_does_not_spend_the_word);
}
