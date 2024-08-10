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

// Creating and destroying data structures
struct DataStructures* initDataStructures(char* filename){
	struct DataStructures* data = malloc(sizeof(struct DataStructures));  

	// Initialize the Word to Int Hash Map 
	data->W2I = Allocate_WordToInt(); 	
	data->I2W = Allocate_IntToWordStruct(); 	
	Initialize_HashMaps(data->W2I, data->I2W, filename); 
	data->wordSet = init_WordSet(data->I2W->numWords); 
	return data; 
}

void freeDataStructures(struct DataStructures* data){
	free_WordSet(data->wordSet); 
	Free_HashMaps(data->W2I, data->I2W); 
	free(data);
}

 // Mid Game Moves
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


