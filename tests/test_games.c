/*
Title: test_games.c
Description: Tests for the game APIs in src/api -- input validation, the FLWP
pathfinder and its undo/redo history, the FLWT tutorial and the FLWG bot.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWT-API.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/flwp/includes/PathGameComponents.h"
#include "../src/flwg/includes/FLWGGame.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"

/*Returns the id of the first neighbour of id that has not been used yet*/
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

static void test_check_input_classifies_words(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int ware = Convert_WordToInt("ware", data);
	int care = Convert_WordToInt("care", data);
	int id;
	int distantWord = -1;

	CHECK(ware != -1);
	CHECK(care != -1);

	/*A single substitution is the only accepted move*/
	CHECK_INT(Check_Input(ware, "care", data), VALID);

	/*Length is rejected before anything else*/
	CHECK_INT(Check_Input(ware, "war", data), TOO_SHORT);
	CHECK_INT(Check_Input(ware, "wares", data), TOO_LONG);

	/*The same word over again shares every letter*/
	CHECK_INT(Check_Input(ware, "ware", data), TOO_MANY_LETTERS_IN_COMMON);

	/*A word the dictionary does not have*/
	CHECK_INT(Check_Input(ware, "zzzz", data), WORD_DOES_NOT_EXIST);

	/*Two or more substitutions away is too far. Picked from the dictionary so
	the test does not depend on a hard coded word being present*/
	for(id = 0; id < data->I2W->numWords; id++){
		if(letters_that_differ("ware", Convert_IntToWord(id, data->I2W), 4) >= 3){
			distantWord = id;
			break;
		}
	}
	CHECK(distantWord != -1);
	if(distantWord != -1){
		CHECK_INT(Check_Input(ware, Convert_IntToWord(distantWord, data->I2W), data),
			NOT_ENOUGH_LETTERS_IN_COMMON);
	}

	/*A word already played is refused even though it is a legal substitution*/
	markUsed_WordSet(care, data->wordSet);
	CHECK_INT(Check_Input(ware, "care", data), WORD_USED);
	reset_WordSet(data->wordSet);

	freeDataStructures(data);
}

static void test_flwp_start_goal_and_solution(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	struct intList* step;
	int stepCount = 0;
	int badStep = 0;
	int lastId = -1;

	CHECK_NOT_NULL(gc);
	CHECK_INT(isStartValid_FLWP(gc), 1);
	CHECK(gc->start != -1);
	CHECK(gc->goal != -1);
	CHECK(gc->start != gc->goal);

	/*The start and goal have to satisfy the adjacency range that was asked for*/
	CHECK(getNumAdjacencies(gc->start, data) >= 4);
	CHECK(getNumAdjacencies(gc->start, data) <= 16);
	CHECK(getNumAdjacencies(gc->goal, data) >= 4);
	CHECK(getNumAdjacencies(gc->goal, data) <= 16);

	/*A solution exists and actually walks from the start to the goal*/
	CHECK_NOT_NULL(gc->solution);
	CHECK_INT(gc->solution->next->data, gc->start);
	CHECK_INT(FindLast_IntLL(gc->solution), gc->goal);

	step = gc->solution->next;
	while(step != NULL){
		if(lastId != -1){
			char* previousWord = Convert_IntToWord(lastId, data->I2W);
			char* currentWord = Convert_IntToWord(step->data, data->I2W);
			if(letters_that_differ(previousWord, currentWord, 4) != 1){
				badStep++;
			}
		}
		lastId = step->data;
		stepCount++;
		step = step->next;
	}
	CHECK_INT(badStep, 0);
	CHECK_INT(stepCount, gc->solution->size);

	/*The distance the hint reports is the number of substitutions, not words*/
	CHECK_INT(hintGetMinAdjacenciesFLWP(gc, data), gc->solution->size - 1);
	CHECK(hintGetMinAdjacenciesFLWP(gc, data) >= 4);
	CHECK(hintGetMinAdjacenciesFLWP(gc, data) <= 8);

	/*The game is not won before a move is made*/
	CHECK_INT(isGameWonFLWP(gc), 0);
	CHECK_STR(getStartWordFLWP(gc, data), Convert_IntToWord(gc->start, data->I2W));
	CHECK_STR(getGoalWordFLWP(gc, data), Convert_IntToWord(gc->goal, data->I2W));

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

static void test_flwp_undo_and_redo_walk_the_history(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	int secondWord = gc->solution->next->next->data;
	int thirdWord = gc->solution->next->next->next->data;

	CHECK_INT(getPrevWordFLWP(gc), gc->start);

	/*Two moves along the known solution*/
	CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(secondWord, data->I2W), gc, data), VALID);
	CHECK_INT(getPrevWordFLWP(gc), secondWord);
	CHECK_INT(userEntersWord_FLWP(Convert_IntToWord(thirdWord, data->I2W), gc, data), VALID);
	CHECK_INT(getPrevWordFLWP(gc), thirdWord);
	CHECK_INT(gc->numMoves, 2);

	/*Undo walks back one move at a time*/
	undoMoveFLWP(gc, data);
	CHECK_INT(getPrevWordFLWP(gc), secondWord);
	undoMoveFLWP(gc, data);
	CHECK_INT(getPrevWordFLWP(gc), gc->start);

	/*Undoing past the beginning is a no-op rather than a crash*/
	undoMoveFLWP(gc, data);
	CHECK_INT(getPrevWordFLWP(gc), gc->start);

	/*Redo puts the moves back*/
	redoMoveFLWP(gc, data);
	CHECK_INT(getPrevWordFLWP(gc), secondWord);
	redoMoveFLWP(gc, data);
	CHECK_INT(getPrevWordFLWP(gc), thirdWord);

	/*An illegal move leaves the history untouched*/
	CHECK_INT(userEntersWord_FLWP("zzzz", gc, data), WORD_DOES_NOT_EXIST);
	CHECK_INT(getPrevWordFLWP(gc), thirdWord);
	CHECK_INT(gc->numMoves, 2);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

static void test_flwt_counts_adjacencies_the_user_finds(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* flwt = initFLWT(3, 10, 30, data);
	int neighbour;
	char* neighbourWord;

	CHECK_NOT_NULL(flwt);
	CHECK_INT(isStartValidFLWT(flwt), 1);
	CHECK(getNumAdjacencies(flwt->startWordId, data) >= 10);
	CHECK(getNumAdjacencies(flwt->startWordId, data) <= 30);
	CHECK_INT(flwt->numAdjacenciesFound, 0);
	CHECK_INT(isGameWonFLWT(flwt), 0);
	CHECK(hint_numOptionsFLWT(flwt, data) > 0);

	/*A genuine adjacency of the start word counts*/
	neighbour = first_unused_neighbour(flwt->startWordId, data);
	CHECK(neighbour != -1);
	neighbourWord = Convert_IntToWord(neighbour, data->I2W);
	CHECK_INT(userEntersWordFLWT(neighbourWord, flwt, data), VALID);
	CHECK_INT(flwt->numAdjacenciesFound, 1);

	/*The same word a second time does not count twice*/
	CHECK_INT(userEntersWordFLWT(neighbourWord, flwt, data), WORD_USED);
	CHECK_INT(flwt->numAdjacenciesFound, 1);

	/*Neither does a word that is not in the dictionary*/
	CHECK_INT(userEntersWordFLWT("zzzz", flwt, data), WORD_DOES_NOT_EXIST);
	CHECK_INT(flwt->numAdjacenciesFound, 1);

	/*Three were asked for, so one is not a win*/
	CHECK_INT(isGameWonFLWT(flwt), 0);

	freeGameComponentsFLWT(flwt);
	freeDataStructures(data);
}

static void test_flwg_bot_plays_a_legal_move(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* gameData = initFLWG(data, 4, 4);
	int startId;
	int played;

	CHECK_NOT_NULL(gameData);
	CHECK_INT(isStartValidFLWG(gameData), 1);

	startId = gameData->currWordId;
	/*The requested adjacency range is exact here*/
	CHECK_INT(getNumAdjacencies(startId, data), 4);
	CHECK_INT(hintNumOptionsFLWG(gameData, data), 4);
	CHECK_INT(isTrapped(startId, data), 0);
	CHECK_STR(getCurrWord(gameData, data), Convert_IntToWord(startId, data->I2W));

	/*The random bot has to pick an unused neighbour of the current word*/
	played = botTakesTurn(gameData, data, 0);
	CHECK(played != -1);
	if(played != -1){
		CHECK_INT(letters_that_differ(Convert_IntToWord(startId, data->I2W),
			Convert_IntToWord(played, data->I2W), 4), 1);
		CHECK_INT(gameData->currWordId, played);
		/*and it has to claim the word so nobody replays it*/
		CHECK_INT(checkIfUsed_WordSet(played, data->wordSet) != 0, 1);
	}

	/*The hint letter is the one that changed relative to an option*/
	CHECK(hintLetterToConsiderFLWG(gameData, data) != '?');

	freeGameComponentsFLWG(gameData);
	freeDataStructures(data);
}

static void test_flwg_user_turn_is_validated(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* gameData = initFLWG(data, 4, 4);
	int startId = gameData->currWordId;
	int neighbour = first_unused_neighbour(startId, data);
	char* neighbourWord = Convert_IntToWord(neighbour, data->I2W);

	/*A legal substitution advances the game*/
	CHECK_INT(userTakesTurn(neighbourWord, gameData, data), VALID);
	CHECK_INT(gameData->currWordId, neighbour);
	CHECK_INT(checkIfUsed_WordSet(neighbour, data->wordSet) != 0, 1);

	/*Playing it again is refused and the current word does not move*/
	CHECK_INT(userTakesTurn(neighbourWord, gameData, data), WORD_USED);
	CHECK_INT(gameData->currWordId, neighbour);

	freeGameComponentsFLWG(gameData);
	freeDataStructures(data);
}

void suite_games(void){
	printf("\n-- game APIs --\n");
	RUN_TEST(test_check_input_classifies_words);
	RUN_TEST(test_flwp_start_goal_and_solution);
	RUN_TEST(test_flwp_undo_and_redo_walk_the_history);
	RUN_TEST(test_flwt_counts_adjacencies_the_user_finds);
	RUN_TEST(test_flwg_bot_plays_a_legal_move);
	RUN_TEST(test_flwg_user_turn_is_validated);
}
