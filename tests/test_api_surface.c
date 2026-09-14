/*
Title: test_api_surface.c
Description: Every public entry point, called the two ways that crash it.

The app reaches the engine through four headers and about seventy functions,
and every one of them takes a pointer to a game. Two things routinely arrive
holding nothing:

  - a game that was never built. A board whose adjacency and distance bounds no
    word satisfies produces no components at all, and the screen that asked for
    it is already asking it questions by the time anyone knows;
  - a game that has been handed back. Every release sets its pointer to zero,
    and calls already in flight land afterwards.

Both are a null pointer arriving where a game was expected. Dereferencing it is
not an error the app can report - it is SIGSEGV, the process gone, no message,
which is exactly how the first of these was found: twice in one afternoon, in
two different getters, on a board that could not be dealt.

So this calls all of them with nothing, and then deals an impossible board of
each kind and calls all of them again on whatever that produced. Nothing here
asserts a useful answer; the assertion is that the process is still running.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWT-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/api/includes/Debug-API.h"
#include "../src/structs/includes/ArrayList.h"

struct DataStructures* open_dictionary(const char* path, int numLetters);

/* Bounds no word can satisfy: more neighbours than the densest word in a four
   letter dictionary has, which is how a board comes back empty. */
#define IMPOSSIBLE_MIN 900
#define IMPOSSIBLE_MAX 999

static char* GOALS[] = { "aaaa", NULL };
static char* AVOIDS[] = { NULL };

/* ------------------------------------------------------------------ nothing */

static void every_flwg_call_with_nothing(struct DataStructures* data){
	CHECK_INT(isStartValidFLWG(NULL), 0);
	CHECK(getCurrWord(NULL, data) == NULL);
	CHECK(getCurrWord(NULL, NULL) == NULL);
	CHECK_INT(botTakesTurn(NULL, data, 0), -1);
	CHECK(userTakesTurn("cat", NULL, data) != 999);
	CHECK(hintLetterToConsiderFLWG(NULL, data) != 999);
	CHECK(hintNumOptionsFLWG(NULL, data) <= 0);
	/* endGame is declared in FLWG-API.h and defined nowhere, so it cannot be
	   called - see the note in the commit. */
	freeGameComponentsFLWG(NULL);
}

static void every_flwp_call_with_nothing(struct DataStructures* data){
	CHECK_INT(isStartValid_FLWP(NULL), 0);
	CHECK(getStartWordFLWP(NULL, data) == NULL);
	CHECK(getGoalWordFLWP(NULL, data) == NULL);
	CHECK(getPrevWordFLWP(NULL) <= 0);
	CHECK_INT(isGameWonFLWP(NULL), 0);
	CHECK(getCurrentWordsFLWP(NULL) == NULL);
	CHECK(userEntersWord_FLWP("cat", NULL, data) != 999);
	removeWord_FLWP("cat", NULL, data);
	ResetFLWP(NULL, data);
	undoMoveFLWP(NULL, data);
	redoMoveFLWP(NULL, data);
	CHECK(hintGetHeadAdjacencyFLWP(NULL, data) == NULL);
	CHECK(hintGetTailAdjacencyFLWP(NULL, data) == NULL);
	CHECK_INT(hintGetMinAdjacenciesFLWP(NULL, data), -1);
	CHECK_INT(hintGetMinAdjacenciesFLWP(NULL, NULL), -1);
	CHECK_INT(distanceToGoalFLWP(NULL, data), -1);
	CHECK_INT(distanceToGoalFLWP(NULL, NULL), -1);
	freeGameComponentsFLWP(NULL, data);
}

static void every_flwgp_call_with_nothing(struct DataStructures* data){
	CHECK_INT(isStartValid_FLWGP(NULL), 0);
	CHECK(getFLWPComponentsFLWGP(NULL) == NULL);
	CHECK(getFLWCComponentsFLWGP(NULL) == NULL);
	CHECK(userEntersWord_FLWGP("cat", NULL, data) != 999);
	undoMoveFLWGP(NULL, data);
	redoMoveFLWGP(NULL, data);
	CHECK(hintGetMinAdjacenciesFLWGP(NULL) <= 0);
	CHECK(hintWordTowardsGoalFLWGP(NULL, data) == NULL);
	CHECK(hintGetValidGoalWordFLWGP(NULL, data) == NULL);
	freeGameComponentsFLWGP(NULL, data);
}

static void every_flwt_call_with_nothing(struct DataStructures* data){
	CHECK_INT(isStartValidFLWT(NULL), 0);
	CHECK(getStartWordFLWT(NULL, data) == NULL);
	CHECK(userEntersWordFLWT("cat", NULL, data) != 999);
	CHECK(getCurrentWordsFLWT(NULL) == NULL);
	CHECK_INT(isGameWonFLWT(NULL), 0);
	CHECK(hint_letterToConsiderFLWT(NULL, data) != 999);
	CHECK(hint_numOptionsFLWT(NULL, data) <= 0);
	freeGameComponentsFLWT(NULL);
}

static void every_flwc_call_with_nothing(struct DataStructures* data){
	CHECK_INT(isStartValidFLWC(NULL), 0);
	CHECK(getStartWordFLWC(NULL, data) == NULL);
	CHECK(userEntersWordFLWC("cat", NULL, data) != 999);
	CHECK(botTakesTurnFLWC(0, NULL, data) != 999);
	CHECK_INT(isGameWonFLWC(NULL), 0);
	CHECK(hintGoalWordFLWC(NULL, data) == NULL);
	CHECK(hintAdjacencyTowardsGoalFLWC(NULL, data) == NULL);
	CHECK(hintPathToGoalFLWC(NULL, data) == NULL);
	CHECK(hintMinDistanceToGoalFLWC(NULL, data) != 999);
	CHECK(hintBestDirectAdjacencyFLWIC(NULL, data) == NULL);
	CHECK(hintDistanceFromNearestAvoidWordFLWIC(NULL, data) != 999);
	freeGameComponentsFLWC(NULL);
}

void test_every_call_survives_a_game_that_is_not_there(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	every_flwg_call_with_nothing(data);
	every_flwp_call_with_nothing(data);
	every_flwgp_call_with_nothing(data);
	every_flwt_call_with_nothing(data);
	every_flwc_call_with_nothing(data);

	freeDataStructures(data);
}

/* --------------------------------------------------- a board nobody can deal */

/* The case the app actually met. Each of these asks for a board on bounds no
   word satisfies, and then does to it exactly what the screen does: asks it
   everything, before anything has established that there is nothing there. */

void test_the_pathfinder_survives_a_board_that_cannot_be_dealt(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* game = initiateFLWP(IMPOSSIBLE_MIN, IMPOSSIBLE_MAX,
		1, 2, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, data);

	CHECK_INT(isStartValid_FLWP(game), 0);
	/* Every one of these is asked by the screen as it opens */
	getStartWordFLWP(game, data);
	getGoalWordFLWP(game, data);
	hintGetMinAdjacenciesFLWP(game, data);
	distanceToGoalFLWP(game, data);
	hintGetHeadAdjacencyFLWP(game, data);
	hintGetTailAdjacencyFLWP(game, data);
	getCurrentWordsFLWP(game);
	isGameWonFLWP(game);
	undoMoveFLWP(game, data);
	redoMoveFLWP(game, data);

	freeGameComponentsFLWP(game, data);
	freeDataStructures(data);
}

void test_the_adversarial_game_survives_a_board_that_cannot_be_dealt(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWG(data, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX);

	CHECK_INT(isStartValidFLWG(game), 0);
	getCurrWord(game, data);
	hintLetterToConsiderFLWG(game, data);
	hintNumOptionsFLWG(game, data);
	botTakesTurn(game, data, 0);

	freeGameComponentsFLWG(game);
	freeDataStructures(data);
}

void test_the_turns_game_survives_a_board_that_cannot_be_dealt(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* game = initFLWT(3, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, data);

	CHECK_INT(isStartValidFLWT(game), 0);
	getStartWordFLWT(game, data);
	getCurrentWordsFLWT(game);
	isGameWonFLWT(game);
	hint_letterToConsiderFLWT(game, data);
	hint_numOptionsFLWT(game, data);

	freeGameComponentsFLWT(game);
	freeDataStructures(data);
}

void test_the_constraint_game_survives_a_board_that_cannot_be_dealt(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC* game = initFLWC(IMPOSSIBLE_MIN, IMPOSSIBLE_MAX,
		GOALS, AVOIDS, 1, 0, 2, 0, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, 3, data);

	CHECK_INT(isStartValidFLWC(game), 0);
	getStartWordFLWC(game, data);
	isGameWonFLWC(game);
	hintGoalWordFLWC(game, data);
	hintAdjacencyTowardsGoalFLWC(game, data);
	hintPathToGoalFLWC(game, data);
	hintMinDistanceToGoalFLWC(game, data);
	hintBestDirectAdjacencyFLWIC(game, data);
	hintDistanceFromNearestAvoidWordFLWIC(game, data);
	botTakesTurnFLWC(0, game, data);

	freeGameComponentsFLWC(game);
	freeDataStructures(data);
}

void test_the_composed_pathfinder_survives_a_board_that_cannot_be_dealt(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWGP* game = initiateFLWGP(IMPOSSIBLE_MIN, IMPOSSIBLE_MAX,
		GOALS, AVOIDS, 1, 0, 2, 0, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, data);

	CHECK_INT(isStartValid_FLWGP(game), 0);
	getFLWPComponentsFLWGP(game);
	getFLWCComponentsFLWGP(game);
	hintGetMinAdjacenciesFLWGP(game);
	hintWordTowardsGoalFLWGP(game, data);
	hintGetValidGoalWordFLWGP(game, data);
	undoMoveFLWGP(game, data);
	redoMoveFLWGP(game, data);

	freeGameComponentsFLWGP(game, data);
	freeDataStructures(data);
}

/* ------------------------------------------------------- the ordinary course */

/* A board that can be dealt, played the way the app plays one: opened, asked
   its questions, moved through, wound back, and handed over. Valgrind is the
   other half of this test - the suite runs under it, so a leak or a read of
   freed memory anywhere along here fails the build. */

void test_a_whole_pathfinder_game_start_to_finish(void){
	/* Nothing here is freed. Ownership is inconsistent across the hint and
	   getter family - some return a fresh string, some a pointer into the
	   dictionary - and freeing one of the latter is a double free. Valgrind
	   runs this suite, so anything genuinely leaked is caught there. */
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* game = initiateFLWP(1, 30, 2, 6, 1, 30, data);

	CHECK(game != NULL);
	if(game != NULL && isStartValid_FLWP(game)){
		char* start = getStartWordFLWP(game, data);
		char* goal = getGoalWordFLWP(game, data);
		CHECK(start != NULL);
		CHECK(goal != NULL);

		CHECK(hintGetMinAdjacenciesFLWP(game, data) >= 0);
		CHECK(distanceToGoalFLWP(game, data) >= 0);
		hintGetHeadAdjacencyFLWP(game, data);
		hintGetTailAdjacencyFLWP(game, data);

		/* A move, wound back, and put on again */
		char* next = hintGetHeadAdjacencyFLWP(game, data);
		if(next != NULL){
			userEntersWord_FLWP(next, game, data);
			CHECK(getCurrentWordsFLWP(game) != NULL);
			undoMoveFLWP(game, data);
			redoMoveFLWP(game, data);
		}
		ResetFLWP(game, data);
		isGameWonFLWP(game);

	}

	freeGameComponentsFLWP(game, data);
	freeDataStructures(data);
}

void test_a_whole_turns_game_start_to_finish(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* game = initFLWT(3, 1, 30, data);

	CHECK(game != NULL);
	if(game != NULL && isStartValidFLWT(game)){
		char* start = getStartWordFLWT(game, data);
		CHECK(start != NULL);
		CHECK(getCurrentWordsFLWT(game) != NULL);
		hint_letterToConsiderFLWT(game, data);
		CHECK(hint_numOptionsFLWT(game, data) >= 0);
		isGameWonFLWT(game);
	}

	freeGameComponentsFLWT(game);
	freeDataStructures(data);
}

void test_a_whole_adversarial_game_start_to_finish(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWG(data, 1, 30);

	CHECK(game != NULL);
	if(game != NULL && isStartValidFLWG(game)){
		char* word = getCurrWord(game, data);
		CHECK(word != NULL);
		hintLetterToConsiderFLWG(game, data);
		hintNumOptionsFLWG(game, data);
		botTakesTurn(game, data, 0);
	}

	freeGameComponentsFLWG(game);
	freeDataStructures(data);
}

void suite_api_surface(void){
	printf("\n-- every entry point, holding nothing --\n");
	RUN_TEST(test_every_call_survives_a_game_that_is_not_there);
	RUN_TEST(test_the_pathfinder_survives_a_board_that_cannot_be_dealt);
	RUN_TEST(test_the_adversarial_game_survives_a_board_that_cannot_be_dealt);
	RUN_TEST(test_the_turns_game_survives_a_board_that_cannot_be_dealt);
	RUN_TEST(test_the_constraint_game_survives_a_board_that_cannot_be_dealt);
	RUN_TEST(test_the_composed_pathfinder_survives_a_board_that_cannot_be_dealt);
	RUN_TEST(test_a_whole_pathfinder_game_start_to_finish);
	RUN_TEST(test_a_whole_turns_game_start_to_finish);
	RUN_TEST(test_a_whole_adversarial_game_start_to_finish);
}
