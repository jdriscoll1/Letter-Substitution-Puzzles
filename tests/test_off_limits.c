/*
Title: test_off_limits.c
Description: The words the engine will not say.

A word being valid to PLAY and a word being one the game deals, aims at, plays
back or hints towards are two different questions - the same split the obscurity
ranks make, for a different reason. The ranks keep out words nobody knows. This
keeps out a handful everybody does, and the cap cannot help: FUCK is the 299th
commonest word in English, so it sails under every cap the game has.

THE PLAYER MAY STILL PLAY EVERY ONE, and that half is tested first, because it
is the half that would be an injustice to get wrong.

AND IT IS NEVER RELAXED. The cap is a preference - dropped when the dictionary
has nothing matching a board's numbers, so that a board can always be dealt.
This is not: a board that could only be built on one of these does not get
built. That is the difference the give-up round test below is about.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWT-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/api/includes/Debug-API.h"
#include "../src/flwp/includes/PathGameComponents.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"

struct DataStructures* open_dictionary(const char* path, int numLetters);

#define LIST "docs/offLimits.txt"

/*A word the list holds and the dictionary has, for the checks below.*/
#define A_BLOCKED_WORD "shit"

static struct DataStructures* loaded(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	Load_Obscurity(data->I2W, "docs/4ranks.txt");
	Load_OffLimits(data->I2W, LIST, data);
	return data;
}

/*How many of the four letter words the list takes out*/
static int howManyBlocked(struct DataStructures* data){
	int i, blocked = 0;
	for(i = 0; i < data->I2W->numWords; i++){
		if(isOffLimits(i, data)){
			blocked++;
		}
	}
	return blocked;
}

void test_off_limits_marks_the_words_on_the_list(void){
	struct DataStructures* data = loaded();

	int id = convertWordToInt(A_BLOCKED_WORD, data);
	CHECK(id >= 0);
	CHECK_INT(isOffLimits(id, data), 1);

	/*And an ordinary word beside it is untouched.*/
	CHECK_INT(isOffLimits(convertWordToInt("care", data), data), 0);
	CHECK_INT(isOffLimits(convertWordToInt("boat", data), data), 0);

	/*The list bites, and does not bite everything - a list that marked the
	whole dictionary would pass every other check in this file.*/
	int blocked = howManyBlocked(data);
	CHECK(blocked >= 20);
	CHECK(blocked < 100);

	freeDataStructures(data);
}

/* THE HALF THAT MATTERS MOST: it is not an obscurity, and it must not be read
   as one. The praise line pays a player for finding a rare word, and these are
   not rare - writing them into the ranks to keep them out would have had the
   game compliment somebody for playing one. */
void test_off_limits_is_not_an_obscurity(void){
	struct DataStructures* data = loaded();

	int id = convertWordToInt(A_BLOCKED_WORD, data);
	CHECK(id >= 0);

	/*Still as common as it ever was, which is very.*/
	CHECK(getObscurity(id, data) < 1000);
	CHECK_INT(isTooObscure(id, data), 0);

	/*So the two questions genuinely differ on it.*/
	CHECK(isOffLimits(id, data) != isTooObscure(id, data));

	freeDataStructures(data);
}

/*And the player may play one. The word list the engine is judged against is
never the one the player is judged against.*/
void test_a_player_may_still_play_one(void){
	struct DataStructures* data = loaded();
	int id = convertWordToInt(A_BLOCKED_WORD, data);
	CHECK(id >= 0);

	/*A walk seated one move from it, so the only question is whether it is
	taken.*/
	struct intList* c = getConnections(id, data->I2W);
	CHECK(c != NULL && c->next != NULL);

	if(c != NULL && c->next != NULL){
		int from = c->next->data;
		struct GameComponents* walk = initFLWPAtStart(convertIntToWord(from, data),
			1, 4, 1, 30, data);
		CHECK(walk != NULL);

		if(walk != NULL){
			CHECK_INT(userEntersWord_FLWP(A_BLOCKED_WORD, walk, data), VALID);
			freeGameComponentsFLWP(walk, data);
		}
	}

	freeDataStructures(data);
}

/*Nothing the ENGINE chooses is ever one of them. Every dealer, over enough
boards that a one in eighty word would have turned up.*/
void test_nothing_the_engine_deals_is_off_limits(void){
	struct DataStructures* data = loaded();
	int round, dealt = 0, said = 0;
	seedGameRandom(5);

	for(round = 0; round < 60; round++){
		struct GameData* flwg = initFLWG(data, 8, 30);
		if(flwg != NULL){
			char* word = getCurrWord(flwg, data);
			if(word != NULL){
				dealt++;
				if(isOffLimits(convertWordToInt(word, data), data)){ said++; }
			}
			freeGameComponentsFLWG(flwg);
		}

		struct GameComponents* walk = initiateFLWP(8, 30, 2, 4, 1, 30, data);
		if(walk != NULL){
			if(walk->start != -1){
				dealt++;
				if(isOffLimits(walk->start, data)){ said++; }
			}
			if(walk->goal != -1){
				dealt++;
				if(isOffLimits(walk->goal, data)){ said++; }
			}
			freeGameComponentsFLWP(walk, data);
		}

		struct GameComponentsFLWT* flwt = initFLWT(3, 8, 30, data);
		if(flwt != NULL){
			char* word = getStartWordFLWT(flwt, data);
			if(word != NULL){
				dealt++;
				if(isOffLimits(convertWordToInt(word, data), data)){ said++; }
			}
			freeGameComponentsFLWT(flwt);
		}
	}

	/*Enough boards that the check means something.*/
	CHECK(dealt > 150);
	CHECK_INT(said, 0);

	freeDataStructures(data);
}

/*And the bot never answers with one, over a whole game played out.*/
void test_the_bot_never_says_one(void){
	struct DataStructures* data = loaded();
	int round, moves = 0, said = 0;
	seedGameRandom(11);

	for(round = 0; round < 20; round++){
		struct GameData* game = initFLWG(data, 8, 30);
		if(game == NULL){
			continue;
		}

		int turn;
		for(turn = 0; turn < 8; turn++){
			int played = botTakesTurn(game, data, 1);
			if(played == -1){
				break;
			}
			moves++;
			if(isOffLimits(played, data)){
				said++;
			}
		}
		freeGameComponentsFLWG(game);
	}

	CHECK(moves > 40);
	CHECK_INT(said, 0);

	freeDataStructures(data);
}

/*A list nobody loaded leaves the engine exactly as it was, and a path that is
not there is not a crash.*/
void test_off_limits_survives_having_no_list(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	Load_OffLimits(data->I2W, "docs/there-is-no-such-file.txt", data);
	Load_OffLimits(data->I2W, NULL, data);
	Load_OffLimits(NULL, LIST, data);
	Load_OffLimits(data->I2W, LIST, NULL);

	CHECK_INT(isOffLimits(convertWordToInt(A_BLOCKED_WORD, data), data), 0);
	CHECK_INT(isOffLimits(-1, data), 0);
	CHECK_INT(isOffLimits(999999, data), 0);
	CHECK_INT(isOffLimits(0, NULL), 0);

	freeDataStructures(data);
}

void suite_off_limits(void){
	printf("\n-- the words the engine will not say --\n");
	RUN_TEST(test_off_limits_marks_the_words_on_the_list);
	RUN_TEST(test_off_limits_is_not_an_obscurity);
	RUN_TEST(test_a_player_may_still_play_one);
	RUN_TEST(test_nothing_the_engine_deals_is_off_limits);
	RUN_TEST(test_the_bot_never_says_one);
	RUN_TEST(test_off_limits_survives_having_no_list);
}
