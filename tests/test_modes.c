/*
Title: test_modes.c
Description: Coverage for the game modes behind src/api that test_games.c does not
reach -- the FLWC challenge, the composed FLWGP pathfinder, the rest of the FLWG
bot types, and the ends of the FLWP and FLWT games.

The challenge modes pick their start word by searching (a BFS, and for FLWC a
whole game search), so the tests that call initFLWC and initiateFLWGP use the
parameter sets the main.c demos use, which are known to be satisfiable. The tests
that only care what happens *during* a game build the components by hand instead:
that keeps them quick and, more to the point, deterministic, since a searched
start word would move the moment anything about the search changed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/api/includes/FLWT-API.h"
#include "../src/api/includes/Debug-API.h"
#include "../src/flwc/includes/Challenges.h"
#include "../src/flwc/includes/FLWC.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/flwp/includes/PathGameComponents.h"
#include "../src/flwg/includes/FLWGGame.h"
#include "../src/structs/includes/ArrayList.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"
#include "../src/structs/includes/WordSet.h"

/*The goal set the FLWC demo uses -- every "-ck" word in docs/4.txt, which is big
enough that a start word satisfying the distance range actually exists*/
static char* DEMO_GOAL_WORDS[] = {"lack", "lock", "pack", "tack", "back", "suck",
	"hack", "hock", "pock", "rack", "sack", "sock", "buck", "dock", "duck",
	"lick", "luck", "muck", "peck", "sick", "mock", "pick", "rock", "tuck",
	"jack", "jock", "tick", "beck", "deck", "heck", "kick", "wick", "yuck",
	"neck", NULL};
static char* NO_WORDS[] = {NULL};

/*Returns the id of the first neighbour of id that nobody has played*/
static int first_unused_neighbour(int id, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	while(adjacency != NULL){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			return adjacency->data;
		}
		adjacency = adjacency->next;
	}
	return -1;
}

/*Is move a neighbour of id?*/
static int is_neighbour(int id, int move, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	while(adjacency != NULL){
		if(adjacency->data == move){
			return 1;
		}
		adjacency = adjacency->next;
	}
	return 0;
}

/*The n-th entry of an int linked list, counting the header as before the first*/
static int nth_entry(struct intList* list, int n){
	struct intList* curr = list->next;
	int i;
	for(i = 0; i < n && curr != NULL; i++){
		curr = curr->next;
	}
	return (curr == NULL) ? -1 : curr->data;
}

/* ----------------------------------------------------------------- FLWC --- */

static void test_flwc_start_word_satisfies_its_parameters(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC* flwc = initFLWC(1, 30, DEMO_GOAL_WORDS, NO_WORDS,
		2, 0, 6, 0, 1, 30, 8, data);
	int distance;

	CHECK_NOT_NULL(flwc);
	CHECK_INT(isStartValidFLWC(flwc), 1);
	CHECK(flwc->wordId != -1);

	/*The word the search settled on has to meet every constraint it was given*/
	CHECK(getNumAdjacencies(flwc->wordId, data) >= 1);
	CHECK(getNumAdjacencies(flwc->wordId, data) <= 30);
	distance = hintMinDistanceToGoalFLWC(flwc, data);
	CHECK(distance >= 2);
	CHECK(distance <= 6);

	/*The start is claimed, and it is not itself a goal, so nothing is won yet*/
	CHECK_INT(checkIfUsed_WordSet(flwc->wordId, data->wordSet) != 0, 1);
	CHECK_INT(isGameWonFLWC(flwc), -1);
	CHECK_STR(getStartWordFLWC(flwc, data), Convert_IntToWord(flwc->wordId, data->I2W));

	freeGameComponentsFLWC(flwc);
	freeDataStructures(data);
}

static void test_flwc_deals_a_board_when_the_distances_asked_for_do_not_exist(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	/*No word in a 4 letter dictionary is 90 substitutions from anything, so the
	board as described does not exist. The distances are a preference, so the
	search gives them up and deals the nearest board that does*/
	struct GameComponentsFLWC* flwc = initFLWC(1, 30, DEMO_GOAL_WORDS, NO_WORDS,
		90, 0, 99, 0, 1, 30, 8, data);

	CHECK_NOT_NULL(flwc);
	CHECK(flwc->wordId != -1);
	CHECK_INT(isStartValidFLWC(flwc), 1);
	/*And it is a game: not already won, and the start is claimed*/
	CHECK_INT(isGameWonFLWC(flwc), -1);
	CHECK_INT(checkIfUsed_WordSet(flwc->wordId, data->wordSet) != 0, 1);
	/*What is never given up: the word it opens on is not itself a goal*/
	CHECK_INT(checkIfUsed_WordSet(flwc->wordId, flwc->goalWords), 0);

	freeGameComponentsFLWC(flwc);
	freeDataStructures(data);
}
static void test_flwc_is_won_by_reaching_a_goal_word(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"care", NULL};
	struct GameComponentsFLWC flwc;

	flwc.wordId = Convert_WordToInt("ware", data);
	flwc.goalWords = convertCharPtrPtrToWordSet(goalWords, data);
	flwc.avoidWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
	markUsed_WordSet(flwc.wordId, data->wordSet);

	CHECK_INT(isGameWonFLWC(&flwc), -1);

	/*ware -> care is one substitution, and care is the goal*/
	CHECK_INT(userEntersWordFLWC("care", &flwc, data), VALID);
	CHECK_INT(flwc.wordId, Convert_WordToInt("care", data));
	CHECK_INT(checkIfUsed_WordSet(flwc.wordId, data->wordSet) != 0, 1);
	CHECK_INT(isGameWonFLWC(&flwc), 1);

	free_WordSet(flwc.goalWords);
	free_WordSet(flwc.avoidWords);
	freeDataStructures(data);
}

static void test_flwic_is_lost_by_reaching_an_avoid_word(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* avoidWords[] = {"care", NULL};
	struct GameComponentsFLWC flwc;

	flwc.wordId = Convert_WordToInt("ware", data);
	flwc.goalWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
	flwc.avoidWords = convertCharPtrPtrToWordSet(avoidWords, data);
	markUsed_WordSet(flwc.wordId, data->wordSet);

	/*Inverted challenge: the same move that wins above loses here*/
	CHECK_INT(userEntersWordFLWC("care", &flwc, data), VALID);
	CHECK_INT(isGameWonFLWC(&flwc), 2);
	/*and the avoid set is what the distance hint measures against*/
	CHECK_INT(hintDistanceFromNearestAvoidWordFLWIC(&flwc, data), 0);

	free_WordSet(flwc.goalWords);
	free_WordSet(flwc.avoidWords);
	freeDataStructures(data);
}

static void test_flwc_refuses_an_illegal_word_without_moving(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC flwc;
	int ware;

	ware = Convert_WordToInt("ware", data);
	flwc.wordId = ware;
	flwc.goalWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
	flwc.avoidWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
	markUsed_WordSet(ware, data->wordSet);

	/*Each rejection has to leave the current word exactly where it was*/
	CHECK_INT(userEntersWordFLWC("zzzz", &flwc, data), WORD_DOES_NOT_EXIST);
	CHECK_INT(flwc.wordId, ware);
	CHECK_INT(userEntersWordFLWC("war", &flwc, data), TOO_SHORT);
	CHECK_INT(flwc.wordId, ware);
	CHECK_INT(userEntersWordFLWC("wares", &flwc, data), TOO_LONG);
	CHECK_INT(flwc.wordId, ware);
	/*The word you are standing on has been claimed, so playing it again is
	refused as used rather than for sharing every letter -- Check_Input looks at
	the word set before it counts letters*/
	CHECK_INT(userEntersWordFLWC("ware", &flwc, data), WORD_USED);
	CHECK_INT(flwc.wordId, ware);

	free_WordSet(flwc.goalWords);
	free_WordSet(flwc.avoidWords);
	freeDataStructures(data);
}

static void test_flwc_bot_takes_a_legal_turn(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC flwc;
	int ware = Convert_WordToInt("ware", data);
	int played;

	flwc.wordId = ware;
	flwc.goalWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
	flwc.avoidWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
	markUsed_WordSet(ware, data->wordSet);

	/*-1 is the bot that keeps its own options open*/
	played = botTakesTurnFLWC(-1, &flwc, data);
	CHECK(played != -1);
	if(played > 0){
		CHECK_INT(is_neighbour(ware, played, data), 1);
		CHECK_INT(flwc.wordId, played);
		CHECK_INT(checkIfUsed_WordSet(played, data->wordSet) != 0, 1);
	}

	free_WordSet(flwc.goalWords);
	free_WordSet(flwc.avoidWords);
	freeDataStructures(data);
}

/*Every botTakesTurn* takes the same int: -2 mirror, -1 maximum adjacencies,
0 random, anything positive a search depth. FLWC only ever looked at -1 and the
positive case, so the other two fell through to an uninitialised result that was
then assigned straight into wordId*/
static void test_flwc_bot_handles_every_bot_type(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC flwc;
	int ware = Convert_WordToInt("ware", data);
	int botTypes[3];
	int i;

	botTypes[0] = -2;
	botTypes[1] = 0;
	botTypes[2] = 1;

	for(i = 0; i < 3; i++){
		int played;
		reset_WordSet(data->wordSet);
		flwc.wordId = ware;
		flwc.goalWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
		flwc.avoidWords = convertCharPtrPtrToWordSet(NO_WORDS, data);
		markUsed_WordSet(ware, data->wordSet);

		played = botTakesTurnFLWC(botTypes[i], &flwc, data);

		/*Whatever it does, it has to leave a real word behind -- either the one
		it started on or a neighbour, never a number it never set*/
		CHECK(played == -1 || played == -2 || (played >= 0 && played < data->I2W->numWords));
		CHECK(flwc.wordId == -1 || (flwc.wordId >= 0 && flwc.wordId < data->I2W->numWords));
		if(botTypes[i] == -2){
			/*The mirror stays put*/
			CHECK_INT(flwc.wordId, ware);
		}
		if(botTypes[i] == 0 && flwc.wordId != ware && flwc.wordId >= 0){
			CHECK_INT(is_neighbour(ware, flwc.wordId, data), 1);
		}

		free_WordSet(flwc.goalWords);
		free_WordSet(flwc.avoidWords);
	}

	freeDataStructures(data);
}

/*A start word is only accepted if the player can force a win from it, so what
the search calls a win decides which games get handed out. When somebody runs
out of moves the game scores it against whoever is stuck, which is what
botTakesTurnFLWC does: -1 when the bot has no move, which it calls a loss for
the bot, and -2 when the player is trapped, which it calls a win for the bot*/
static void test_flwc_winnability_treats_a_dead_end_as_a_loss(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"care", NULL};
	struct WordSet* goals = convertCharPtrPtrToWordSet(goalWords, data);
	struct WordSet* avoids = convertCharPtrPtrToWordSet(NO_WORDS, data);
	int ware = Convert_WordToInt("ware", data);
	struct intList* adjacency;

	/*ware -> care, with turns to spare, is a win*/
	CHECK_INT(is_game_winnable_FLWC(ware, 4, 1, goals, avoids, data, 0, 1), 1);

	/*Wall it in: every neighbour claimed, so whoever is on turn is stuck*/
	for(adjacency = getConnections(ware, data->I2W)->next; adjacency != NULL; adjacency = adjacency->next){
		markUsed_WordSet(adjacency->data, data->wordSet);
	}

	/*A player with nowhere to go has lost, so the word must not be handed out as
	a start. This used to come back as the sentinel -100, which every caller
	reads as a truthy int and so as a win*/
	CHECK_INT(is_game_winnable_FLWC(ware, 4, 1, goals, avoids, data, 0, 1), 0);
	/*The opponent running out is the other way round -- a win for the player.
	This used to come back as +100, truthy for the right reason by accident*/
	CHECK_INT(is_game_winnable_FLWC(ware, 4, 0, goals, avoids, data, 0, 1), 1);

	free_WordSet(goals);
	free_WordSet(avoids);
	freeDataStructures(data);
}

static void test_get_all_words_lists_the_dictionary(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char** allWords = getAllWords(data);
	int i;
	int mismatches = 0;

	CHECK_NOT_NULL(allWords);
	for(i = 0; i < data->I2W->numWords; i++){
		if(strcmp(allWords[i], Convert_IntToWord(i, data->I2W)) != 0){
			mismatches++;
		}
	}
	CHECK_INT(mismatches, 0);

	/*The array is the caller's, but the words in it still belong to I2W*/
	free(allWords);
	freeDataStructures(data);
}

/* ---------------------------------------------------------------- FLWGP --- */

static void test_flwgp_composes_a_path_and_a_challenge(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"ties", "pies", "lies", NULL};
	struct GameComponentsFLWGP* flwgp = initiateFLWGP(1, 30, goalWords, NO_WORDS,
		2, 0, 2, 0, 1, 30, data);
	struct GameComponents* flwp;
	struct GameComponentsFLWC* flwc;

	CHECK_NOT_NULL(flwgp);
	CHECK_INT(isStartValid_FLWGP(flwgp), 1);

	flwp = getFLWPComponentsFLWGP(flwgp);
	flwc = getFLWCComponentsFLWGP(flwgp);
	CHECK_NOT_NULL(flwp);
	CHECK_NOT_NULL(flwc);

	if(flwp != NULL && flwc != NULL){
		/*The two halves have to be looking at the same word to begin with*/
		CHECK_INT(flwp->start, flwc->wordId);
		CHECK_INT(getPrevWordFLWP(flwp), flwc->wordId);

		/*FLWGP has no single goal word -- the path runs to the nearest word in
		the goal set, so the solution ends inside that set rather than at a
		goal id the FLWP half knows about*/
		CHECK_INT(flwp->goal, -1);
		CHECK_NOT_NULL(flwp->solution);
		CHECK_INT(nth_entry(flwp->solution, 0), flwp->start);
		CHECK_INT(checkIfUsed_WordSet(FindLast_IntLL(flwp->solution), flwc->goalWords) != 0, 1);
		CHECK_INT(hintGetMinAdjacenciesFLWGP(flwgp), flwp->solution->size - 1);
		CHECK_STR(hintGetValidGoalWordFLWGP(flwgp, data),
			Convert_IntToWord(FindLast_IntLL(flwp->solution), data->I2W));
	}

	freeGameComponentsFLWGP(flwgp, data);
	freeDataStructures(data);
}

static void test_flwgp_undo_and_redo_keep_both_halves_in_step(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"ties", "pies", "lies", NULL};
	struct GameComponentsFLWGP* flwgp = initiateFLWGP(1, 30, goalWords, NO_WORDS,
		2, 0, 2, 0, 1, 30, data);
	struct GameComponents* flwp = getFLWPComponentsFLWGP(flwgp);
	struct GameComponentsFLWC* flwc = getFLWCComponentsFLWGP(flwgp);
	int start;
	int second;

	CHECK_INT(isStartValid_FLWGP(flwgp), 1);
	start = flwp->start;
	second = nth_entry(flwp->solution, 1);
	CHECK(second != -1);

	/*One step along the solution moves both halves together. This is the whole
	point of the composed mode: a move made through the path half has to be the
	move the challenge half thinks it is on, or the goal check reads the wrong
	word*/
	CHECK_INT(userEntersWord_FLWGP(Convert_IntToWord(second, data->I2W), flwgp, data), VALID);
	CHECK_INT(getPrevWordFLWP(flwp), second);
	CHECK_INT(flwc->wordId, second);

	undoMoveFLWGP(flwgp, data);
	CHECK_INT(getPrevWordFLWP(flwp), start);
	CHECK_INT(flwc->wordId, start);

	redoMoveFLWGP(flwgp, data);
	CHECK_INT(getPrevWordFLWP(flwp), second);
	CHECK_INT(flwc->wordId, second);

	/*A refused move must not drag either half out of step either*/
	CHECK_INT(userEntersWord_FLWGP("zzzz", flwgp, data), WORD_DOES_NOT_EXIST);
	CHECK_INT(getPrevWordFLWP(flwp), second);
	CHECK_INT(flwc->wordId, second);

	freeGameComponentsFLWGP(flwgp, data);
	freeDataStructures(data);
}

/* ----------------------------------------------------------------- FLWG --- */

static void test_flwg_bot_types_each_play_their_own_way(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* gameData = initFLWG(data, 4, 8);
	int start = gameData->currWordId;
	struct intList* adjacency;
	int bestOptions = -1;
	int played;

	CHECK_INT(isStartValidFLWG(gameData), 1);

	/*-2 mirrors: it hands the same word straight back*/
	CHECK_INT(botTakesTurn(gameData, data, -2), start);
	CHECK_INT(gameData->currWordId, start);

	/*-1 takes the neighbour with the most moves still open. Worked out before
	the call, since the bot claims the word it picks*/
	for(adjacency = getConnections(start, data->I2W)->next; adjacency != NULL; adjacency = adjacency->next){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			int options = getNumOptions(adjacency->data, data);
			if(options > bestOptions){
				bestOptions = options;
			}
		}
	}
	CHECK(bestOptions > 0);
	played = botTakesTurn(gameData, data, -1);
	CHECK(played != -1);
	if(played > 0){
		CHECK_INT(is_neighbour(start, played, data), 1);
		/*The word it took had as many options as the best on offer. Claiming it
		does not change its own count, since a word is never its own neighbour*/
		CHECK_INT(getNumOptions(played, data), bestOptions);
	}

	freeGameComponentsFLWG(gameData);
	freeDataStructures(data);
}

static void test_flwg_minimax_bot_plays_a_legal_move(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	struct GameData* gameData = initFLWG(data, 3, 6);
	int start;
	int played;

	CHECK_INT(isStartValidFLWG(gameData), 1);
	start = gameData->currWordId;

	/*Any positive bot type is a minimax depth. Three is deep enough to exercise
	the search and shallow enough to stay a unit test*/
	played = botTakesTurn(gameData, data, 3);
	CHECK(played != -1);
	if(played > 0){
		CHECK_INT(is_neighbour(start, played, data), 1);
		CHECK_INT(gameData->currWordId, played);
		CHECK_INT(checkIfUsed_WordSet(played, data->wordSet) != 0, 1);
	}

	freeGameComponentsFLWG(gameData);
	freeDataStructures(data);
}

static void test_flwg_deals_a_board_when_the_adjacencies_asked_for_do_not_exist(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	/*Nothing in the dictionary has this many neighbours, so the count is given
	up and the game opens on a word that exists instead of on nothing*/
	struct GameData* gameData = initFLWG(data, 5000, 6000);

	CHECK_NOT_NULL(gameData);
	CHECK(gameData->currWordId != -1);
	CHECK_INT(isStartValidFLWG(gameData), 1);
	/*A real word, playable from: it has somewhere to go*/
	CHECK(getNumAdjacencies(gameData->currWordId, data) > 0);

	freeGameComponentsFLWG(gameData);
	freeDataStructures(data);
}
/* ----------------------------------------------------------------- FLWP --- */

static void test_flwp_walking_the_solution_wins_the_game(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	struct intList* step;
	int rejected = 0;

	CHECK_INT(isStartValid_FLWP(gc), 1);
	CHECK_INT(isGameWonFLWP(gc), 0);

	/*Replaying the solution the search produced has to be a legal game and has
	to end on the goal -- otherwise the solution is not one*/
	for(step = gc->solution->next->next; step != NULL; step = step->next){
		if(userEntersWord_FLWP(Convert_IntToWord(step->data, data->I2W), gc, data) != VALID){
			rejected++;
		}
	}
	CHECK_INT(rejected, 0);
	CHECK_INT(getPrevWordFLWP(gc), gc->goal);
	CHECK_INT(isGameWonFLWP(gc), 1);
	CHECK_INT(gc->numMoves, gc->solution->size - 1);

	/*Every word played is in the list the UI reads back*/
	CHECK_NOT_NULL(getCurrentWordsFLWP(gc));
	CHECK_INT(getCurrentWordsFLWP(gc) == gc->aList, 1);
	/*The list is one string, not one entry per word: the words are joined with
	"->", so its length is 4 characters a word plus 2 for each join*/
	CHECK_INT((int)getCurrentWordsFLWP(gc)->currPrecision,
		(gc->solution->size * 4) + ((gc->solution->size - 1) * 2));

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

static void test_flwp_reset_returns_the_game_to_its_start(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	int second = nth_entry(gc->solution, 1);
	int third = nth_entry(gc->solution, 2);

	CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(second, data->I2W), gc, data), VALID);
	CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(third, data->I2W), gc, data), VALID);
	CHECK_INT(gc->numMoves, 2);

	/*Taking the last word back leaves the one before it current*/
	removeWord_FLWP(Convert_IntToWord(third, data->I2W), gc, data);
	CHECK_INT(getPrevWordFLWP(gc), second);

	ResetFLWP(gc, data);
	CHECK_INT(getPrevWordFLWP(gc), gc->start);
	CHECK_INT(gc->numMoves, 0);
	CHECK_INT(isGameWonFLWP(gc), 0);
	/*The start word is the only one left in the path, and with nothing to join
	to it there is no "->" either*/
	CHECK_INT((int)getCurrentWordsFLWP(gc)->currPrecision, 4);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

static void test_flwp_head_and_tail_hints_bracket_the_solution(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);

	CHECK(gc->solution->size >= 3);

	/*The head hint is the word after the start, the tail hint the word before
	the goal -- one step in from each end, not the ends themselves*/
	CHECK_STR(hintGetHeadAdjacencyFLWP(gc, data),
		Convert_IntToWord(nth_entry(gc->solution, 1), data->I2W));
	CHECK_STR(hintGetTailAdjacencyFLWP(gc, data),
		Convert_IntToWord(nth_entry(gc->solution, gc->solution->size - 2), data->I2W));

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/* ----------------------------------------------------------------- FLWT --- */

static void test_flwt_is_won_once_enough_adjacencies_are_found(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* flwt = initFLWT(3, 10, 30, data);
	int start;
	int found = 0;
	struct intList* adjacency;

	CHECK_INT(isStartValidFLWT(flwt), 1);
	start = flwt->startWordId;
	CHECK_STR(getStartWordFLWT(flwt, data), Convert_IntToWord(start, data->I2W));
	CHECK(hint_letterToConsiderFLWT(flwt, data) != '?');

	/*Find exactly the three that were asked for*/
	for(adjacency = getConnections(start, data->I2W)->next; adjacency != NULL && found < 3; adjacency = adjacency->next){
		if(userEntersWordFLWT(Convert_IntToWord(adjacency->data, data->I2W), flwt, data) == VALID){
			found++;
			CHECK_INT(flwt->numAdjacenciesFound, found);
		}
	}
	CHECK_INT(found, 3);
	CHECK_INT(isGameWonFLWT(flwt), 1);

	/*and the words found are the ones handed back to the UI*/
	/*Unlike the FLWP path, the tutorial writes its words in end to end*/
	CHECK_NOT_NULL(getCurrentWordsFLWT(flwt));
	CHECK_INT((int)getCurrentWordsFLWT(flwt)->currPrecision, 3 * 4);

	freeGameComponentsFLWT(flwt);
	freeDataStructures(data);
}

static void test_flwt_stays_won_after_an_extra_find(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* flwt = initFLWT(2, 10, 30, data);
	int start = flwt->startWordId;
	int found = 0;
	struct intList* adjacency;

	for(adjacency = getConnections(start, data->I2W)->next; adjacency != NULL && found < 3; adjacency = adjacency->next){
		if(userEntersWordFLWT(Convert_IntToWord(adjacency->data, data->I2W), flwt, data) == VALID){
			found++;
		}
	}
	CHECK_INT(found, 3);

	/*Two were asked for and three were found. A game that has been won does not
	come un-won because the player kept going*/
	CHECK_INT(isGameWonFLWT(flwt), 1);

	freeGameComponentsFLWT(flwt);
	freeDataStructures(data);
}

static void test_flwt_refuses_a_word_that_is_not_an_adjacency(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* flwt = initFLWT(3, 10, 30, data);
	int start = flwt->startWordId;
	int distant = -1;
	int id;

	/*A real word, but two or more substitutions away*/
	for(id = 0; id < data->I2W->numWords; id++){
		if(letters_that_differ(Convert_IntToWord(start, data->I2W),
			Convert_IntToWord(id, data->I2W), 4) >= 3){
			distant = id;
			break;
		}
	}
	CHECK(distant != -1);
	if(distant != -1){
		CHECK_INT(userEntersWordFLWT(Convert_IntToWord(distant, data->I2W), flwt, data),
			NOT_ENOUGH_LETTERS_IN_COMMON);
	}
	CHECK_INT(flwt->numAdjacenciesFound, 0);
	CHECK_INT(isGameWonFLWT(flwt), 0);

	freeGameComponentsFLWT(flwt);
	freeDataStructures(data);
}

/* The board the draw rule exists for.
 *
 * TREE has four neighbours; TREK has exactly one, and it is TREE. So a player
 * who plays TREE, answered by a bot that plays TREK, is left with nowhere to
 * go -- the only way out of TREK is a word already spent.
 *
 * How the engine says so matters to everything above it. The bot's turn
 * returns -2, "the player is trapped". It does NOT set wordId to -1, because
 * the bot moved perfectly legally, so isGameWonFLWC still reads -1, in
 * progress. A caller waiting for isGameWonFLWC to return 0 before calling the
 * game a standoff waits forever, and the player sits on a board they cannot
 * move on until the clock runs out.
 */
static void test_flwc_traps_the_player_when_the_board_runs_out(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWC* flwc = initFLWCAtStart("free", NO_WORDS, NO_WORDS, data);
	int botResult;

	CHECK_INT(isStartValidFLWC(flwc), 1);
	CHECK_STR(getStartWordFLWC(flwc, data), "free");

	/*Spend the other two ways out of TREE, so TREK is the bot's only legal
	move whatever kind of bot it is*/
	markUsed_WordSet(Convert_WordToInt("thee", data), data->wordSet);
	markUsed_WordSet(Convert_WordToInt("true", data), data->wordSet);

	CHECK_INT(userEntersWordFLWC("tree", flwc, data), VALID);
	CHECK_INT(isGameWonFLWC(flwc), -1);

	botResult = botTakesTurnFLWC(0, flwc, data);

	CHECK_STR(Convert_IntToWord(flwc->wordId, data->I2W), "trek");
	CHECK_INT(isTrapped(flwc->wordId, data), 1);
	/*-2 is the whole signal: the bot played, and the player cannot answer*/
	CHECK_INT(botResult, -2);
	/*and the game still reads as in progress, which is the trap*/
	CHECK_INT(isGameWonFLWC(flwc), -1);

	freeGameComponentsFLWC(flwc);
	freeDataStructures(data);
}

/* ------------------------------------------------- the board it starts on --- */

/* How many words the shared set is holding, and which one.
 *
 * The word set is one structure reused by every game, and it does double duty:
 * it is both "already played this round" and a general membership set. So the
 * state a mode is handed is whatever the last game left behind, and starting a
 * game has to mean starting from a clear board.
 */
static int usedWordCount(struct DataStructures* data){
	int n = 0;
	int i;
	for(i = 0; i < data->I2W->numWords; i++){
		if(checkIfUsed_WordSet(i, data->wordSet)){
			n++;
		}
	}
	return n;
}

/*Leave the set thoroughly dirty, so a mode that forgets to clear it is caught
rather than flattered by an already empty board*/
static void dirtyTheWordSet(struct DataStructures* data){
	int i;
	for(i = 0; i < data->I2W->numWords; i += 3){
		markUsed_WordSet(i, data->wordSet);
	}
}

/* Every mode begins with one word claimed and one only: the word it starts on.
 *
 * Anything still marked from the last game is a word quietly deleted from this
 * one - it exists, it is spelt correctly, it is one letter away, and the game
 * refuses it as already used. And the start word itself has to be marked, or
 * the player can walk back onto it later, which no mode intends.
 */
static void test_every_mode_starts_from_a_clear_board(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"ties", "pies", "lies", NULL};

	/*FLWG*/
	dirtyTheWordSet(data);
	struct GameData* flwg = initFLWG(data, 1, 30);
	CHECK_INT(isStartValidFLWG(flwg), 1);
	CHECK_INT(usedWordCount(data), 1);
	CHECK_INT(checkIfUsed_WordSet(flwg->currWordId, data->wordSet), 1);
	freeGameComponentsFLWG(flwg);

	/*FLWT*/
	dirtyTheWordSet(data);
	struct GameComponentsFLWT* flwt = initFLWT(3, 10, 30, data);
	CHECK_INT(isStartValidFLWT(flwt), 1);
	CHECK_INT(usedWordCount(data), 1);
	CHECK_INT(checkIfUsed_WordSet(flwt->startWordId, data->wordSet), 1);
	freeGameComponentsFLWT(flwt);

	/*FLWP*/
	dirtyTheWordSet(data);
	struct GameComponents* flwp = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	CHECK_INT(usedWordCount(data), 1);
	CHECK_INT(checkIfUsed_WordSet(flwp->start, data->wordSet), 1);
	freeGameComponentsFLWP(flwp, data);

	/*FLWC*/
	dirtyTheWordSet(data);
	struct GameComponentsFLWC* flwc = initFLWC(1, 30, goalWords, NO_WORDS,
		2, 0, 2, 0, 1, 30, 8, data);
	CHECK_INT(isStartValidFLWC(flwc), 1);
	CHECK_INT(usedWordCount(data), 1);
	CHECK_INT(checkIfUsed_WordSet(flwc->wordId, data->wordSet), 1);
	freeGameComponentsFLWC(flwc);

	/*FLWGP, which builds an FLWC and an FLWP over the same set*/
	dirtyTheWordSet(data);
	struct GameComponentsFLWGP* flwgp = initiateFLWGP(1, 30, goalWords, NO_WORDS,
		2, 0, 2, 0, 1, 30, data);
	CHECK_NOT_NULL(flwgp->flwpComponents);
	if(flwgp->flwpComponents != NULL){
		CHECK_INT(usedWordCount(data), 1);
		CHECK_INT(checkIfUsed_WordSet(flwgp->flwpComponents->start, data->wordSet), 1);
	}
	freeGameComponentsFLWGP(flwgp, data);

	freeDataStructures(data);
}

/* A board that can be won in one move is not a board.
 *
 * A constraint game is dealt by looking for a goal word a given distance off,
 * but the player does not win by reaching that word - they win by reaching any
 * word the rule admits. When the rule is a common one there is very often a
 * nearer one, and the board is over in a move whatever distance it was dealt
 * at. Asking for a word containing J, Q, X or Z three to five moves away dealt
 * VEAL, which is one letter from ZEAL.
 *
 * So the floor is asked of the nearest winning word rather than of the chosen
 * one, and it is never relaxed - the search may give up on how far a goal is
 * allowed to be, never on how near.
 */
static int nearestGoalDistance(int id, struct WordSet* goals, struct DataStructures* data){
	int frontier[4096];
	int distance[4096];
	int head = 0, tail = 0, i;
	struct WordSet* seen = init_WordSet(data->I2W->numWords);
	int answer = -1;

	markUsed_WordSet(id, seen);
	frontier[tail] = id; distance[tail] = 0; tail++;

	while(head < tail && answer < 0){
		int curr = frontier[head];
		int d = distance[head];
		struct intList* conn;
		head++;
		if(d > 0 && checkIfUsed_WordSet(curr, goals)){
			answer = d;
			break;
		}
		if(d >= 6 || tail >= 4000){
			continue;
		}
		conn = getConnections(curr, data->I2W);
		while(conn->next != NULL){
			conn = conn->next;
			if(checkIfUsed_WordSet(conn->data, seen)){
				continue;
			}
			markUsed_WordSet(conn->data, seen);
			frontier[tail] = conn->data; distance[tail] = d + 1; tail++;
		}
	}
	free_WordSet(seen);
	(void)i;
	return answer;
}

/* A board is always a choice.
 *
 * The adjacency band is a preference and is given up when the dictionary has
 * nothing like what was asked for, which is right. The last round used to give
 * it up altogether, and that is worse than it sounds, because the checks that
 * are never relaxed all get easier the fewer moves a word has: a winner cannot
 * be dealt within two moves of a word if almost nothing is within two moves of
 * it, and a player cannot be forced onto a forbidden word if there is nowhere
 * to force them. So once adjacency stopped counting, the dead ends of the
 * graph were not merely allowed - they were the best candidates in it.
 *
 * DEMO is what came out, over and over. Its only neighbour in the four letter
 * dictionary is MEMO, and MEMO's only neighbour is DEMO: a two word island
 * where nothing the rule forbids can be reached at all. Every never-relaxed
 * check passed and the board had exactly one move in it.
 */
static void test_flwc_always_deals_a_board_with_somewhere_to_go(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	/* Keep me off words ending in CK - the campaign's own level 23. */
	char* avoidWords[512];
	int numAvoid = 0;
	int deal, dealt = 0, thinnest = 1000;
	int seenWords[64];
	int numSeen = 0;

	for(int i = 0; i < data->I2W->numWords && numAvoid < 511; i++){
		char* w = Convert_IntToWord(i, data->I2W);
		if(w != NULL && w[2] == 'c' && w[3] == 'k'){
			avoidWords[numAvoid++] = w;
		}
	}
	avoidWords[numAvoid] = NULL;
	CHECK(numAvoid > 10);

	/* Asked for a board eight or nine moves from anything ending in CK, which
		this dictionary does not contain, so the search must give every
		preference up in turn and arrive at the round that used to accept
		anything at all. That is the round DEMO came from. */
	for(deal = 0; deal < 30; deal++){
		struct GameComponentsFLWC* flwc = initFLWC(15, 24, NO_WORDS, avoidWords,
			0, 8, 0, 9, 0, 0, 4, data);
		if(flwc == NULL || flwc->wordId < 0){
			if(flwc != NULL){ freeGameComponentsFLWC(flwc); }
			continue;
		}
		dealt++;
		int waysOut = data->I2W->array[flwc->wordId]->numConnections;
		if(waysOut < thinnest){ thinnest = waysOut; }
		CHECK(waysOut >= FEWEST_WAYS_OUT);

		/* And how many different boards came out of thirty deals. The fault was
			noticed as repetition before it was noticed as a dead end - the same
			word over and over - because the words that passed every other check
			were a tiny set. */
		int already = 0;
		for(int s = 0; s < numSeen; s++){
			if(seenWords[s] == flwc->wordId){ already = 1; break; }
		}
		if(!already && numSeen < 64){ seenWords[numSeen++] = flwc->wordId; }
		freeGameComponentsFLWC(flwc);
	}

	/* And it still deals a board. The floor is only worth having if giving the
		player somewhere to go did not cost them the game entirely. */
	CHECK(dealt > 20);
	CHECK(thinnest >= FEWEST_WAYS_OUT);
	/* Ten different openings out of thirty deals. Not a demand for thirty - the
		rule genuinely narrows the field and a repeat is fair - but one or two
		words answering for every board is the thing that was wrong. */
	CHECK(numSeen >= 10);

	freeDataStructures(data);
}

static void test_flwc_never_deals_a_board_won_in_one_move(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	/* Every word that starts and ends with the same letter - the campaign's own
		level 28, and the rule that was a one move board in thirty-seven per cent
		of its openings. */
	char* goalWords[512];
	int numGoals = 0;
	int deal, nearest, dealt = 0;

	for(int i = 0; i < data->I2W->numWords && numGoals < 511; i++){
		char* w = Convert_IntToWord(i, data->I2W);
		if(w != NULL && w[0] == w[3]){
			goalWords[numGoals++] = w;
		}
	}
	goalWords[numGoals] = NULL;
	CHECK(numGoals > 20);

	/* Dealt many times over, because the fault was a minority of openings rather
		than all of them, and because the search gives up on its preferences a
		round at a time - it was the third round of giving up that used to let a
		neighbour through. */
	for(deal = 0; deal < 40; deal++){
		struct GameComponentsFLWC* flwc = initFLWC(12, 20, goalWords, NO_WORDS,
			2, 0, 3, 0, 1, 30, 7, data);
		if(flwc == NULL || flwc->wordId < 0){
			if(flwc != NULL){ freeGameComponentsFLWC(flwc); }
			continue;
		}
		dealt++;
		nearest = nearestGoalDistance(flwc->wordId, flwc->goalWords, data);
		/* -1 is no winning word within six moves, which is far enough. */
		CHECK(nearest < 0 || nearest >= 2);
		freeGameComponentsFLWC(flwc);
	}
	CHECK(dealt > 30);

	/* The round the fault actually lived in.
	 *
	 * Nothing in this dictionary has a word of this rule five moves off, so the
	 * search cannot have what it asked for and gives its preferences up a round
	 * at a time - and the band it is giving up walks its own floor down with it:
	 * five, then four, then three, then two, then one. It was that round that
	 * dealt VEAL next to ZEAL. Asking for the impossible must still not produce
	 * a board that is over in a move. */
	for(deal = 0; deal < 20; deal++){
		struct GameComponentsFLWC* flwc = initFLWC(12, 20, goalWords, NO_WORDS,
			5, 0, 6, 0, 1, 30, 7, data);
		if(flwc == NULL || flwc->wordId < 0){
			if(flwc != NULL){ freeGameComponentsFLWC(flwc); }
			continue;
		}
		nearest = nearestGoalDistance(flwc->wordId, flwc->goalWords, data);
		CHECK(nearest < 0 || nearest >= 2);
		freeGameComponentsFLWC(flwc);
	}

	freeDataStructures(data);
}
/* The road the loss screen offers to show.
 *
 * From where the player is standing rather than from the opening word: they
 * have usually moved, and a route from somewhere they are not explains
 * nothing. It walks around the words they have spent for the same reason the
 * distance does - a route through one is not a route they have.
 */
static void test_flwp_hands_back_the_road_from_here(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPAtStart("ware", 4, 8, 1, 30, data);
	char* road;
	int steps;

	/* Standing on the opening word, the road is as long as the distance says,
		counting the word being stood on as well as the goal. */
	road = routeToGoalFLWP(gc, data);
	CHECK_NOT_NULL(road);
	if(road != NULL){
		steps = 0;
		for(int i = 0; road[i] != 0; i++){ if(road[i] == 32){ steps++; } }
		CHECK_INT(steps, distanceToGoalFLWP(gc, data) + 1);
		/* It starts where the player is and ends on the goal. */
		CHECK_INT(strncmp(road, "ware", 4), 0);
		free(road);
	}

	/* A step along it shortens it by exactly one. */
	CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(nth_entry(gc->solution, 1), data->I2W), gc, data), VALID);
	road = routeToGoalFLWP(gc, data);
	CHECK_NOT_NULL(road);
	if(road != NULL){
		steps = 0;
		for(int i = 0; road[i] != 0; i++){ if(road[i] == 32){ steps++; } }
		CHECK_INT(steps, distanceToGoalFLWP(gc, data) + 1);
		free(road);
	}

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/* And on the three letter dictionary, where the word and the gap after it are
 * not the same width as they are on the four. */
static void test_flwp_road_reads_on_three_letter_boards(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	struct GameComponents* gc = initiateFLWP(3, 12, 3, 5, 3, 12, data);
	char* road = routeToGoalFLWP(gc, data);

	CHECK_NOT_NULL(road);
	if(road != NULL){
		int steps = 0;
		int i;
		/* As many words as the distance says, and every character a letter or a
			single separating space.
			
			The separator used to be written four along whatever the word length,
			so on a three letter board it landed on the first letter of the next
			word and the gap it should have filled was left as whatever malloc
			had returned. Checking only that each character is a letter does not
			catch it: that memory comes back zeroed often enough that the string
			simply ends after the first word, and one word is all letters. It is
			the count that tells. */
		for(i = 0; road[i] != 0; i++){
			CHECK(road[i] == 32 || (road[i] >= 97 && road[i] <= 122));
			if(road[i] == 32){ steps++; }
		}
		CHECK_INT(steps, distanceToGoalFLWP(gc, data) + 1);
		CHECK_INT(i, steps * 4);
		free(road);
	}

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}
/* The number the pathfinder's bound is made of.
 *
 * Asked again after every move, so it has to answer from where the player is
 * standing - and it has to walk around the words they have spent, because a
 * route through one is not a route they have. On a board where calling it
 * wrong costs the level, counting a spent word would be the one mistake that
 * tells a player a lost board is still winnable.
 */
static void test_flwp_distance_answers_from_where_the_player_is(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPAtStart("ware", 4, 8, 1, 30, data);
	int atStart;

	CHECK_INT(gc->start, Convert_WordToInt("ware", data));
	atStart = distanceToGoalFLWP(gc, data);
	CHECK(atStart > 0);
	/*the same answer the board was solved to, before anybody has moved*/
	CHECK_INT(atStart, hintGetMinAdjacenciesFLWP(gc, data));

	/*A step along the solution brings it down by exactly one*/
	CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(nth_entry(gc->solution, 1), data->I2W), gc, data), VALID);
	CHECK_INT(distanceToGoalFLWP(gc, data), atStart - 1);

	/*and standing on the goal is no distance at all*/
	while(distanceToGoalFLWP(gc, data) > 0){
		int next = nth_entry(gc->solution, gc->numMoves + 1);
		if(next < 0){
			break;
		}
		CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(next, data->I2W), gc, data), VALID);
	}
	CHECK_INT(distanceToGoalFLWP(gc, data), 0);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/*A goal with nothing leading to it has no distance to report, and must say so
rather than search for ever*/
static void test_flwp_distance_reports_no_way_through(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPAtStart("ware", 4, 8, 1, 30, data);
	int i;

	/*Spend the whole dictionary bar the word being stood on: there is now no
	route anywhere, because every step would be through a used word*/
	for(i = 0; i < data->I2W->numWords; i++){
		if(i != gc->prevInput && i != gc->goal){
			markUsed_WordSet(i, data->wordSet);
		}
	}
	CHECK_INT(distanceToGoalFLWP(gc, data), -1);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/* A board dealt with no way through.
 *
 * The four letter dictionary is one body of 1828 words and ninety odd islands,
 * so a goal on an island cannot be reached from the body whatever is played.
 * That is the board this deals, and everything downstream has to cope with a
 * game whose solution does not exist - the hints that read it, the distance
 * that searches for it, and the free that takes it apart.
 */
static void test_flwp_deals_a_board_with_no_way_through(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initFLWPUnreachable(4, 16, data);

	CHECK(gc->start != -1);
	CHECK(gc->goal != -1);
	CHECK(gc->start != gc->goal);
	/*no route, and it says so rather than pretending to one*/
	CHECK_NULL(gc->solution);
	CHECK_INT(distanceToGoalFLWP(gc, data), -1);

	/*the hints that walk the solution must not walk a NULL*/
	CHECK_INT(hintGetMinAdjacenciesFLWP(gc, data), -1);
	CHECK_NULL(hintGetHeadAdjacencyFLWP(gc, data));
	CHECK_NULL(hintGetTailAdjacencyFLWP(gc, data));

	/*and the start word still reads, because the board is playable even though
	it cannot be finished*/
	CHECK_NOT_NULL(getStartWordFLWP(gc, data));

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

/*The goal really is out of reach, not merely far: nothing the player can play
from the start ever arrives at it*/
static void test_flwp_unreachable_goal_is_truly_unreachable(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int round;

	for(round = 0; round < 12; round++){
		struct GameComponents* gc = initFLWPUnreachable(4, 16, data);
		CHECK(gc->start != -1);
		if(gc->start != -1){
			/*walk everywhere the start can go and confirm the goal is not there*/
			int numWords = data->I2W->numWords;
			int* seen = calloc(numWords, sizeof(int));
			int* queue = malloc(sizeof(int) * numWords);
			int head = 0, tail = 0, found = 0;
			queue[tail++] = gc->start;
			seen[gc->start] = 1;
			while(head < tail){
				struct intList* c = getConnections(queue[head++], data->I2W);
				for(c = c->next; c != NULL; c = c->next){
					if(c->data == gc->goal){
						found = 1;
					}
					if(!seen[c->data]){
						seen[c->data] = 1;
						queue[tail++] = c->data;
					}
				}
			}
			CHECK_INT(found, 0);
			free(seen);
			free(queue);
		}
		freeGameComponentsFLWP(gc, data);
	}
	freeDataStructures(data);
}

void suite_modes(void){
	printf("\n-- game modes --\n");
	RUN_TEST(test_flwc_start_word_satisfies_its_parameters);
	RUN_TEST(test_flwc_traps_the_player_when_the_board_runs_out);
	RUN_TEST(test_every_mode_starts_from_a_clear_board);
	RUN_TEST(test_flwc_never_deals_a_board_won_in_one_move);
	RUN_TEST(test_flwc_always_deals_a_board_with_somewhere_to_go);
	RUN_TEST(test_flwp_hands_back_the_road_from_here);
	RUN_TEST(test_flwp_road_reads_on_three_letter_boards);
	RUN_TEST(test_flwp_distance_answers_from_where_the_player_is);
	RUN_TEST(test_flwp_distance_reports_no_way_through);
	RUN_TEST(test_flwp_deals_a_board_with_no_way_through);
	RUN_TEST(test_flwp_unreachable_goal_is_truly_unreachable);
	RUN_TEST(test_flwc_deals_a_board_when_the_distances_asked_for_do_not_exist);
	RUN_TEST(test_flwc_is_won_by_reaching_a_goal_word);
	RUN_TEST(test_flwic_is_lost_by_reaching_an_avoid_word);
	RUN_TEST(test_flwc_refuses_an_illegal_word_without_moving);
	RUN_TEST(test_flwc_bot_takes_a_legal_turn);
	RUN_TEST(test_flwc_bot_handles_every_bot_type);
	RUN_TEST(test_flwc_winnability_treats_a_dead_end_as_a_loss);
	RUN_TEST(test_get_all_words_lists_the_dictionary);
	RUN_TEST(test_flwgp_composes_a_path_and_a_challenge);
	RUN_TEST(test_flwgp_undo_and_redo_keep_both_halves_in_step);
	RUN_TEST(test_flwg_bot_types_each_play_their_own_way);
	RUN_TEST(test_flwg_minimax_bot_plays_a_legal_move);
	RUN_TEST(test_flwg_deals_a_board_when_the_adjacencies_asked_for_do_not_exist);
	RUN_TEST(test_flwp_walking_the_solution_wins_the_game);
	RUN_TEST(test_flwp_reset_returns_the_game_to_its_start);
	RUN_TEST(test_flwp_head_and_tail_hints_bracket_the_solution);
	RUN_TEST(test_flwt_is_won_once_enough_adjacencies_are_found);
	RUN_TEST(test_flwt_stays_won_after_an_extra_find);
	RUN_TEST(test_flwt_refuses_a_word_that_is_not_an_adjacency);
}
