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
	CHECK(hintSafeMoveFLWG(NULL, 2, data) == NULL);
	CHECK(hintSafeMoveFLWG(NULL, 2, NULL) == NULL);
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

/* ------------------------------------------ a board nobody asked for exactly */

/* The case the app actually met, and what the engine now does about it.
 *
 * Each of these asks for a board on bounds no word in the dictionary satisfies.
 * That used to come back undealt - wordId -1, isStartValid 0 - and the app threw
 * it away and asked again from the same table, which is a loop rather than a
 * retry. The bounds are preferences now, so the search gives them up a little
 * at a time and deals the nearest board that does exist.
 *
 * So each of these asserts a real board came back, and then does to it exactly
 * what the screen does: asks it everything, straight away. Both halves matter.
 * A board that is dealt but answers nothing is no better than one that was
 * never dealt. */

void test_the_pathfinder_deals_a_board_on_bounds_nothing_satisfies(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* game = initiateFLWP(IMPOSSIBLE_MIN, IMPOSSIBLE_MAX,
		1, 2, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, data);

	CHECK_INT(isStartValid_FLWP(game), 1);
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

void test_the_adversarial_game_deals_a_board_on_bounds_nothing_satisfies(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWG(data, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX);

	CHECK_INT(isStartValidFLWG(game), 1);
	getCurrWord(game, data);
	hintLetterToConsiderFLWG(game, data);
	hintNumOptionsFLWG(game, data);
	botTakesTurn(game, data, 0);

	freeGameComponentsFLWG(game);
	freeDataStructures(data);
}

void test_the_turns_game_deals_a_board_on_bounds_nothing_satisfies(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* game = initFLWT(3, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, data);

	CHECK_INT(isStartValidFLWT(game), 1);
	getStartWordFLWT(game, data);
	getCurrentWordsFLWT(game);
	isGameWonFLWT(game);
	hint_letterToConsiderFLWT(game, data);
	hint_numOptionsFLWT(game, data);

	freeGameComponentsFLWT(game);
	freeDataStructures(data);
}

void test_the_constraint_game_deals_a_board_on_bounds_nothing_satisfies(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC* game = initFLWC(IMPOSSIBLE_MIN, IMPOSSIBLE_MAX,
		GOALS, AVOIDS, 1, 0, 2, 0, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, 3, data);

	CHECK_INT(isStartValidFLWC(game), 1);
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

void test_the_composed_pathfinder_deals_a_board_on_bounds_nothing_satisfies(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWGP* game = initiateFLWGP(IMPOSSIBLE_MIN, IMPOSSIBLE_MAX,
		GOALS, AVOIDS, 1, 0, 2, 0, IMPOSSIBLE_MIN, IMPOSSIBLE_MAX, data);

	CHECK_INT(isStartValid_FLWGP(game), 1);
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

/* ---------------------------------------------- whatever it is asked for --- */

/* The claim, swept rather than sampled.
 *
 * The app draws a board's numbers from tables and hands them over without
 * anybody having checked the dictionary has a word to match. Whether it does
 * is not something the app can know - only the dictionary knows which boards
 * exist - so the engine's job is to answer with a board whatever it is asked
 * for, and the point of this is that there is no request that gets a hole back.
 *
 * Every combination below describes a board that does not exist: adjacencies
 * past the densest word in the dictionary, distances past its diameter, bands
 * inverted so that no number at all falls inside them. Each one still has to
 * come back playable.
 */

static const int NOWHERE_NEAR[][2] = {
	{ 900, 999 },   /* far more neighbours than any word has */
	{ 25, 40 },     /* just past the densest word in a four letter dictionary */
	{ 30, 10 },     /* inverted, so nothing is inside it */
	{ -5, -1 },     /* below the floor */
	{ 24, 24 },     /* a band of one, at the very top */
};
#define NOWHERE_NEAR_COUNT ((int)(sizeof(NOWHERE_NEAR) / sizeof(NOWHERE_NEAR[0])))

static void test_the_adversarial_game_always_deals_something(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	for(int i = 0; i < NOWHERE_NEAR_COUNT; i++){
		struct GameData* game = initFLWG(data, NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1]);
		CHECK_INT(isStartValidFLWG(game), 1);
		CHECK(getCurrWord(game, data) != NULL);
		freeGameComponentsFLWG(game);
	}

	freeDataStructures(data);
}

static void test_the_turns_game_always_deals_something(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	for(int i = 0; i < NOWHERE_NEAR_COUNT; i++){
		struct GameComponentsFLWT* game = initFLWT(3, NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1], data);
		CHECK_INT(isStartValidFLWT(game), 1);
		CHECK(getStartWordFLWT(game, data) != NULL);
		freeGameComponentsFLWT(game);
	}

	freeDataStructures(data);
}

static void test_the_pathfinder_always_deals_something(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	for(int i = 0; i < NOWHERE_NEAR_COUNT; i++){
		/* The route length is asked for out of reach as well, so both the word
		   and the distance between the two words have to be given up. */
		struct GameComponents* game = initiateFLWP(NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1],
			40, 60, NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1], data);

		CHECK_INT(isStartValid_FLWP(game), 1);
		CHECK(getStartWordFLWP(game, data) != NULL);
		CHECK(getGoalWordFLWP(game, data) != NULL);
		/* And the two are different words with a route between them, which is
		   the one thing a pathfinder cannot do without. Read through the same
		   calls the screen uses, since the components are opaque here. */
		CHECK(strcmp(getStartWordFLWP(game, data), getGoalWordFLWP(game, data)) != 0);
		CHECK(distanceToGoalFLWP(game, data) > 0);

		freeGameComponentsFLWP(game, data);
	}

	freeDataStructures(data);
}

static void test_the_constraint_game_always_deals_something(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	for(int i = 0; i < NOWHERE_NEAR_COUNT; i++){
		struct GameComponentsFLWC* game = initFLWC(NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1],
			GOALS, AVOIDS, 40, 0, 60, 0, NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1], 3, data);

		CHECK_INT(isStartValidFLWC(game), 1);
		CHECK(getStartWordFLWC(game, data) != NULL);
		/* Not already over: what is never given up is that the board opens on a
		   word which is neither a goal nor forbidden. */
		CHECK_INT(isGameWonFLWC(game), -1);
		CHECK_INT(checkIfUsed_WordSet(game->wordId, game->goalWords), 0);
		CHECK_INT(checkIfUsed_WordSet(game->wordId, game->avoidWords), 0);

		freeGameComponentsFLWC(game);
	}

	freeDataStructures(data);
}

/* Keep away, which is the mode that went wrong: no goal words at all, and a
   rule that forbids a share of the board. */
static void test_the_keep_away_game_always_deals_something(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	static char* forbidden[] = { "care", "ware", "bare", "hare", "mare", NULL };
	static char* nothing[] = { NULL };

	for(int i = 0; i < NOWHERE_NEAR_COUNT; i++){
		struct GameComponentsFLWC* game = initFLWC(NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1],
			nothing, forbidden, 0, 1, 0, 6, NOWHERE_NEAR[i][0], NOWHERE_NEAR[i][1], 3, data);

		CHECK_INT(isStartValidFLWC(game), 1);
		CHECK(getStartWordFLWC(game, data) != NULL);
		CHECK_INT(checkIfUsed_WordSet(game->wordId, game->avoidWords), 0);

		freeGameComponentsFLWC(game);
	}

	freeDataStructures(data);
}

/* And that giving up is the last resort rather than the first: a board that can
   be dealt exactly as described still is. Otherwise the tables the app tunes
   its difficulty with would stop meaning anything. */
static void test_a_board_that_exists_is_dealt_as_asked(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	for(int attempt = 0; attempt < 25; attempt++){
		struct GameData* game = initFLWG(data, 6, 9);
		CHECK_INT(isStartValidFLWG(game), 1);
		CHECK(getNumAdjacencies(game->currWordId, data) >= 6);
		CHECK(getNumAdjacencies(game->currWordId, data) <= 9);
		freeGameComponentsFLWG(game);
	}

	for(int attempt = 0; attempt < 25; attempt++){
		struct GameComponentsFLWT* game = initFLWT(3, 10, 14, data);
		CHECK_INT(isStartValidFLWT(game), 1);
		CHECK(getNumAdjacencies(game->startWordId, data) >= 10);
		CHECK(getNumAdjacencies(game->startWordId, data) <= 14);
		freeGameComponentsFLWT(game);
	}

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
	RUN_TEST(test_the_pathfinder_deals_a_board_on_bounds_nothing_satisfies);
	RUN_TEST(test_the_adversarial_game_deals_a_board_on_bounds_nothing_satisfies);
	RUN_TEST(test_the_turns_game_deals_a_board_on_bounds_nothing_satisfies);
	RUN_TEST(test_the_constraint_game_deals_a_board_on_bounds_nothing_satisfies);
	RUN_TEST(test_the_composed_pathfinder_deals_a_board_on_bounds_nothing_satisfies);
	RUN_TEST(test_the_adversarial_game_always_deals_something);
	RUN_TEST(test_the_turns_game_always_deals_something);
	RUN_TEST(test_the_pathfinder_always_deals_something);
	RUN_TEST(test_the_constraint_game_always_deals_something);
	RUN_TEST(test_the_keep_away_game_always_deals_something);
	RUN_TEST(test_a_board_that_exists_is_dealt_as_asked);
	RUN_TEST(test_a_whole_pathfinder_game_start_to_finish);
	RUN_TEST(test_a_whole_turns_game_start_to_finish);
	RUN_TEST(test_a_whole_adversarial_game_start_to_finish);
}
