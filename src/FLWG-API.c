/*************************
Name: Jordan D. Andrew B.  
Date: Aug 8, 2024
Purpose: A library to encapsulate & organize the code into an API

************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "FLWG-API.h"
#include "flwp/includes/GameFunctions.h"
#include "flwp/includes/UserInput.h"
// Creating and destroying data structures
struct DataStructures* initDataStructures(int fd){
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

int botTakesTurn(struct DataStructures* data){
	return 0; 
}
int userTakesTurn(char* userInput, struct GameData* gameData, struct DataStructures* data){
	
	// Check if the word is valid
	enum ERROR_CODE result; 
	if((result = Check_Input(gameData->currWordId, (const char*)userInput, data->W2I, data->I2W)) != VALID){
		return result; 
	}

	int wordId = convertWordToInt(userInput, data); 
	// Check word is not in word set 
	if(checkIfUsed_WordSet(gameData->currWordId, data->wordSet)){
		return WORD_USED; 
	} 
	// Add word to word set 
	markUsed_WordSet(wordId, data->wordSet); 
        gameData->currWordId = wordId;
	return 0; 
}
int resetWordSet(struct DataStructures* data){
	return 0; 
}
int startGameReturnFirstWord(struct DataStructures *data){

}

// Test Functionality  

char* convertIntToWord(int wordId, struct DataStructures* data){
	return Convert_IntToWord(wordId, data->I2W); 
}

int convertWordToInt(char* word, struct DataStructures* data){
	return Convert_WordToInt(word, data->W2I); 
}


