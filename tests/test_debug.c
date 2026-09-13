/*
Title: test_debug.c
Description: The debug entry points (src/api/src/Debug-API.c), which start a game
on a word you name instead of one the mode picked for itself. What these hold
down is that naming a word changes nothing except the first word: the components
are the ordinary ones, the game plays, and the usual freeGameComponents* frees
it. And that an unknown word is reported through isStartValid*, not crashed on.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/api/includes/FLWT-API.h"
#include "../src/api/includes/Debug-API.h"

#include "../src/flwp/includes/PathGameComponents.h"
#include "../src/flwp/includes/GameFunctions.h"
#include "../src/flwp/includes/UserInput.h"

/*"bane", "cane" and "band" each differ from one another in a single letter, so
all three sit next to each other in docs/4.txt -- enough to start a game and
take a turn in it*/
static char* GOALS[] = {"band", NULL};
static char* NOTHING[] = {NULL};

/*The right length, and not a word the dictionary has ever heard of*/
static char* NOT_A_WORD = "zzzz";

/* ----------------------------------------------------------------- FLWG --- */

static void test_flwg_starts_on_the_word_it_was_given(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWGAtStart("bane", data);

	CHECK_INT(isStartValidFLWG(game), 1);
	CHECK_STR(getCurrWord(game, data), "bane");

	/*And it is the word in play, not merely a word that was recorded: it has
	been marked used, so nobody can play it again*/
	CHECK_INT(userTakesTurn("bane", game, data), WORD_USED);

	freeGameComponentsFLWG(game);
	freeDataStructures(data);
}

static void test_a_named_start_plays_like_any_other(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWGAtStart("bane", data);
	int botMove;

	CHECK_INT(userTakesTurn("cane", game, data), VALID);
	CHECK_STR(getCurrWord(game, data), "cane");

	/*The bot answers from the named position the way it would from any other*/
	botMove = botTakesTurn(game, data, 0);
	CHECK(botMove != -1);
	CHECK_INT(letters_that_differ(convertIntToWord(botMove, data), "cane", 4), 1);

	freeGameComponentsFLWG(game);
	freeDataStructures(data);
}

/* ----------------------------------------------------------------- FLWP --- */

static void test_flwp_starts_on_the_word_it_was_given(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPAtStart("bane", 2, 8, 1, 30, data);

	CHECK_INT(isStartValid_FLWP(gc), 1);
	CHECK_STR(getStartWordFLWP(gc, data), "bane");

	/*The goal was searched for as usual, so there is a path, and it sets off
	from the word that was asked for*/
	CHECK_NOT_NULL(gc->solution);
	CHECK_INT(gc->solution->next->data, gc->start);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

static void test_flwp_joins_two_named_words(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPBetween("bane", "band", data);
	struct intList* step;
	int last = -1;

	CHECK_INT(isStartValid_FLWP(gc), 1);
	CHECK_STR(getStartWordFLWP(gc, data), "bane");
	CHECK_STR(getGoalWordFLWP(gc, data), "band");

	/*The path runs from the one to the other*/
	CHECK_NOT_NULL(gc->solution);
	CHECK_INT(gc->solution->next->data, gc->start);
	for(step = gc->solution->next; step != NULL; step = step->next){
		last = step->data;
	}
	CHECK_INT(last, gc->goal);

	/*and walking it is a legal game that ends won*/
	for(step = gc->solution->next->next; step != NULL; step = step->next){
		CHECK_INT(userEntersWord_FLWP(convertIntToWord(step->data, data), gc, data), VALID);
	}
	CHECK_INT(isGameWonFLWP(gc), 1);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

static void test_flwp_refuses_a_path_from_a_word_to_itself(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPBetween("bane", "bane", data);

	/*There is no game in standing still, and it says so rather than handing
	back a path of length one*/
	CHECK_INT(isStartValid_FLWP(gc), 0);
	CHECK_NULL(gc->solution);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/* ----------------------------------------------------------------- FLWC --- */

static void test_flwc_starts_on_the_word_it_was_given(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC* flwc = initFLWCAtStart("bane", GOALS, NOTHING, data);

	CHECK_INT(isStartValidFLWC(flwc), 1);
	CHECK_STR(getStartWordFLWC(flwc, data), "bane");

	/*The goal list it was handed is the one it plays to: "band" is one
	substitution away, so playing it wins*/
	CHECK_INT(isGameWonFLWC(flwc), -1);
	CHECK_INT(userEntersWordFLWC("band", flwc, data), VALID);
	CHECK_INT(isGameWonFLWC(flwc), 1);

	freeGameComponentsFLWC(flwc);
	freeDataStructures(data);
}

/* ----------------------------------------------------------------- FLWT --- */

static void test_flwt_starts_on_the_word_it_was_given(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* flwt = initFLWTAtStart("bane", 2, data);

	CHECK_INT(isStartValidFLWT(flwt), 1);
	CHECK_STR(getStartWordFLWT(flwt, data), "bane");

	/*The ceiling is the word's own adjacency count, so a named word can never
	be asked for more neighbours than it has*/
	CHECK_INT(flwt->maxAdjacenciesThatCanBeFound, getNumAdjacencies(flwt->startWordId, data));

	CHECK_INT(isGameWonFLWT(flwt), 0);
	CHECK_INT(userEntersWordFLWT("cane", flwt, data), VALID);
	CHECK_INT(userEntersWordFLWT("band", flwt, data), VALID);
	CHECK_INT(isGameWonFLWT(flwt), 1);

	freeGameComponentsFLWT(flwt);
	freeDataStructures(data);
}

/* --------------------------------------------------- a word that is not --- */

static void test_every_mode_reports_a_word_it_does_not_know(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	struct GameData* flwg = initFLWGAtStart(NOT_A_WORD, data);
	struct GameComponents* flwp = initFLWPAtStart(NOT_A_WORD, 2, 8, 1, 30, data);
	struct GameComponents* between = initFLWPBetween(NOT_A_WORD, "band", data);
	struct GameComponentsFLWC* flwc = initFLWCAtStart(NOT_A_WORD, GOALS, NOTHING, data);
	struct GameComponentsFLWT* flwt = initFLWTAtStart(NOT_A_WORD, 2, data);

	/*Each says so the way its own mode always says so, and each is still safe
	to hand back*/
	CHECK_INT(isStartValidFLWG(flwg), 0);
	CHECK_INT(isStartValid_FLWP(flwp), 0);
	CHECK_INT(isStartValid_FLWP(between), 0);
	CHECK_INT(isStartValidFLWC(flwc), 0);
	CHECK_INT(isStartValidFLWT(flwt), 0);

	freeGameComponentsFLWG(flwg);
	freeGameComponentsFLWP(flwp, data);
	freeGameComponentsFLWP(between, data);
	freeGameComponentsFLWC(flwc);
	freeGameComponentsFLWT(flwt);
	freeDataStructures(data);
}

void suite_debug(void){
	printf("\n-- debug starts --\n");
	RUN_TEST(test_flwg_starts_on_the_word_it_was_given);
	RUN_TEST(test_a_named_start_plays_like_any_other);
	RUN_TEST(test_flwp_starts_on_the_word_it_was_given);
	RUN_TEST(test_flwp_joins_two_named_words);
	RUN_TEST(test_flwp_refuses_a_path_from_a_word_to_itself);
	RUN_TEST(test_flwc_starts_on_the_word_it_was_given);
	RUN_TEST(test_flwt_starts_on_the_word_it_was_given);
	RUN_TEST(test_every_mode_reports_a_word_it_does_not_know);
}
