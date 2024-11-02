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

#include "FLWG-API.h"
#include "flwp/includes/GameFunctions.h"
#include "flwp/includes/UserInput.h"
#include "flwg/includes/FLWGGame.h"
// Creating and destroying data structures
struct DataStructures* initDataStructures(int fd){

    srand(time(NULL));

	struct DataStructures* data = malloc(sizeof(struct DataStructures));

	// Initialize the Word to Int Hash Map 
	data->W2I = Allocate_WordToInt(); 	
	data->I2W = Allocate_IntToWordStruct(); 	
	Initialize_HashMaps_fd(data->W2I, data->I2W, fd); 
	data->wordSet = init_WordSet(data->I2W->numWords); 
	return data; 
}

void freeDataStructures(struct DataStructures* data){
	free_WordSet(data->wordSet); 
	Free_HashMaps(data->W2I, data->I2W); 
	free(data);
}

 // FLWG functions
struct GameData* initiateGame(struct DataStructures* dataStructures){
	// reset word set	
	struct GameData* gameData = malloc(sizeof(struct GameData)); 
	gameData->currWordId = ChooseStart(dataStructures->I2W);
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

void endGame(struct GameData* gameData){
	free(gameData); 
}

int botTakesTurn(struct GameData* gameData, struct DataStructures* data){
	int result = botPly(gameData->currWordId, 1, data->I2W, data->wordSet);
    if (result >= 0) {
        gameData->currWordId = result;
    }
    return result;
}

int userTakesTurn(char* userInput, struct GameData* gameData, struct DataStructures* data){
	
	// Check if the word is valid
	enum ERROR_CODE result; 
	if((result = Check_Input(gameData->currWordId, (const char*)userInput, data->W2I, data->I2W)) != VALID){
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


struct GameComponents* initiateFLWP(int minConnections, struct DataStructures* dataStructures){

	return InitializeGameComponents(dataStructures->I2W, minConnections, dataStructures->wordSet); 
}

void ResetFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures){

	ResetGameComponents(gameComponents, dataStructures->I2W);
}
int userEntersWord_FLWP(char* userInput, struct GameComponents *gameComponents, struct DataStructures* dataStructures){
    return AddWord_Struct(gameComponents, userInput, dataStructures->W2I, dataStructures->I2W);
}


// Test Functionality  

char* convertIntToWord(int wordId, struct DataStructures* data){
	return Convert_IntToWord(wordId, data->I2W); 
}

int convertWordToInt(char* word, struct DataStructures* data){
	return Convert_WordToInt(word, data->W2I); 
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

int isGameWonFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures) {
    return gameComponents->goal == gameComponents->prevInput;
}

void removeWord_FLWP(char* word, struct GameComponents *gameComponents, struct DataStructures* dataStructures) {

    char tempStr[numLetters + 2];
    tempStr[0] = '-';
    tempStr[numLetters + 1] = '\0';

    strncpy(&tempStr[1], word, numLetters);

    RemoveWord_Struct(gameComponents, tempStr, 0, dataStructures->W2I, dataStructures->I2W);

}