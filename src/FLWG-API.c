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
#include "flwg/includes/FLWGGame.h"
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

// Starts game & returns starting word id 
struct GameData* initiateGame(struct DataStructures* dataStructures){
	// reset word set	
	struct GameData* gameData = malloc(sizeof(struct GameData)); 
	gameData->currWordId = ChooseStart(dataStructures->I2W);  
	gameData->difficulty = 0; 
	gameData->numPlayers = 2; 
	reset_WordSet(dataStructures->wordSet); 
	return gameData; 
	
	
}

int getCurrWord(struct GameData* gameData){
	 return gameData->currWordId; 
}

void endGame(struct GameData* gameData){
	free(gameData); 
}

int botTakesTurn(struct DataStructures* data){
	return 0; 
}
int userTakesTurn(char* userInput, struct DataStructures* data){
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


