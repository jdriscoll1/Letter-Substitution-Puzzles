extern int numLetters; 

#ifndef seenFLWC
#define seenFLWC

#include "../../FLWG-API.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
int* getGoalWordSet(int startId, int distance, struct DataStructures* data); 

// BotPly method -- bot needs to determine the best way to go
int botPly_FLWC(int word, int depth, struct DataStructures* data);  
int botPly_Random(int word, struct DataStructures* data);  

// A method that initalizes the game  
void FLWC(struct DataStructures* data); 

#endif
