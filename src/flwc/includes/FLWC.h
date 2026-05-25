#ifndef seenFLWC
#define seenFLWC

#include "../../api/includes/FLWG-API.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
int* getGoalWordSet(int distance, struct DataStructures* data, int numAdjacenciesToStartWord);

// BotPly method -- bot needs to determine the best way to go
int botPly_Mirror(int word); 
int botPly_FLWC(int word, int depth, struct WordSet* goalWords, struct WordSet* avoidWords, struct DataStructures* data);
int botPly_Random(int word, struct DataStructures* data);
int botPly_MaxAdjacencies(int word, struct WordSet* goalWords, struct DataStructures* data);

// A method that initalizes the game  
int FLWC(struct DataStructures* data); 
void FLWC_Test(struct DataStructures* data); 

void generalizedFLWCGame(struct DataStructures* data); 

// Check if a an input is initially winnable
int is_game_winnable_FLWC(
	// the current id being searched	
	int id, 
	int depth, 
	// is it currently the player or bot's perspective	
	int isPlayerPerspective, 
	// the set of goal words	
	struct WordSet* goalWords, 
	// the set of avoid words
	struct WordSet* avoidWords, 
	struct DataStructures* data, 
	// alpha --> Initialized to -100
	int alpha, 
	// beta  --> Initialized to +100
	int beta); 


#endif
