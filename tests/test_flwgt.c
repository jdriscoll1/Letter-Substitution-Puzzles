/*
Title: test_flwgt.c
Description: The generalized turns game - name words a given distance from a set.

This mode is not a walk, which is what makes it worth testing carefully: every
other game in here validates a move against the word you are standing on, and
this one validates a word against a rule about the whole dictionary. There is
no board, no start word and no chain, so the usual safety net - "it has to be
one letter off the last one" - is not there to catch anything.

The property everything else rests on is that the distances are a fact about
the GRAPH. They are worked out once, and naming a word must never move another
word. Get that wrong and the rule the level states quietly stops being true
halfway through playing it, which is the kind of thing nobody reports as a bug
because it just feels unfair.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/flwgt/includes/FLWGT.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"
#include "../src/structs/includes/WordSet.h"

struct DataStructures* open_dictionary(const char* path, int numLetters);

/* A word that is exactly `want` from the seed, found off the graph rather than
   written down here, so this does not carry a copy of the dictionary. */
static int someWordAt(int* distance, int total, int want, int notThis){
	int i;
	for(i = 0; i < total; i++){
		if(distance[i] == want && i != notThis){
			return i;
		}
	}
	return -1;
}

/* ------------------------------------------------------- the measuring */

/* Multi-source is the whole trick: one pass from every seed at once, rather
   than a search per seed and a minimum afterwards. This checks it IS that
   minimum, over the entire dictionary, by doing it the slow way too. */
void test_flwgt_measures_from_the_nearest_seed(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int total = data->I2W->numWords;

	int a = 0, b = total / 2;
	int pair[2] = { a, b };

	int one[1] = { a };
	int two[1] = { b };
	int* fromA = distancesFromSeeds(one, 1, data);
	int* fromB = distancesFromSeeds(two, 1, data);
	int* fromBoth = distancesFromSeeds(pair, 2, data);

	CHECK(fromA != NULL);
	CHECK(fromB != NULL);
	CHECK(fromBoth != NULL);

	int i, checked = 0, wrong = 0, differed = 0;
	for(i = 0; i < total; i++){
		int nearest;
		if(fromA[i] == -1){ nearest = fromB[i]; }
		else if(fromB[i] == -1){ nearest = fromA[i]; }
		else { nearest = fromA[i] < fromB[i] ? fromA[i] : fromB[i]; }

		checked++;
		if(fromBoth[i] != nearest){ wrong++; }
		if(fromA[i] != fromB[i]){ differed++; }
	}

	CHECK(checked > 100);
	CHECK_INT(wrong, 0);
	/*and the two seeds really do disagree about most words, so the check above
	  is not passing because they are the same search*/
	CHECK(differed > checked / 2);

	free(fromA);
	free(fromB);
	free(fromBoth);
	freeDataStructures(data);
}

void test_flwgt_a_seed_is_no_distance_from_itself(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	CHECK(seed >= 0);

	int seeds[1] = { seed };
	int* distance = distancesFromSeeds(seeds, 1, data);
	CHECK(distance != NULL);
	CHECK_INT(distance[seed], 0);

	/*and every neighbour of it is exactly one*/
	struct intList* conn = getConnections(seed, data->I2W);
	int neighbours = 0;
	for(conn = conn->next; conn != NULL; conn = conn->next){
		CHECK_INT(distance[conn->data], 1);
		neighbours++;
	}
	CHECK(neighbours > 0);

	free(distance);
	freeDataStructures(data);
}

/* A word the seeds cannot reach at all is -1, not a large number and not zero.
   Zero would make it a seed and a large number would make it an answer to some
   band, both of which are worse than saying there is no path. */
void test_flwgt_says_when_there_is_no_way_there(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int total = data->I2W->numWords;

	/*a word with no neighbours at all cannot be reached from anywhere else*/
	int lonely = -1, i;
	for(i = 0; i < total && lonely == -1; i++){
		if(data->I2W->array[i]->numConnections == 0){
			lonely = i;
		}
	}
	CHECK(lonely != -1);

	if(lonely != -1){
		int seeds[1] = { 0 };
		int* distance = distancesFromSeeds(seeds, 1, data);
		CHECK(distance != NULL);
		CHECK_INT(distance[lonely], -1);
		free(distance);
	}

	freeDataStructures(data);
}

/* ------------------------------------------------------- playing a board */

void test_flwgt_takes_a_word_at_the_right_distance(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };

	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 3, data);
	CHECK(game != NULL);

	int* distance = distancesFromSeeds(seeds, 1, data);
	int twoAway = someWordAt(distance, data->I2W->numWords, 2, -1);
	int oneAway = someWordAt(distance, data->I2W->numWords, 1, -1);
	CHECK(twoAway != -1);
	CHECK(oneAway != -1);

	/*the one that fits counts*/
	CHECK_INT(userEntersWordFLWGT(Convert_IntToWord(twoAway, data->I2W), game, data), VALID);
	CHECK_INT(wordsFoundFLWGT(game), 1);

	/*and the one that does not is refused for the reason it was refused - not
	  as a bad word, which is what the board says out loud*/
	CHECK_INT(userEntersWordFLWGT(Convert_IntToWord(oneAway, data->I2W), game, data), WRONG_DISTANCE);
	CHECK_INT(wordsFoundFLWGT(game), 1);

	/*the seed itself is not two from itself either*/
	CHECK_INT(userEntersWordFLWGT("cat", game, data), WRONG_DISTANCE);

	free(distance);
	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

/* The band, rather than a single number. "Within two" has to admit the things
   "exactly two" refuses, or a level cannot ask the easier question. */
void test_flwgt_a_band_admits_what_an_exact_distance_refuses(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };

	int* distance = distancesFromSeeds(seeds, 1, data);
	int oneAway = someWordAt(distance, data->I2W->numWords, 1, -1);
	CHECK(oneAway != -1);
	char* word = Convert_IntToWord(oneAway, data->I2W);

	struct GameComponentsFLWGT* exact = initFLWGT(seeds, 1, 2, 2, 1, data);
	CHECK_INT(userEntersWordFLWGT(word, exact, data), WRONG_DISTANCE);
	freeGameComponentsFLWGT(exact);

	resetWordSet(data);
	struct GameComponentsFLWGT* within = initFLWGT(seeds, 1, 0, 2, 1, data);
	CHECK_INT(userEntersWordFLWGT(word, within, data), VALID);
	freeGameComponentsFLWGT(within);

	free(distance);
	freeDataStructures(data);
}

void test_flwgt_wants_as_many_words_as_it_asked_for(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };

	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 3, data);
	CHECK(game != NULL);
	CHECK_INT(wordsWantedFLWGT(game), 3);
	CHECK_INT(isGameWonFLWGT(game), 0);

	int i;
	for(i = 0; i < 3; i++){
		int answer = anAnswerFLWGT(game, data);
		CHECK(answer != -1);
		CHECK_INT(isGameWonFLWGT(game), 0);
		CHECK_INT(userEntersWordFLWGT(Convert_IntToWord(answer, data->I2W), game, data), VALID);
	}

	CHECK_INT(wordsFoundFLWGT(game), 3);
	CHECK_INT(isGameWonFLWGT(game), 1);

	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

void test_flwgt_will_not_take_the_same_word_twice(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };

	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 3, data);
	int answer = anAnswerFLWGT(game, data);
	CHECK(answer != -1);
	char* word = Convert_IntToWord(answer, data->I2W);

	CHECK_INT(userEntersWordFLWGT(word, game, data), VALID);
	CHECK_INT(userEntersWordFLWGT(word, game, data), WORD_USED);
	CHECK_INT(wordsFoundFLWGT(game), 1);

	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

void test_flwgt_refuses_what_is_not_a_word(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };
	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 3, data);

	/*"qqq" rather than "zzz". ZZZ IS IN THE DICTIONARY - it is on line 1011 of
	  docs/3.txt with no connections at all - so it comes back as the wrong
	  distance rather than as no word, which is correct and was not what this
	  test first claimed. Worth the note: guessing which letter salad is absent
	  is how a test ends up asserting something about the dictionary it did not
	  mean to.*/
	CHECK_INT(userEntersWordFLWGT("qqq", game, data), WORD_DOES_NOT_EXIST);
	/*and the unreachable real word is refused for the right reason*/
	CHECK_INT(userEntersWordFLWGT("zzz", game, data), WRONG_DISTANCE);
	CHECK_INT(userEntersWordFLWGT("ca", game, data), TOO_SHORT);
	CHECK_INT(userEntersWordFLWGT("cart", game, data), TOO_LONG);
	CHECK_INT(wordsFoundFLWGT(game), 0);

	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

/* THE ONE THAT MATTERS. The rule the level states is a fact about the graph,
   so it has to still be true on the last answer as it was on the first. */
void test_flwgt_naming_a_word_moves_nothing(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };
	int total = data->I2W->numWords;

	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 5, data);
	CHECK(game != NULL);

	int* before = malloc(sizeof(int) * total);
	int i;
	for(i = 0; i < total; i++){
		before[i] = distanceOfWordFLWGT(Convert_IntToWord(i, data->I2W), game, data);
	}

	for(i = 0; i < 5; i++){
		int answer = anAnswerFLWGT(game, data);
		if(answer == -1){ break; }
		CHECK_INT(userEntersWordFLWGT(Convert_IntToWord(answer, data->I2W), game, data), VALID);
	}
	CHECK_INT(isGameWonFLWGT(game), 1);

	int moved = 0;
	for(i = 0; i < total; i++){
		if(distanceOfWordFLWGT(Convert_IntToWord(i, data->I2W), game, data) != before[i]){
			moved++;
		}
	}
	CHECK_INT(moved, 0);

	free(before);
	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

/* --------------------------------------------- what the board can be asked */

void test_flwgt_counts_what_is_left_to_find(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };

	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 3, data);
	int atFirst = answersLeftFLWGT(game, data);
	CHECK(atFirst > 3);

	int answer = anAnswerFLWGT(game, data);
	CHECK_INT(userEntersWordFLWGT(Convert_IntToWord(answer, data->I2W), game, data), VALID);

	CHECK_INT(answersLeftFLWGT(game, data), atFirst - 1);

	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

/* A board asking for three words when there are only two is not a hard board,
   it is one that should never have been dealt. */
void test_flwgt_knows_when_a_board_cannot_be_finished(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	int seed = Convert_WordToInt("cat", data);
	int seeds[1] = { seed };

	struct GameComponentsFLWGT* fine = initFLWGT(seeds, 1, 2, 2, 3, data);
	CHECK_INT(isSolvableFLWGT(fine), 1);
	freeGameComponentsFLWGT(fine);

	/*nothing is a hundred moves away in a dictionary this small*/
	struct GameComponentsFLWGT* hopeless = initFLWGT(seeds, 1, 100, 100, 1, data);
	CHECK_INT(isSolvableFLWGT(hopeless), 0);
	CHECK_INT(anAnswerFLWGT(hopeless, data), -1);
	freeGameComponentsFLWGT(hopeless);

	/*and asking for more than there are is caught too*/
	struct GameComponentsFLWGT* greedy = initFLWGT(seeds, 1, 0, 0, 2, data);
	CHECK_INT(isSolvableFLWGT(greedy), 0);
	freeGameComponentsFLWGT(greedy);

	freeDataStructures(data);
}

/* Solvability is counted in words the GAME would use. The cap is what decides
   which those are, and it must not decide what the player may type. */
void test_flwgt_is_solvable_in_words_the_game_would_deal(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	int seed = Convert_WordToInt("cart", data);
	CHECK(seed >= 0);
	int seeds[1] = { seed };

	setObscurityCap(data, OBSCURITY_UNKNOWN);
	struct GameComponentsFLWGT* wideOpen = initFLWGT(seeds, 1, 2, 2, 1, data);
	int everything = wideOpen->answersWorthAsking;

	setObscurityCap(data, 800);
	struct GameComponentsFLWGT* narrow = initFLWGT(seeds, 1, 2, 2, 1, data);
	int common = narrow->answersWorthAsking;

	/*the same board, counted twice, and the cap really does bite*/
	CHECK_INT(wideOpen->answersTotal, narrow->answersTotal);
	CHECK(common < everything);

	/*but a word past the cap is still a perfectly good answer when a player
	  brings it themselves*/
	int i, obscureAnswer = -1;
	for(i = 0; i < narrow->numWords && obscureAnswer == -1; i++){
		if(narrow->distance[i] == 2 && isTooObscure(i, data)){
			obscureAnswer = i;
		}
	}
	CHECK(obscureAnswer != -1);
	if(obscureAnswer != -1){
		CHECK_INT(userEntersWordFLWGT(Convert_IntToWord(obscureAnswer, data->I2W), narrow, data), VALID);
	}

	freeGameComponentsFLWGT(wideOpen);
	freeGameComponentsFLWGT(narrow);
	freeDataStructures(data);
}

/* The hint is the engine choosing a word to put in front of somebody, so it
   sorts commonest first the way every other hint in here does. */
void test_flwgt_offers_the_commonest_answer_first(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	Load_Obscurity(data->I2W, "docs/4ranks.txt");
	setObscurityCap(data, OBSCURITY_UNKNOWN);

	int seed = Convert_WordToInt("cart", data);
	int seeds[1] = { seed };
	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 3, data);

	int offered = anAnswerFLWGT(game, data);
	CHECK(offered != -1);

	int i, commoner = 0;
	for(i = 0; i < game->numWords; i++){
		if(game->distance[i] == 2 && getObscurity(i, data) < getObscurity(offered, data)){
			commoner++;
		}
	}
	CHECK_INT(commoner, 0);

	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

/* Several seeds, which is the case a walk cannot express at all: no word is
   ever standing on two places at once, but plenty of words are three from one
   word and three from another. */
void test_flwgt_can_be_asked_about_two_words_at_once(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int pies = Convert_WordToInt("pies", data);
	int test = Convert_WordToInt("test", data);
	CHECK(pies >= 0);
	CHECK(test >= 0);

	int seeds[2] = { pies, test };
	struct GameComponentsFLWGT* game = initFLWGT(seeds, 2, 0, 0, 2, data);
	CHECK(game != NULL);

	/*both seeds are zero from the set, and nothing else is*/
	CHECK_INT(game->distance[pies], 0);
	CHECK_INT(game->distance[test], 0);
	CHECK_INT(game->answersTotal, 2);

	freeGameComponentsFLWGT(game);
	freeDataStructures(data);
}

void test_flwgt_survives_being_handed_nothing(void){
	CHECK(distancesFromSeeds(NULL, 0, NULL) == NULL);
	CHECK(initFLWGT(NULL, 0, 1, 1, 1, NULL) == NULL);
	CHECK_INT(isSolvableFLWGT(NULL), 0);
	CHECK_INT(isGameWonFLWGT(NULL), 0);
	CHECK_INT(wordsFoundFLWGT(NULL), 0);
	CHECK_INT(wordsWantedFLWGT(NULL), 0);
	CHECK_INT(answersLeftFLWGT(NULL, NULL), 0);
	CHECK_INT(anAnswerFLWGT(NULL, NULL), -1);
	CHECK_INT(distanceOfWordFLWGT("cat", NULL, NULL), -2);
	CHECK_INT(userEntersWordFLWGT("cat", NULL, NULL), UNKNOWN_ERROR);
	freeGameComponentsFLWGT(NULL);
}

void suite_flwgt(void){
	printf("\n-- the generalized turns game --\n");
	RUN_TEST(test_flwgt_measures_from_the_nearest_seed);
	RUN_TEST(test_flwgt_a_seed_is_no_distance_from_itself);
	RUN_TEST(test_flwgt_says_when_there_is_no_way_there);
	RUN_TEST(test_flwgt_takes_a_word_at_the_right_distance);
	RUN_TEST(test_flwgt_a_band_admits_what_an_exact_distance_refuses);
	RUN_TEST(test_flwgt_wants_as_many_words_as_it_asked_for);
	RUN_TEST(test_flwgt_will_not_take_the_same_word_twice);
	RUN_TEST(test_flwgt_refuses_what_is_not_a_word);
	RUN_TEST(test_flwgt_naming_a_word_moves_nothing);
	RUN_TEST(test_flwgt_counts_what_is_left_to_find);
	RUN_TEST(test_flwgt_knows_when_a_board_cannot_be_finished);
	RUN_TEST(test_flwgt_is_solvable_in_words_the_game_would_deal);
	RUN_TEST(test_flwgt_offers_the_commonest_answer_first);
	RUN_TEST(test_flwgt_can_be_asked_about_two_words_at_once);
	RUN_TEST(test_flwgt_survives_being_handed_nothing);
}
