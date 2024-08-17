/*************************
Name: Jordan D. Andrew B.  
Date: Aug 8, 2024
Purpose: A library to encapsulate & organize the code into an API

************************/
extern int numLetters; 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "./structs/includes/HashMap.h"
#include "./structs/includes/TreeSet.h"
#include "./structs/includes/WordSet.h"

#ifndef seenFLWGAPI
#define seenFLWGAPI


// Some things that need to be done
// Convert Word To Int 
struct DataStructures {
	// The Word to Int Map 
	struct DummyHeadNode*** W2I; 
	// The Int to Word Map
	struct wordDataArray* I2W; 
	// The Word Set
	struct WordSet* wordSet; 
	// Current word
	int currWordId; 
}; 

struct GameData {
	int currWordId; 
	int numPlayers; 
	int difficulty; 
};

// Creating and destroying data structures
struct DataStructures* initDataStructures(int fd); 

void freeDataStructures(struct DataStructures* dataStructures); 

// Game functions 
struct GameData* initiateGame(struct DataStructures* dataStructures); 
char* getCurrWord(struct GameData* gameData, struct DataStructures* dataStructures); 
void endGame(struct GameData* gameData); 
int botTakesTurn(struct GameData* gameData, struct DataStructures* data);
int userTakesTurn(char* userInput, struct GameData* gameData, struct DataStructures* dataStructures); 
int resetWordSet(struct DataStructures* dataStructures); 
int startGameReturnFirstWord(struct DataStructures *dataStructures);

// Test Functionality  

char* convertIntToWord(int wordId, struct DataStructures* dataStructures); 

int convertWordToInt(char* word, struct DataStructures* dataStructures); 

#endif 
