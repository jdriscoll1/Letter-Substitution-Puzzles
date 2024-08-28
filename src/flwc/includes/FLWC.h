extern int numLetters; 

#ifndef seenFLWC
#define seenFLWC

#include "../../FLWG-API.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
int getGoalWord(int word, int distance, struct DataStructrues* data); 

// BotPly method -- bot needs to determine the best way to go
int botPly_FLWC(int word, int depth, struct DataStructures* data);  

// A method that initalizes the game  
void FLWC(); 

#endif
