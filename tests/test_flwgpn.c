/*
Title: test_flwgpn.c
Description: The walk whose ports are rules rather than words.

It is the chain with the ports described instead of named, so most of what it
does is the chain's and the chain is already tested. What is new is the
consequences of a port being a SET:

  - ANY member finishes the leg, not the one the dealer aimed at. That is the
    whole mode, and it is the one thing that would still "work" if it were
    broken - a board where only the dealer's word counts is a perfectly
    playable board of the wrong game.

  - ONE WORD ANSWERS ONE PORT. Sets overlap, so a word could settle two legs at
    once. It settles the earliest it can and the player still has to move.

  - NOTHING IS SPENT AND NOTHING IS FORBIDDEN. The chain needs both rules
    because its ports are single words that touching would destroy; a set
    survives having a member touched. Tested because they were dropped
    deliberately rather than forgotten.

  - THE AIM FOLLOWS THE PLAYER. walk->goal is a word, because distanceToGoal,
    the route hint and the star marking all read one, and it is re-aimed at the
    nearest member of the due rule after every move.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/flwgpn/includes/FLWGPN.h"
#include "../src/flwp/includes/PathGameComponents.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"
#include "../src/structs/includes/WordSet.h"

struct DataStructures* open_dictionary(const char* path, int numLetters);

/* ------------------------------------------------------------ making rules */

/* Every word in the dictionary whose letter at `at` is `letter`, as the
   NULL-terminated list the engine takes. A rule a player could be told in
   words - "a word starting with B" - and small enough to reason about. */
static char** wordsWithLetterAt(int at, char letter, struct DataStructures* data){
	int total = data->I2W->numWords;
	char** out = malloc(sizeof(char*) * (total + 1));
	int i, kept = 0;

	for(i = 0; i < total; i++){
		char* word = convertIntToWord(i, data);
		if(word != NULL && word[at] == letter){
			out[kept++] = word;
		}
	}
	out[kept] = NULL;
	return out;
}

/*How many words a NULL-terminated list holds*/
static int howMany(char** words){
	int n = 0;
	while(words[n] != NULL){
		n++;
	}
	return n;
}

/* ------------------------------------------------------------ walking one */

/* The shortest route from where the board is to `to`, as word ids.
 *
 * AROUND `avoid`, which is always the word the board opened on. That is the one
 * word an ordinary walk spends - FinishGameComponents marks it used - so a
 * route back through it is refused move by move, and a helper that walked into
 * it would read exactly like a broken engine. */
static int routeTo(int from, int to, int avoid, struct DataStructures* data,
	int* route, int max){
	int total = data->I2W->numWords;
	int* cameFrom = malloc(sizeof(int) * total);
	int* queue = malloc(sizeof(int) * total);
	int i, head = 0, tail = 0, found = 0;

	for(i = 0; i < total; i++){
		cameFrom[i] = -2;
	}
	cameFrom[from] = -1;
	queue[tail++] = from;

	while(head < tail && !found){
		int curr = queue[head++];
		struct intList* c = getConnections(curr, data->I2W);
		if(c == NULL){
			continue;
		}
		for(c = c->next; c != NULL && !found; c = c->next){
			int next = c->data;
			if(cameFrom[next] != -2 || (next == avoid && next != to)){
				continue;
			}
			cameFrom[next] = curr;
			if(next == to){
				found = 1;
				break;
			}
			queue[tail++] = next;
		}
	}

	int length = 0;
	if(found || from == to){
		int backwards[128];
		int at = to, steps = 0;
		while(at != -1 && at != -2 && steps < 128){
			backwards[steps++] = at;
			at = cameFrom[at];
		}
		for(i = steps - 1; i >= 0 && length < max; i--){
			route[length++] = backwards[i];
		}
	}

	free(cameFrom);
	free(queue);
	return length;
}

/*Play the board to `to`, one move at a time. -1 if any move was refused.*/
static int playTo(struct GameComponentsFLWGPN* game, int to, struct DataStructures* data){
	struct GameComponents* walk = getFLWPComponentsFLWGPN(game);
	int at = walk->prevInput >= 0 ? walk->prevInput : walk->start;
	int route[64];
	int length = routeTo(at, to, walk->start, data, route, 64);
	int i, moves = 0;

	if(length < 1){
		return -1;
	}
	for(i = 1; i < length; i++){
		if(userEntersWordFLWGPN(convertIntToWord(route[i], data), game, data) != VALID){
			return -1;
		}
		moves++;
	}
	return moves;
}

/* --------------------------------------------------------------- the tests */

void test_flwgpn_deals_a_board_with_a_rule_per_leg(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(11);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	CHECK(howMany(withB) > 20);
	CHECK(howMany(withS) > 20);

	struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
	CHECK(game != NULL);

	if(game != NULL){
		CHECK_INT(legsTotalFLWGPN(game), 2);
		CHECK_INT(legsDoneFLWGPN(game), 0);
		CHECK_INT(isStartValidFLWGPN(game), 1);
		CHECK_INT(isGameWonFLWGPN(game), 0);

		/*it aims at a word the first rule admits, and measures the whole chain*/
		int aim = currentGoalFLWGPN(game);
		CHECK(aim >= 0);
		CHECK_INT(satisfiesCurrentRuleFLWGPN(aim, game), 1);
		CHECK(convertIntToWord(aim, data)[0] == 'b');
		CHECK(shortestRouteFLWGPN(game) >= 2);

		freeGameComponentsFLWGPN(game, data);
	}

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/* THE WHOLE MODE. A board where only the dealer's word finishes the leg is a
   playable board of a different game, so this walks to a DIFFERENT member of
   the rule than the one the board is aiming at and expects the leg to close. */
void test_flwgpn_any_word_the_rule_admits_finishes_the_leg(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(3);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	int tried, worked = 0;
	for(tried = 0; tried < 24 && !worked; tried++){
		struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
		if(game == NULL){
			continue;
		}

		int aimed = currentGoalFLWGPN(game);
		struct GameComponents* walk = getFLWPComponentsFLWGPN(game);

		/* Step off the board's own line and come back to the rule somewhere
		   else: a neighbour of the start that the rule does NOT admit, and then
		   one of ITS neighbours that the rule does, which is not the word the
		   board is aiming at. Two moves, found by looking at adjacency lists -
		   no search, because the answer only has to exist, not be shortest. */
		int step = -1, land = -1;
		struct intList* c = getConnections(walk->start, data->I2W);
		for(c = c == NULL ? NULL : c->next; c != NULL && land == -1; c = c->next){
			struct intList* d;
			if(checkIfUsed_WordSet(c->data, game->goals[0])){
				continue;
			}
			d = getConnections(c->data, data->I2W);
			for(d = d == NULL ? NULL : d->next; d != NULL; d = d->next){
				if(d->data != aimed && d->data != walk->start
					&& checkIfUsed_WordSet(d->data, game->goals[0])){
					step = c->data;
					land = d->data;
					break;
				}
			}
		}

		if(land != -1
			&& userEntersWordFLWGPN(convertIntToWord(step, data), game, data) == VALID
			&& userEntersWordFLWGPN(convertIntToWord(land, data), game, data) == VALID){
			CHECK_INT(legsDoneFLWGPN(game), 1);
			CHECK(walk->prevInput == land);
			CHECK(land != aimed);
			/*and the board now aims at the SECOND rule*/
			CHECK_INT(satisfiesCurrentRuleFLWGPN(currentGoalFLWGPN(game), game), 1);
			CHECK(convertIntToWord(currentGoalFLWGPN(game), data)[3] == 's');
			worked = 1;
		}

		freeGameComponentsFLWGPN(game, data);
	}

	/*not an escape hatch: if no board could be probed the test has proved
	  nothing and says so rather than passing quietly*/
	CHECK_INT(worked, 1);

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/* Sets overlap. BUBS starts with a B and ends with an S, so it could answer
   both rules at once - and must not. */
void test_flwgpn_one_word_answers_one_rule(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(5);

	/* THE SAME RULE TWICE, which is the sharpest form of the question: every
	   word that answers the first also answers the second, so a word that
	   settled both would finish the board in one move. Two overlapping but
	   different rules would ask the same thing and only sometimes - whether a
	   word in both sets happens to sit near the start is up to the draw. */
	char** withB = wordsWithLetterAt(0, 'b', data);
	char** sets[2] = { withB, withB };

	struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
	CHECK(game != NULL);

	if(game != NULL){
		int first = currentGoalFLWGPN(game);
		CHECK(first >= 0);

		CHECK(playTo(game, first, data) > 0);

		/*one leg, not two, and there is still a board to play*/
		CHECK_INT(legsDoneFLWGPN(game), 1);
		CHECK_INT(isGameWonFLWGPN(game), 0);

		/*and it is now aiming at some OTHER word the same rule admits*/
		int second = currentGoalFLWGPN(game);
		CHECK(second >= 0);
		CHECK(second != first);
		CHECK(convertIntToWord(second, data)[0] == 'b');

		freeGameComponentsFLWGPN(game, data);
	}

	free(withB);
	freeDataStructures(data);
}

/* The chain refuses a port that is not due, because touching it would spend it
   and kill the board. A set cannot be killed that way, so this must NOT. */
void test_flwgpn_lets_you_touch_a_later_rules_word(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(13);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	int tried, worked = 0;
	for(tried = 0; tried < 12 && !worked; tried++){
		struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
		if(game == NULL){
			continue;
		}

		struct GameComponents* walk = getFLWPComponentsFLWGPN(game);

		/*a neighbour of the start that the SECOND rule admits and the first
		  does not - a word the chain would have refused outright*/
		struct intList* c = getConnections(walk->start, data->I2W);
		int step = -1;
		for(c = c->next; c != NULL && step == -1; c = c->next){
			if(checkIfUsed_WordSet(c->data, game->goals[1])
				&& !checkIfUsed_WordSet(c->data, game->goals[0])){
				step = c->data;
			}
		}

		if(step != -1){
			CHECK_INT(userEntersWordFLWGPN(convertIntToWord(step, data), game, data), VALID);
			/*taken, and it did not count for anything*/
			CHECK_INT(legsDoneFLWGPN(game), 0);
			worked = 1;
		}

		freeGameComponentsFLWGPN(game, data);
	}

	CHECK_INT(worked, 1);

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/* And no word is spent: the same word may be walked through twice, which the
   chain forbids and an ordinary walk has always allowed. */
void test_flwgpn_spends_nothing_on_the_way(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(17);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
	CHECK(game != NULL);

	if(game != NULL){
		struct GameComponents* walk = getFLWPComponentsFLWGPN(game);
		int start = walk->start;

		/*step away and step back to the word the board opened on*/
		struct intList* c = getConnections(start, data->I2W);
		int away = c->next != NULL ? c->next->data : -1;
		CHECK(away >= 0);

		if(away >= 0){
			CHECK_INT(userEntersWordFLWGPN(convertIntToWord(away, data), game, data), VALID);
			/*the word the board STARTED on is the one word a walk does spend,
			  so going back to it is refused - by the walk, as it always was*/
			int back = userEntersWordFLWGPN(convertIntToWord(start, data), game, data);
			CHECK(back == WORD_USED);

			/*but any other word on the way is free to be revisited*/
			struct intList* d = getConnections(away, data->I2W);
			int onward = -1;
			for(d = d->next; d != NULL && onward == -1; d = d->next){
				if(d->data != start){ onward = d->data; }
			}
			if(onward >= 0){
				CHECK_INT(userEntersWordFLWGPN(convertIntToWord(onward, data), game, data), VALID);
				CHECK_INT(userEntersWordFLWGPN(convertIntToWord(away, data), game, data), VALID);
			}
		}

		freeGameComponentsFLWGPN(game, data);
	}

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/* The aim follows the player. Standing next to a word the rule admits, the
   board must say one move, not the distance to wherever the dealer looked. */
void test_flwgpn_aims_at_the_nearest_word_the_rule_admits(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(23);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	int tried, worked = 0;
	for(tried = 0; tried < 12 && !worked; tried++){
		struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 4, 1, 30, data);
		if(game == NULL){
			continue;
		}

		struct GameComponents* walk = getFLWPComponentsFLWGPN(game);

		/*a neighbour of the start that is NOT in the first set, but which has a
		  neighbour that is - so from there the rule is exactly one move off*/
		struct intList* c = getConnections(walk->start, data->I2W);
		int step = -1;
		for(c = c->next; c != NULL && step == -1; c = c->next){
			if(checkIfUsed_WordSet(c->data, game->goals[0])){
				continue;
			}
			struct intList* d = getConnections(c->data, data->I2W);
			for(d = d->next; d != NULL; d = d->next){
				if(checkIfUsed_WordSet(d->data, game->goals[0])){
					step = c->data;
					break;
				}
			}
		}

		if(step != -1
			&& userEntersWordFLWGPN(convertIntToWord(step, data), game, data) == VALID){
			int aim = currentGoalFLWGPN(game);
			CHECK(aim >= 0);
			CHECK_INT(satisfiesCurrentRuleFLWGPN(aim, game), 1);
			/*one move away, which is only true if it was re-aimed*/
			int route[8];
			CHECK_INT(routeTo(step, aim, walk->start, data, route, 8), 2);
			worked = 1;
		}

		freeGameComponentsFLWGPN(game, data);
	}

	CHECK_INT(worked, 1);

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/*An undo that steps back over an arrival has to un-satisfy that rule*/
void test_flwgpn_undo_gives_back_the_rule(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(29);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	int tried, worked = 0;
	for(tried = 0; tried < 12 && !worked; tried++){
		struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
		if(game == NULL){
			continue;
		}

		if(playTo(game, currentGoalFLWGPN(game), data) > 0){
			CHECK_INT(legsDoneFLWGPN(game), 1);

			undoMoveFLWGPN(game, data);
			CHECK_INT(legsDoneFLWGPN(game), 0);
			/*and it is aiming at the first rule again*/
			CHECK(convertIntToWord(currentGoalFLWGPN(game), data)[0] == 'b');

			redoMoveFLWGPN(game, data);
			CHECK_INT(legsDoneFLWGPN(game), 1);
			worked = 1;
		}

		freeGameComponentsFLWGPN(game, data);
	}

	CHECK_INT(worked, 1);

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/*Reset gives up the route and goes back to the first rule*/
void test_flwgpn_reset_goes_back_to_the_first_rule(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(31);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	int tried, worked = 0;
	for(tried = 0; tried < 12 && !worked; tried++){
		struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 5, 1, 30, data);
		if(game == NULL){
			continue;
		}

		if(playTo(game, currentGoalFLWGPN(game), data) > 0){
			struct GameComponents* walk = getFLWPComponentsFLWGPN(game);
			int start = walk->start;

			resetFLWGPN(game, data);
			CHECK_INT(legsDoneFLWGPN(game), 0);
			CHECK_INT(isGameWonFLWGPN(game), 0);
			CHECK_INT(walk->start, start);
			CHECK(convertIntToWord(currentGoalFLWGPN(game), data)[0] == 'b');
			worked = 1;
		}

		freeGameComponentsFLWGPN(game, data);
	}

	CHECK_INT(worked, 1);

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/*Every board it hands over can actually be finished, rule after rule*/
void test_flwgpn_every_board_it_deals_can_be_finished(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(37);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char** withS = wordsWithLetterAt(3, 's', data);
	char** sets[2] = { withB, withS };

	int round, dealt = 0;
	for(round = 0; round < 10; round++){
		struct GameComponentsFLWGPN* game = initiateFLWGPN(8, 30, sets, 2, 2, 4, 1, 30, data);
		if(game == NULL){
			continue;
		}
		dealt++;

		int leg, ok = 1;
		for(leg = 0; leg < 2 && ok; leg++){
			int aim = currentGoalFLWGPN(game);
			if(aim < 0 || playTo(game, aim, data) < 1){
				ok = 0;
			}
		}

		CHECK_INT(ok, 1);
		CHECK_INT(isGameWonFLWGPN(game), 1);
		freeGameComponentsFLWGPN(game, data);
	}

	CHECK(dealt >= 8);

	free(withB);
	free(withS);
	freeDataStructures(data);
}

/*A rule nothing satisfies is a board that cannot be dealt, not a crash*/
void test_flwgpn_refuses_a_rule_nothing_satisfies(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(41);

	char** withB = wordsWithLetterAt(0, 'b', data);
	char* nothing[1] = { NULL };
	char** sets[2] = { withB, nothing };

	CHECK(initiateFLWGPN(8, 30, sets, 2, 2, 4, 1, 30, data) == NULL);

	free(withB);
	freeDataStructures(data);
}

/*Every call, handed nothing*/
void test_flwgpn_survives_being_handed_nothing(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	CHECK(initiateFLWGPN(8, 30, NULL, 2, 2, 4, 1, 30, data) == NULL);
	CHECK(getFLWPComponentsFLWGPN(NULL) == NULL);
	CHECK_INT(isStartValidFLWGPN(NULL), 0);
	CHECK_INT(legsDoneFLWGPN(NULL), 0);
	CHECK_INT(legsTotalFLWGPN(NULL), 0);
	CHECK_INT(shortestRouteFLWGPN(NULL), 0);
	CHECK_INT(currentGoalFLWGPN(NULL), -1);
	CHECK_INT(satisfiesCurrentRuleFLWGPN(0, NULL), 0);
	CHECK_INT(userEntersWordFLWGPN("bugs", NULL, data), -1);
	CHECK_INT(isGameWonFLWGPN(NULL), 0);
	undoMoveFLWGPN(NULL, data);
	redoMoveFLWGPN(NULL, data);
	resetFLWGPN(NULL, data);
	freeGameComponentsFLWGPN(NULL, data);

	freeDataStructures(data);
}

void suite_flwgpn(void){
	printf("\n-- the walk whose ports are rules --\n");
	RUN_TEST(test_flwgpn_deals_a_board_with_a_rule_per_leg);
	RUN_TEST(test_flwgpn_any_word_the_rule_admits_finishes_the_leg);
	RUN_TEST(test_flwgpn_one_word_answers_one_rule);
	RUN_TEST(test_flwgpn_lets_you_touch_a_later_rules_word);
	RUN_TEST(test_flwgpn_spends_nothing_on_the_way);
	RUN_TEST(test_flwgpn_aims_at_the_nearest_word_the_rule_admits);
	RUN_TEST(test_flwgpn_undo_gives_back_the_rule);
	RUN_TEST(test_flwgpn_reset_goes_back_to_the_first_rule);
	RUN_TEST(test_flwgpn_every_board_it_deals_can_be_finished);
	RUN_TEST(test_flwgpn_refuses_a_rule_nothing_satisfies);
	RUN_TEST(test_flwgpn_survives_being_handed_nothing);
}
