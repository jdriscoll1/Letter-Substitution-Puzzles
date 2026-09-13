/*************************
Name: Debug-API
Purpose: Start any game mode on a word you name. See Debug-API.h.
************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/Debug-API.h"

#include "../../flwp/includes/PathGameComponents.h"
#include "../../flwp/includes/BreadthFirstSearch_FLWP.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../flwc/includes/Challenges.h"
#include "../../structs/includes/ArrayList.h"
#include "../../structs/includes/WordSet.h"

/*The named word's id, or -1 if the dictionary has never heard of it. Everything
here goes through this, so an unknown word fails the same way in every mode.*/
static int idOf(char* word, struct DataStructures* data){
	if(word == NULL){
		return -1;
	}
	return convertWordToInt(word, data);
}

struct GameData* initFLWGAtStart(char* startWord, struct DataStructures* data){
	struct GameData* gameData = malloc(sizeof(struct GameData));
	gameData->currWordId = idOf(startWord, data);
	gameData->difficulty = 0;
	gameData->numPlayers = 2;
	gameData->goalWord = -1;

	reset_WordSet(data->wordSet);
	if(gameData->currWordId != -1){
		markUsed_WordSet(gameData->currWordId, data->wordSet);
	}
	return gameData;
}

/*The two path games share everything but how the goal is arrived at, so they
share this: seat the pair, work out the path between them, then let the ordinary
setup dress the components. A start of -1 is how every path game says it could
not be built, and FinishGameComponents and freeGameComponentsFLWP both expect it.*/
static struct GameComponents* pathGameBetween(int start, int goal, struct DataStructures* data){
	struct GameComponents* gc = malloc(sizeof(struct GameComponents));
	gc->start = start;
	gc->goal = goal;
	gc->minConnections = 4;
	gc->solution = NULL;

	if(start != -1 && goal != -1){
		getSolution_FLWP(start, goal, gc, data);
	}

	// No path joins them, so there is no game here to play
	if(gc->solution == NULL){
		gc->start = -1;
		gc->goal = -1;
		gc->minConnections = 4;
	}

	FinishGameComponents(gc, data);
	return gc;
}

struct GameComponents* initFLWPAtStart(char* startWord, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data){
	int start = idOf(startWord, data);
	int goal = -1;

	if(start != -1){
		goal = chooseGoalBFS_FLWP(start, minDistance, maxDistance, minAdjacenciesToGoal, maxAdjacenciesToGoal, data);
	}
	return pathGameBetween(start, goal, data);
}

struct GameComponents* initFLWPBetween(char* startWord, char* goalWord, struct DataStructures* data){
	int start = idOf(startWord, data);
	int goal = idOf(goalWord, data);

	// A word is not a path to itself
	if(start != -1 && start == goal){
		start = -1;
		goal = -1;
	}
	return pathGameBetween(start, goal, data);
}

struct GameComponentsFLWC* initFLWCAtStart(char* startWord, char** goalWords, char** avoidWords, struct DataStructures* data){
	struct GameComponentsFLWC* flwcComponents = malloc(sizeof(struct GameComponentsFLWC));

	flwcComponents->goalWords = convertCharPtrPtrToWordSet(goalWords, data);
	flwcComponents->avoidWords = convertCharPtrPtrToWordSet(avoidWords, data);
	flwcComponents->wordId = idOf(startWord, data);

	reset_WordSet(data->wordSet);
	if(flwcComponents->wordId != -1){
		markUsed_WordSet(flwcComponents->wordId, data->wordSet);
	}
	return flwcComponents;
}

struct GameComponentsFLWT* initFLWTAtStart(char* startWord, int numTurns, struct DataStructures* data){
	struct GameComponentsFLWT* flwtComponents = malloc(sizeof(struct GameComponentsFLWT));
	int start = idOf(startWord, data);

	// The ceiling is whatever the word actually has, so a named word cannot be
	// given a target it has no way of meeting
	int adjacencies = (start == -1) ? 0 : data->I2W->array[start]->numConnections;

	flwtComponents->startWordId = start;
	flwtComponents->numAdjacenciesFound = 0;
	flwtComponents->minAdjacenciesUserNeedsToFind = numTurns;
	flwtComponents->maxAdjacenciesThatCanBeFound = adjacencies;
	flwtComponents->prevWords = init_ArrayList((adjacencies > 0) ? adjacencies : 1, 5, STR);

	reset_WordSet(data->wordSet);
	if(start != -1){
		markUsed_WordSet(start, data->wordSet);
	}
	return flwtComponents;
}
