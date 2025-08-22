#ifndef seenFLWCTutorial
#define seenFLWCTutorial

#include "FLWG-API.h"
#include "../../structs/includes/ArrayList.h"

struct GameComponentsFLWT{
	// The word that the user needs to connect from 
	int startWordId; 
	
	// The number of adjacencies the user has found 
	int numAdjacenciesFound; 	

	// The words the user has already used
	struct arrayList* prevWords; 
	
	// The minimum number of adjacencies the user needs to find
	int minAdjacenciesUserNeedsToFind; 

	// The maximum number of adjacencies the chosen word needs to have 
	int maxAdjacenciesThatCanBeFound; 
};


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWT* initFLWT(int numTurns, int minAdjacencies, int maxAdjacencies, struct DataStructures* data);

// Is the word oufn in FLWC valid? 
int isStartValidFLWT(struct GameComponentsFLWT* flwtComponents);

// Choose a word
char* getStartWordFLWT(struct GameComponentsFLWT* flwtComponents, struct DataStructures *data); 

// Allow the user to insert a word 
int userEntersWordFLWT(char* userInput, struct GameComponentsFLWT* flwtComponents,  struct DataStructures* data); 


struct arrayList* getCurrentWordsFLWT(struct GameComponentsFLWT* flwtComponents); 

int isGameWonFLWT(struct GameComponentsFLWT* flwtComponents); 

void freeGameComponentsFLWT(struct GameComponentsFLWT* flwtComponents);

#endif
