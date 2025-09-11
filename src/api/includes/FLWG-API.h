/*************************
Name: Jordan D. Andrew B.
Date: Aug 8, 2024
Purpose: A library to encapsulate & organize the code into an API

************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/TreeSet.h"
#include "../../structs/includes/WordSet.h"

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
}; 

struct GameData {
	int currWordId; 
	int numPlayers; 
	int difficulty; 
	int goalWord; 
};

struct GameComponentsFLWGP{
	struct GameComponents* flwpComponents; 
	struct GameComponentsFLWC* flwcComponents; 

}; 

// Shared behavior
struct DataStructures* initDataStructures(int fd, int numLetters); 

// flwg behavior
void freeDataStructures(struct DataStructures* dataStructures); 
struct GameData* initFLWG(struct DataStructures* dataStructures, int minAdjacencies, int maxAdjacencies);
char* getCurrWord(struct GameData* gameData, struct DataStructures* dataStructures); 
void endGame(struct GameData* gameData); 
int botTakesTurn(struct GameData* gameData, struct DataStructures* data, int botType);
int userTakesTurn(char* userInput, struct GameData* gameData, struct DataStructures* dataStructures); 
void resetWordSet(struct DataStructures* dataStructures);
int startGameReturnFirstWord(struct DataStructures *dataStructures);
void freeGameComponentsFLWG(struct GameData* gameData);
int isStartValidFLWG(struct GameData* gameData);

// flwp behavior
struct GameComponents* initiateFLWP(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data);
void ResetFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures); 
int userEntersWord_FLWP(char* userInput, struct GameComponents *gameComponents, struct DataStructures* dataStructures); 
void removeWord_FLWP(char* word, struct GameComponents *gameComponents, struct DataStructures* dataStructures);
int isStartValid_FLWP(struct GameComponents *gameComponents); 
char* getStartWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
char* getGoalWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
int getPrevWordFLWP(struct GameComponents* gameComponents);
int isGameWonFLWP(struct GameComponents *gameComponents);
void undoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
void redoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
struct arrayList *getCurrentWordsFLWP(struct GameComponents *gameComponents);
void freeGameComponentsFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures);
char* hintGetHeadAdjacencyFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures); 
char* hintGetTailAdjacencyFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures); 
int hintGetMinAdjacenciesFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures); 

// flwp-generalized additional functionality 
struct GameComponentsFLWGP* initiateFLWGP(int minAdjacenciesToStart,  int maxAdjacenciesToStart, char** goalWords, char** avoidWords, int minGoalDistance, int minAvoidDistance, int maxGoalDistance, int maxAvoidDistance, int minGoalAdjacencies, int maxGoalAdjacencies, struct DataStructures* dataStructures);
struct GameComponents* getFLWPComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents);
struct GameComponentsFLWC* getFLWCComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents); 
void freeGameComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures); 
int userEntersWord_FLWGP(char* userInput, struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures); 
void undoMoveFLWGP(struct GameComponentsFLWGP *flwgpComponents, struct DataStructures* dataStructures);
void redoMoveFLWGP(struct GameComponentsFLWGP *flwgpComponents, struct DataStructures* dataStructures);
int isStartValid_FLWGP(struct GameComponentsFLWGP* flwgpComponents); 
int hintGetMinAdjacenciesFLWGP(struct GameComponentsFLWGP* flwgpComponents); 
char* hintWordTowardsGoalFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 
char* hintGetValidGoalWordFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 

char hintLetterToConsiderFLWG(struct GameData* flwgComponents, struct DataStructures* data);
int hintNumOptionsFLWG(struct GameData* flwgComponents, struct DataStructures* data);

// Test Functionality  
char* convertIntToWord(int wordId, struct DataStructures* dataStructures); 
int convertWordToInt(char* word, struct DataStructures* dataStructures); 



#endif 
