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
#endif
