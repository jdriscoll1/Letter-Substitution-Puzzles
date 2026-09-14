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

/* A pathfinder board with no way through, on purpose.
 *
 * pathGameBetween refuses a pair with no route between them, and is right to
 * for every board meant to be walked. A board meant not to be is a different
 * thing: the start is real, the goal is real, and the absence of a route is
 * the puzzle. The player is not asked to prove that absence by searching -
 * the board states how far the goal is at least, and on a board like this that
 * number is larger than the moves they have, which is the whole of the proof.
 *
 * The dictionary can supply these: four letter words are one body of 1828 and
 * ninety odd islands, most of them a single word. A goal on an island is
 * unreachable from anywhere in the body, whatever anybody plays.
 *
 * Leaves solution NULL, which is what having no route means. Everything that
 * walks the solution has to expect that - see the guards on the hints below.
 */
struct GameComponents* initFLWPUnreachable(int minAdjacencies, int maxAdjacencies, struct DataStructures* data){
	int numWords = data->I2W->numWords;
	int start = -1;
	int goal = -1;
	int* candidates = malloc(sizeof(int) * numWords);
	int* reachable = malloc(sizeof(int) * numWords);
	int count = 0;
	int i;

	if(candidates == NULL || reachable == NULL){
		free(candidates);
		free(reachable);
		return pathGameBetween(-1, -1, data);
	}

	/*A start with room to move, taken from a shuffled list so the board is not
	the same one every time*/
	for(i = 0; i < numWords; i++){
		int adjacencies = data->I2W->array[i]->numConnections;
		if(adjacencies >= minAdjacencies && adjacencies <= maxAdjacencies){
			candidates[count++] = i;
		}
	}
	if(count == 0){
		free(candidates);
		free(reachable);
		return pathGameBetween(-1, -1, data);
	}
	Shuffle_IntArray(candidates, count);
	start = candidates[0];

	/*Everywhere that start can get to, so the goal can be chosen from
	everywhere it cannot*/
	for(i = 0; i < numWords; i++){
		reachable[i] = 0;
	}
	{
		int head = 0;
		int tail = 0;
		candidates[tail++] = start;
		reachable[start] = 1;
		while(head < tail){
			int curr = candidates[head++];
			struct intList* c = getConnections(curr, data->I2W);
			for(c = c->next; c != NULL; c = c->next){
				if(!reachable[c->data]){
					reachable[c->data] = 1;
					candidates[tail++] = c->data;
				}
			}
		}
	}

	/*Any word the start cannot get to. Counted first and then chosen from, so
	every island is as likely as every other rather than the first one found*/
	count = 0;
	for(i = 0; i < numWords; i++){
		if(!reachable[i]){
			count++;
		}
	}
	if(count > 0){
		int wanted = rand() % count;
		for(i = 0; i < numWords; i++){
			if(!reachable[i]){
				if(wanted == 0){
					goal = i;
					break;
				}
				wanted--;
			}
		}
	}

	free(candidates);
	free(reachable);

	if(goal == -1){
		// every word is reachable from this start, so there is no such board
		return pathGameBetween(-1, -1, data);
	}

	{
		struct GameComponents* gc = malloc(sizeof(struct GameComponents));
		gc->start = start;
		gc->goal = goal;
		gc->minConnections = 4;
		gc->solution = NULL;
		FinishGameComponents(gc, data);
		return gc;
	}
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
