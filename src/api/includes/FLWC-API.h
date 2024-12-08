#ifndef seenFLWCTutorial
#define seenFLWCTutorial

#include "FLWG-API.h"
#include "../../structs/includes/ArrayList.h"

struct GameComponentsFLWC{
	// The word that the user needs to connect from 
	int startWordId; 
	
};


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWC* initFLWC(int challengeId, int distance, struct DataStructures* data);

// Choose a word
char* getStartWordFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures *data); 

// Allow the user to insert a word 
int userEntersWordFLWC(char* userInput, struct GameComponentsFLWC* flwcComponents,  struct DataStructures* data); 

int isGameWonFLWC(struct GameComponentsFLWC* flwcComponents); 

#endif
