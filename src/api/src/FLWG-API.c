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
	int wordId = gameData->currWordId; 
	char* word = convertIntToWord(wordId, dataStructures);
	return word; 
}

void freeGameComponentsFLWG(struct GameData* gameData){
	free(gameData); 
}

int botTakesTurn(struct GameData* gameData, struct DataStructures* data, int botType){
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

	ResetGameComponents(gameComponents, dataStructures->I2W);
}

int getPrevWordFLWP(struct GameComponents* gameComponents){
    return gameComponents->prevInput;

}
int userEntersWord_FLWP(char* userInput, struct GameComponents *gameComponents, struct DataStructures* dataStructures){
    return AddWord_Struct(gameComponents, userInput, dataStructures); 
}
char* convertIntToWord(int wordId, struct DataStructures* data){
	return Convert_IntToWord(wordId, data->I2W); 
}

int convertWordToInt(char* word, struct DataStructures* data){
	return Convert_WordToInt(word, data); 
}

char* getStartWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
    return convertIntToWord(gameComponents->start, dataStructures);
}
char* getGoalWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
    return convertIntToWord(gameComponents->goal, dataStructures);
}

void undoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
    Undo_Struct(gameComponents, dataStructures->I2W);
}


void redoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
    Redo_Struct(gameComponents, dataStructures->I2W);
}

struct arrayList *getCurrentWordsFLWP(struct GameComponents *gameComponents) {
    return gameComponents->aList;
}

int isGameWonFLWP(struct GameComponents *gameComponents) {
    return gameComponents->goal == gameComponents->prevInput;
}

void removeWord_FLWP(char* word, struct GameComponents *gameComponents, struct DataStructures* dataStructures) {

    char tempStr[dataStructures->I2W->numLetters + 2];
    tempStr[0] = '-';
    tempStr[dataStructures->I2W->numLetters + 1] = '\0';

    strncpy(&tempStr[1], word, dataStructures->I2W->numLetters);

    RemoveWord_Struct(gameComponents, tempStr, 0, dataStructures); 
}

int isStartValid_FLWP(struct GameComponents* gameComponents){
	return gameComponents->start != -1; 	

}
void freeGameComponentsFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures){
	FreeGameComponents(gameComponents, dataStructures->I2W); 

}


struct GameComponentsFLWGP* initiateFLWGP(int minAdjacenciesToStart,  int maxAdjacenciesToStart, char** goalWords, char** avoidWords, int minGoalDistance, int minAvoidDistance, int maxGoalDistance, int maxAvoidDistance, int minGoalAdjacencies, int maxGoalAdjacencies, struct DataStructures* dataStructures){

	struct GameComponentsFLWC* flwcComponents = initFLWC(minAdjacenciesToStart, maxAdjacenciesToStart, goalWords, avoidWords, minGoalDistance, minAvoidDistance, maxGoalDistance, maxAvoidDistance, minGoalAdjacencies, maxGoalAdjacencies, dataStructures);
	if(flwcComponents->wordId == -1){

		struct GameComponents* flwpComponents = NULL; 
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
	
	struct GameComponentsFLWGP* flwgpComponents = malloc(sizeof(struct GameComponentsFLWGP)); 
	flwgpComponents->flwcComponents = flwcComponents; 
	flwgpComponents->flwpComponents = flwpComponents; 
	return flwgpComponents; 

}

void freeGameComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures){
	if(flwgpComponents->flwpComponents != NULL){
		Free_IntLL(flwgpComponents->flwpComponents->userConnections);
		Free_GenericLinkedList(flwgpComponents->flwpComponents->storageHeader);
		free_ArrayList(flwgpComponents->flwpComponents->aList);
		free(flwgpComponents->flwpComponents);

	}
	freeGameComponentsFLWC(flwgpComponents->flwcComponents); 
	free(flwgpComponents); 
} 

struct GameComponentsFLWC* getFLWCComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	return flwgpComponents->flwcComponents; 
}

struct GameComponents* getFLWPComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	return flwgpComponents->flwpComponents; 
}

int userEntersWord_FLWGP(char* userInput, struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures){

	int result = userEntersWord_FLWP(userInput, flwgpComponents->flwpComponents, dataStructures); 
	if(result == 0){
		markUnused_WordSet(Convert_WordToInt(userInput, dataStructures), dataStructures->wordSet); 
		userEntersWordFLWC(userInput, flwgpComponents->flwcComponents, dataStructures); 
		markUnused_WordSet(Convert_WordToInt(userInput, dataStructures), dataStructures->wordSet); 
	}
	return result; 

}


void undoMoveFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures) {
	Undo_Struct(flwgpComponents->flwpComponents, dataStructures->I2W);
	flwgpComponents->flwcComponents->wordId = getPrevWordFLWP(flwgpComponents->flwpComponents); 

}
char* getSolutionFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	return flwgpComponents->flwcComponents->solution; 
}

void redoMoveFLWGP(struct GameComponentsFLWGP *flwgpComponents, struct DataStructures* dataStructures) {
	Redo_Struct(flwgpComponents->flwpComponents, dataStructures->I2W);
	flwgpComponents->flwcComponents->wordId = getPrevWordFLWP(flwgpComponents->flwpComponents); 
}
int isStartValid_FLWGP(struct GameComponentsFLWGP *flwgpComponents){
	return  isStartValidFLWC(flwgpComponents->flwcComponents);
}

int isStartValidFLWG(struct GameData* gameData){

	return gameData->currWordId != -1; 

}


char hintletterToConsiderFLWG(struct GameData* flwgComponents, struct DataStructures* data){
	return letterToConsiderHint(flwgComponents->currWordId, data); 
}

char hintNumOptionsFLWG(struct GameData* flwgComponents, struct DataStructures* data){
	return numOptionsHint(flwgComponents->currWordId, data); 
}

