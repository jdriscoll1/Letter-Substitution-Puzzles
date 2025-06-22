#ifndef seenFLWCAPI
#define seenFLWCAPI

#include "FLWG-API.h"
#include "../../structs/includes/ArrayList.h"

struct GameComponentsFLWC{
	// The word that the user needs to connect from 
	int wordId; 
	// The set of words the user wants to get to 
	struct WordSet* goalWords; 
	// The set of words the user wants to avoid
	struct WordSet* avoidWords; 
	// The solution of the game 
	char* solution; 
};


// This creates the first word and returns a set of game components for the flwt

struct GameComponentsFLWC* initFLWC(int numAdjacenciesToStartWord, char goalCharacter, int minGoalCharacterDistance, char avoidCharacter, int minAvoidCharacterDistance,  struct DataStructures *data);

// Choose a word
char* getStartWordFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures *data); 

// Allow the user to insert a word 
int userEntersWordFLWC(char* userInput, struct GameComponentsFLWC* flwcComponents,  struct DataStructures* data); 

int botTakesTurnFLWC(int botType, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data);

int isGameWonFLWC(struct GameComponentsFLWC* flwcComponents); 

void freeGameComponentsFLWC(struct GameComponentsFLWC* flwcComponents); 

char* getSolutionFLWC(struct GameComponentsFLWC* flwcComponents);

// Confirms if the parameters inputted are valid
int isValidStartFLWC(struct GameComponentsFLWC* flwcComponents);

#endif
