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
#include "../../flwg/includes/FLWGGame.h"
#include "../../flwc/includes/FLWC.h"

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
struct GameData* initiateGame(struct DataStructures* dataStructures, int numAdjacencies){
	// reset word set	
	struct GameData* gameData = malloc(sizeof(struct GameData)); 
	gameData->currWordId = ChooseStart(dataStructures->I2W, numAdjacencies);
	gameData->difficulty = 0;
	gameData->numPlayers = 2; 
	reset_WordSet(dataStructures->wordSet);
    markUsed_WordSet(gameData->currWordId, dataStructures->wordSet);
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
    if(botType == -1){
        result = botPly_MaxAdjacencies(gameData->currWordId, NULL, data);
    }
    if(botType == 0){
        result = botPly_Random(gameData->currWordId, data);

    }
    if(botType > 0){
        result = botPly(gameData->currWordId, botType, data->I2W, data->wordSet);


    }
    if (result >= 0) {
        gameData->currWordId = result;
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


struct GameComponents* initiateFLWP(int minConnections, struct DataStructures* dataStructures, int numAdjacenciesStartWord){

	return InitializeGameComponents(dataStructures->I2W, minConnections, dataStructures->wordSet, numAdjacenciesStartWord);
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

void freeGameComponentsFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures){
	FreeGameComponents(gameComponents, dataStructures->I2W); 

}

struct GameComponentsFLWGP* initiateFLWPGeneralized(int minDistance, int numAdjacenciesStartWord,  char goalCharacter, int minGoalCharacterDistance, char avoidCharacter, int minAvoidCharacterDistance, struct DataStructures* dataStructures){

	struct GameComponentsFLWC* flwcComponents = initFLWC(numAdjacenciesStartWord, -1, goalCharacter, minGoalCharacterDistance, avoidCharacter, minAvoidCharacterDistance, dataStructures);

	struct GameComponents* flwpComponents = malloc(sizeof(struct GameComponents)); 
	flwpComponents->start = flwcComponents->wordId; 
	flwpComponents->goal = -1;  
	//Sets the minimum number of connection
	flwpComponents->minConnections = minDistance; 
	//Sets the number of moves
	flwpComponents->numMoves = 0;
	//Instantiates the number of undo calls 
	flwpComponents->undoCalls = 0; 
 	//Instantiates the number of hint points
 	flwpComponents->hc = NULL; 
	flwpComponents->prevInput = flwpComponents->start; 
	//Initialize the arrayList 
	flwpComponents->aList = init_ArrayList(dataStructures->I2W->numLetters * (minDistance * 1.5), dataStructures->I2W->numLetters * (minDistance), STR); 
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
	freeGameComponentsFLWP(flwgpComponents->flwpComponents, dataStructures); 
	freeGameComponentsFLWC(flwgpComponents->flwcComponents); 
	free(flwgpComponents); 
} 

struct GameComponentsFLWC* getFLWCComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	return flwgpComponents->flwcComponents; 
}

struct GameComponents* getFLWPComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents){
	return flwgpComponents->flwpComponents; 
}
