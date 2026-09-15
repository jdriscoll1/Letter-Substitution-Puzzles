/*************************
Name: Jordan D. Andrew B.  
Date: Aug 8, 2024
Purpose: A library to encapsulate & organize the code into an API

************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "../includes/FLWG-API.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../flwp/includes/UserInput.h"
#include "../../flwp/includes/BreadthFirstSearch_FLWP.h"
#include "../../flwg/includes/FLWGGame.h"
#include "../../flwc/includes/FLWC.h"
#include "../../flwg/includes/Hints2.h"

/* Every random choice the engine makes runs off rand(), which initDataStructures
seeds from the clock once at startup. Seeding it again by hand makes everything
after it repeatable: which word a game starts on, which word the bot answers
with, which letter a hint offers. That is what a daily puzzle needs - not a
board of the same shape for everyone, but the same board. */
void seedGameRandom(unsigned int seed){
	srand(seed);
}

// Creating and destroying data structures
struct DataStructures* initDataStructures(int fd, int numLetters){

    srand(time(NULL));

	struct DataStructures* data = malloc(sizeof(struct DataStructures));

	// Initialize the Word to Int Hash Map 
	data->W2I = Allocate_WordToInt(); 	
	data->I2W = Allocate_IntToWordStruct(); 	
	Initialize_HashMaps_fd(data->W2I, data->I2W, fd, numLetters); 
	data->wordSet = init_WordSet(data->I2W->numWords); 
	return data; 
}

void freeDataStructures(struct DataStructures* data){
	free_WordSet(data->wordSet); 
	Free_HashMaps(data->W2I, data->I2W); 
	free(data);
}

 // FLWG functions
struct GameData* initFLWG(struct DataStructures* dataStructures, int minAdjacencies, int maxAdjacencies){
	// reset word set	
	struct GameData* gameData = malloc(sizeof(struct GameData)); 
	gameData->currWordId = ChooseStart_Range(dataStructures->I2W, minAdjacencies, maxAdjacencies);
	gameData->difficulty = 0;
	gameData->numPlayers = 2; 
	reset_WordSet(dataStructures->wordSet);
	
	if(gameData->currWordId != -1){
		markUsed_WordSet(gameData->currWordId, dataStructures->wordSet);
        }
	return gameData;
	
	
}

char* getCurrWord(struct GameData* gameData, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameData == NULL){
		return NULL;
	}

	int wordId = gameData->currWordId; 
	char* word = convertIntToWord(wordId, dataStructures);
	return word; 
}

void freeGameComponentsFLWG(struct GameData* gameData){
	/* nothing to work with */
	if(gameData == NULL){
		return;
	}

	free(gameData); 
}

int botTakesTurn(struct GameData* gameData, struct DataStructures* data, int botType){
	/* nothing to work with */
	if(gameData == NULL){
		return -1;
	}

	int result = -1;
	// If the bot type is -1 it is its easiest version
	if(botType == -2){
		result = botPly_Mirror(gameData->currWordId);
	}
	if(botType == -1){
		result = botPly_MaxAdjacencies(gameData->currWordId, NULL, data);
	}
	// If the bot type is 0, it chooses randomly which is a little harder than its easiest variant 
	if(botType == 0){
		result = botPly_Random(gameData->currWordId, data);

    	}
	// If the bot tyep is greater than 0, it uses that as its depth 
    	if(botType > 0){
		result = botPly(gameData->currWordId, botType, data->I2W, data->wordSet);


    	}
	// If the result is -1 the bot has lost
    	if(result == -1){
		return -1; 
    	}
	gameData->currWordId = result;
	if(isTrapped(gameData->currWordId, data) == 1){
		return -2; 
    	}
    	return result;
}

int userTakesTurn(char* userInput, struct GameData* gameData, struct DataStructures* data){
	/* nothing to work with */
	if(gameData == NULL){
		return -1;
	}

	
	// Check if the word is valid
	enum ERROR_CODE result; 
	if((result = Check_Input(gameData->currWordId, (const char*)userInput, data)) != VALID){
		return result; 
	}

	int wordId = convertWordToInt(userInput, data); 
	// Check word is not in word set 
	if(checkIfUsed_WordSet(wordId, data->wordSet)){
		return WORD_USED; 
	} 
	// Add word to word set 
	markUsed_WordSet(wordId, data->wordSet); 
        gameData->currWordId = wordId;
	return VALID;
}
void resetWordSet(struct DataStructures* data){
	reset_WordSet(data->wordSet);
}


struct GameComponents* initiateFLWP(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data){

	return InitializeGameComponents(minAdjacenciesToStart, maxAdjacenciesToStart, minDistance, maxDistance, minAdjacenciesToGoal, maxAdjacenciesToGoal, data);
}

void ResetFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameComponents == NULL){
		return;
	}


	ResetGameComponents(gameComponents, dataStructures->I2W);
}

int getPrevWordFLWP(struct GameComponents* gameComponents){
	/* nothing to work with */
	if(gameComponents == NULL){
		return -1;
	}

    return gameComponents->prevInput;

}
int userEntersWord_FLWP(char* userInput, struct GameComponents *gameComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameComponents == NULL){
		return -1;
	}

    return AddWord_Struct(gameComponents, userInput, dataStructures); 
}
char* convertIntToWord(int wordId, struct DataStructures* data){
	return Convert_IntToWord(wordId, data->I2W); 
}

int convertWordToInt(char* word, struct DataStructures* data){
	return Convert_WordToInt(word, data); 
}

char* getStartWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return NULL;
	}

    return convertIntToWord(gameComponents->start, dataStructures);
}
char* getGoalWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return NULL;
	}

    return convertIntToWord(gameComponents->goal, dataStructures);
}

void undoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return;
	}

    Undo_Struct(gameComponents, dataStructures->I2W);
}


void redoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return;
	}

    Redo_Struct(gameComponents, dataStructures->I2W);
}

struct arrayList *getCurrentWordsFLWP(struct GameComponents *gameComponents) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return NULL;
	}

    return gameComponents->aList;
}

int isGameWonFLWP(struct GameComponents *gameComponents) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return 0;
	}

    return gameComponents->goal == gameComponents->prevInput;
}

void removeWord_FLWP(char* word, struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(gameComponents == NULL){
		return;
	}


    char tempStr[dataStructures->I2W->numLetters + 2];
    tempStr[0] = '-';
    tempStr[dataStructures->I2W->numLetters + 1] = '\0';

    strncpy(&tempStr[1], word, dataStructures->I2W->numLetters);

    //RemoveWord_Struct hands back a freshly allocated copy of the word
    free(RemoveWord_Struct(gameComponents, tempStr, 0, dataStructures)); 
}

int isStartValid_FLWP(struct GameComponents* gameComponents){
	/* nothing to work with */
	if(gameComponents == NULL){
		return 0;
	}

	return gameComponents->start != -1; 	

}
void freeGameComponentsFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameComponents == NULL){
		return;
	}

	FreeGameComponents(gameComponents, dataStructures->I2W); 

}



/* The rest of a generalized board, once its first word is settled.
 *
 * Everything below the start word is the same work however that word was
 * arrived at: the path components, the route to the nearest goal, and putting
 * the start back on the board after the search has finished with it. Split out
 * so that a board opening on a named word is the same board as one opening on
 * a word this file picked, rather than a second copy of the setup that has to
 * be kept in step with this one by hand.
 */
struct GameComponentsFLWGP* buildFLWGPFromFLWC(struct GameComponentsFLWC* flwcComponents, int minGoalDistance, struct DataStructures* dataStructures){
	if(flwcComponents->wordId == -1){

		struct GameComponentsFLWGP* flwgpComponents = malloc(sizeof(struct GameComponentsFLWGP));
		flwgpComponents->flwcComponents = flwcComponents; 
		flwgpComponents->flwpComponents = NULL; 
		return flwgpComponents; 
	}
	struct GameComponents* flwpComponents = malloc(sizeof(struct GameComponents)); 
	flwpComponents->start = flwcComponents->wordId; 
	flwpComponents->goal = -1;  
	markUnused_WordSet(flwpComponents->start, dataStructures->wordSet); 
	//Sets the minimum number of connection
	flwpComponents->minConnections = minGoalDistance;
	//Sets the number of moves
	flwpComponents->numMoves = 0;
	//Instantiates the number of undo calls 
	flwpComponents->undoCalls = 0; 
 	//Instantiates the number of hint points
 	flwpComponents->hc = NULL; 
	flwpComponents->prevInput = flwpComponents->start; 
	//Initialize the arrayList 
	flwpComponents->aList = init_ArrayList(dataStructures->I2W->numLetters * (minGoalDistance * 1.5), dataStructures->I2W->numLetters * (minGoalDistance), STR);
	//Instantiate the input storage 
	flwpComponents->storage = malloc(sizeof(struct GenericLinkedListNode)); 
	flwpComponents->storage->next = NULL; 
	flwpComponents->storage->prev =  NULL; 

	//Creates the storage header
	flwpComponents->storageHeader = flwpComponents->storage; 

	//Instantiates the user connection
	flwpComponents->userConnections = malloc(sizeof(struct intList)); 
	flwpComponents->userConnections->size = 0; 
	flwpComponents->userConnections->next = NULL; 

	//There is no input to be freed
	//Insert the word into the back of the word linked list
	AddToBack_IntLL(flwpComponents->start, flwpComponents->userConnections); 
 
 	addString_ArrayList(Convert_IntToWord(flwpComponents->start, dataStructures->I2W), dataStructures->I2W->numLetters, flwpComponents->aList); 
 	 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(flwpComponents->storage, INT_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(flwpComponents->userConnections, flwpComponents->storage, 1, INT_LL);
	
	/*GET SOLUTION FLWGP*/

	// Start parameters for getting a solution
	struct StartWordParametersFLWC p = {
	.goalWords=flwcComponents->goalWords,
	.avoidWords=flwcComponents->avoidWords,
	.minGoalDistance=0,
	.maxGoalDistance=100,
	.minAvoidDistance=0,
	.maxAvoidDistance=100,
	.minAdjacencies=1,
	.maxAdjacencies=100,
	.minGoalAdjacencies=1,
	.maxGoalAdjacencies=100
	};
	struct arrayList* solution_ArrayList = getPathToNearestWordInWordSet(flwcComponents->wordId, p, dataStructures);
	struct intList* solution = init_IntLL(); 
	for(int i = 0; i < solution_ArrayList->currPrecision; i++){
		AddToBack_IntLL(((int*)(solution_ArrayList->list))[i], solution); 
	}
	free_ArrayList(solution_ArrayList); 
	flwpComponents->solution = solution; 










	/*END GET SOLUTION FLWGP*/

	/* The start word goes back on the board.
	 *
	 * It was taken off a few lines above so the solution search could set out
	 * from it: a word already marked is one the search will not leave from. But
	 * that is a detail of finding the route, not of playing it, and nothing put
	 * it back - so this was the one mode that began with nothing claimed at all,
	 * and the only one where a player could walk back onto the word they started
	 * on. Every other mode marks its start in init and means it. */
	markUsed_WordSet(flwpComponents->start, dataStructures->wordSet);
	
	struct GameComponentsFLWGP* flwgpComponents = malloc(sizeof(struct GameComponentsFLWGP)); 
	flwgpComponents->flwcComponents = flwcComponents; 
	flwgpComponents->flwpComponents = flwpComponents; 
	return flwgpComponents; 

}

struct GameComponentsFLWGP* initiateFLWGP(int minAdjacenciesToStart,  int maxAdjacenciesToStart, char** goalWords, char** avoidWords, int minGoalDistance, int minAvoidDistance, int maxGoalDistance, int maxAvoidDistance, int minGoalAdjacencies, int maxGoalAdjacencies, struct DataStructures* dataStructures){

	struct GameComponentsFLWC* flwcComponents = initFLWC(minAdjacenciesToStart, maxAdjacenciesToStart, goalWords, avoidWords, minGoalDistance, minAvoidDistance, maxGoalDistance, maxAvoidDistance, minGoalAdjacencies, maxGoalAdjacencies, -1, dataStructures);
	return buildFLWGPFromFLWC(flwcComponents, minGoalDistance, dataStructures);
}

void freeGameComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return;
	}

	if(flwgpComponents->flwpComponents != NULL){
		Free_IntLL(flwgpComponents->flwpComponents->userConnections);
		Free_GenericLinkedList(flwgpComponents->flwpComponents->storageHeader);
		free_ArrayList(flwgpComponents->flwpComponents->aList);
		Free_IntLL(flwgpComponents->flwpComponents->solution); 
		free(flwgpComponents->flwpComponents);

	}
	freeGameComponentsFLWC(flwgpComponents->flwcComponents); 
	free(flwgpComponents); 
} 

struct GameComponentsFLWC* getFLWCComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return NULL;
	}

	return flwgpComponents->flwcComponents; 
}

struct GameComponents* getFLWPComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return NULL;
	}

	return flwgpComponents->flwpComponents; 
}

int userEntersWord_FLWGP(char* userInput, struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return -1;
	}


	int result = userEntersWord_FLWP(userInput, flwgpComponents->flwpComponents, dataStructures); 
	if(result == 0){
		markUnused_WordSet(Convert_WordToInt(userInput, dataStructures), dataStructures->wordSet); 
		userEntersWordFLWC(userInput, flwgpComponents->flwcComponents, dataStructures); 
		markUnused_WordSet(Convert_WordToInt(userInput, dataStructures), dataStructures->wordSet); 
	}
	return result; 

}


void undoMoveFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return;
	}

	Undo_Struct(flwgpComponents->flwpComponents, dataStructures->I2W);
	flwgpComponents->flwcComponents->wordId = getPrevWordFLWP(flwgpComponents->flwpComponents); 

}

void redoMoveFLWGP(struct GameComponentsFLWGP *flwgpComponents, struct DataStructures* dataStructures) {
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return;
	}

	Redo_Struct(flwgpComponents->flwpComponents, dataStructures->I2W);
	flwgpComponents->flwcComponents->wordId = getPrevWordFLWP(flwgpComponents->flwpComponents); 
}
int isStartValid_FLWGP(struct GameComponentsFLWGP *flwgpComponents){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return 0;
	}
	if(flwgpComponents->flwcComponents == NULL){
		return 0;
	}


	return  isStartValidFLWC(flwgpComponents->flwcComponents);
}

int isStartValidFLWG(struct GameData* gameData){
	/* nothing to work with */
	if(gameData == NULL){
		return 0;
	}


	return gameData->currWordId != -1; 

}


char hintLetterToConsiderFLWG(struct GameData* flwgComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwgComponents == NULL){
		return '?';
	}

	return letterToConsiderHint(flwgComponents->currWordId, data); 
}

int hintNumOptionsFLWG(struct GameData* flwgComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwgComponents == NULL){
		return -1;
	}

	return numOptionsHint(flwgComponents->currWordId, data); 
}



char* hintGetHeadAdjacencyFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameComponents == NULL){
		return NULL;
	}

	/* A board with no route has no solution to read a word out of. Boards like
	   that are dealt on purpose now, so every one of these has to say it has
	   nothing rather than walk a NULL. */
	if(gameComponents->solution == NULL || gameComponents->solution->size < 3){
		return NULL;
	}
	return Convert_IntToWord(gameComponents->solution->next->next->data, dataStructures->I2W);
}
char* hintGetTailAdjacencyFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameComponents == NULL){
		return NULL;
	}

	if(gameComponents->solution == NULL || gameComponents->solution->size < 2){
		return NULL;
	}
	int size = gameComponents->solution->size; 
	struct intList* curr = gameComponents->solution; 
	for(int i = 0; i < size - 1; i++){
		curr = curr->next; 	
		

	}
	return Convert_IntToWord(curr->data, dataStructures->I2W); 

}
/* How far the goal is from where the player is standing, over words they have
 * not already spent. -1 when there is no way through at all.
 *
 * Not the same question as hintGetMinAdjacenciesFLWP, which reports the route
 * the board was solved to when it was built and never changes. This is asked
 * again after every move, and it is the number the pathfinder's bound is made
 * of, so it has to answer from the current word rather than the first one.
 *
 * Words already played are walked around rather than through. A route that
 * needs a word the player has spent is not a route the player has, so counting
 * it would tell them a board was still winnable when it is not - and on a
 * board where being wrong costs the level, that is the one mistake this must
 * not make.
 *
 * It keeps its own visited array rather than borrowing wordData's prevID. That
 * field is shared scratch space the searches and the hints both write, and
 * this runs in the middle of a live game.
 */
int distanceToGoalFLWP(struct GameComponents* gameComponents, struct DataStructures* data){
	if(gameComponents == NULL || data == NULL || data->I2W == NULL){
		return -1;
	}

	int numWords = data->I2W->numWords;
	int start = gameComponents->prevInput;
	int goal = gameComponents->goal;

	if(start < 0 || goal < 0 || start >= numWords || goal >= numWords){
		return -1;
	}
	if(start == goal){
		return 0;
	}

	int* queue = malloc(sizeof(int) * numWords);
	int* depth = malloc(sizeof(int) * numWords);
	if(queue == NULL || depth == NULL){
		free(queue);
		free(depth);
		return -1;
	}
	for(int i = 0; i < numWords; i++){
		depth[i] = -1;
	}

	int head = 0;
	int tail = 0;
	int answer = -1;
	queue[tail++] = start;
	depth[start] = 0;

	while(head < tail && answer == -1){
		int curr = queue[head++];
		struct intList* c = getConnections(curr, data->I2W);
		for(c = c->next; c != NULL; c = c->next){
			int next = c->data;
			if(next == goal){
				answer = depth[curr] + 1;
				break;
			}
			if(depth[next] != -1){
				continue;
			}
			// spent words are walked around, not through
			if(checkIfUsed_WordSet(next, data->wordSet)){
				continue;
			}
			depth[next] = depth[curr] + 1;
			queue[tail++] = next;
		}
	}

	free(queue);
	free(depth);
	return answer;
}

/* The route still open from where the player is standing to the goal.
 *
 * distanceToGoalFLWP already answers how far, by the same search and with the
 * same rule - a word already spent is walked around rather than through,
 * because a route through one is not a route the player has. This is that
 * search keeping the trail it walked so the road itself can be handed back
 * rather than only its length.
 *
 * Asked for once, when a board has been lost, so the loss screen can offer to
 * show it. Not the solution the board was dealt with: that one starts at the
 * opening word and the player has usually moved, and a route from somewhere
 * they are not is not an explanation of anything.
 *
 * NULL when there is no way through from here, which is a real answer on a
 * board that has been played into a corner - the loss screen offers nothing
 * rather than an empty box.
 */
char* routeToGoalFLWP(struct GameComponents* gameComponents, struct DataStructures* data){
	if(gameComponents == NULL || data == NULL || data->I2W == NULL){
		return NULL;
	}

	int numWords = data->I2W->numWords;
	int start = gameComponents->prevInput;
	int goal = gameComponents->goal;

	if(start < 0 || goal < 0 || start >= numWords || goal >= numWords){
		return NULL;
	}

	int* queue = malloc(sizeof(int) * numWords);
	int* cameFrom = malloc(sizeof(int) * numWords);
	if(queue == NULL || cameFrom == NULL){
		free(queue);
		free(cameFrom);
		return NULL;
	}
	for(int i = 0; i < numWords; i++){
		cameFrom[i] = -2;
	}

	int head = 0;
	int tail = 0;
	queue[tail++] = start;
	cameFrom[start] = -1;
	int found = (start == goal);

	while(head < tail && !found){
		int curr = queue[head++];
		struct intList* c = getConnections(curr, data->I2W);
		for(c = c->next; c != NULL; c = c->next){
			int next = c->data;
			if(cameFrom[next] != -2){
				continue;
			}
			/* Spent words are walked around, not through - the same rule the
			   distance uses, and for the same reason. The goal itself is never
			   one of them, so it is reached before this can refuse it. */
			if(next != goal && checkIfUsed_WordSet(next, data->wordSet)){
				continue;
			}
			cameFrom[next] = curr;
			if(next == goal){
				found = 1;
				break;
			}
			queue[tail++] = next;
		}
	}

	if(!found){
		free(queue);
		free(cameFrom);
		return NULL;
	}

	/* Walked back from the goal and then turned round, so the road reads the
	   way it would be travelled. */
	int length = 0;
	for(int at = goal; at != -1; at = cameFrom[at]){
		length++;
	}
	int* forwards = malloc(sizeof(int) * length);
	if(forwards == NULL){
		free(queue);
		free(cameFrom);
		return NULL;
	}
	int at = goal;
	for(int i = length - 1; i >= 0; i--){
		forwards[i] = at;
		at = cameFrom[at];
	}

	struct arrayList* route = init_ArrayList(length, length, NUM);
	for(int i = 0; i < length; i++){
		add_ArrayList(&forwards[i], route, NUM);
	}

	char* result = idArrayListToString(route, data);

	free_ArrayList(route);
	free(forwards);
	free(queue);
	free(cameFrom);
	return result;
}

int hintGetMinAdjacenciesFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures){
	/* nothing to work with */
	if(gameComponents == NULL){
		return -1;
	}

	/* -1 for a board there is no route through, the same answer
	distanceToGoalFLWP gives - and for no board at all.

	The second half of that was missing, and it is the half that crashes: a game
	whose parameters no word satisfies is never built, so the pointer is null
	rather than the solution being null, and reading ->solution off it reads
	address 0x48. The screen asks for this as it opens, before anything has told
	it the board could not be dealt, so the process was gone before the failure
	could be reported. */
	if(gameComponents == NULL || gameComponents->solution == NULL){
		return -1;
	}
	return gameComponents->solution->size - 1; 


}

int hintGetMinAdjacenciesFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return -1;
	}
	/* The composed game holds a pathfinder and a constraint game, and a board
	   that could not be built leaves one or both of them, or the solution
	   inside the pathfinder, as nothing. The same answer the plain
	   pathfinder gives: no route.
	*/
	if(flwgpComponents->flwpComponents == NULL || flwgpComponents->flwpComponents->solution == NULL){
		return -1;
	}

	
	 return flwgpComponents->flwpComponents->solution->size - 1; 
}
char* hintWordTowardsGoalFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return NULL;
	}
	if(flwgpComponents->flwpComponents == NULL || flwgpComponents->flwpComponents->solution == NULL
		|| flwgpComponents->flwpComponents->solution->next == NULL
		|| flwgpComponents->flwpComponents->solution->next->next == NULL){
		return NULL;
	}


	return Convert_IntToWord(flwgpComponents->flwpComponents->solution->next->next->data, data->I2W);
}
char* hintGetValidGoalWordFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwgpComponents == NULL){
		return NULL;
	}
	if(flwgpComponents->flwpComponents == NULL || flwgpComponents->flwpComponents->solution == NULL){
		return NULL;
	}


	struct intList* curr = flwgpComponents->flwpComponents->solution; 
	
	while(curr->next != NULL){

		curr = curr->next; 
	}
	return Convert_IntToWord(curr->data, data->I2W); 

}



