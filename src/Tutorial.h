#ifndef seenFLWCTutorial
#define seenFLWCTutorial

#include "FLWG-API.h"
#include "./structs/includes/ArrayList.h"

struct GameComponentsFLWT{
	// The word that the user needs to connect from 
	int startWord; 
	
	int numAdjacenciesFound; 	

	// The words the user has already used
	struct arrayList* prevWords; 
	
	// The minimum number of adjacencies the user needs enter
	int minAdjacencies; 

};


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWT* initFLWT(int minNumConnections, int maxNumConnections, int minAdjacencies, struct DataStructures* data);

// Choose a word
int getStartWordFLWT(int d1, int d2, struct DataStructures *data); 

// Allow the user to insert a word 
int userEntersWordFLWT(char* userInput, struct GameComponentsFLWT* flwtComponents,  struct DataStructures* data); 


struct arrayList* getCurrentWordsFLWT(struct GameComponentsFLWT* flwtComponents); 

int isGameWonFLWT(struct GameComponentsFLWT* flwtComponents); 

#endif
