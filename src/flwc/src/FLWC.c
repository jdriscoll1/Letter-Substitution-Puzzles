extern int numLetters; 

#ifndef seenFLWC
#define seenFLWC

#include "../../FLWG-API.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
// Returns the goal word 
int getGoalWord(int word, int distance, struct DataStructrues* data){
	// Call BFS Function that returns a list of word
	// Choose from that list 
	return 0; 
}

// BotPly method -- bot needs to determine the best way to go
// Returns the word the bot has chosen
int botPly_FLWC(int word, int depth, struct DataStructures* data){
	// First assign every word that is not used a score by running BFS that goes out to everyword
	// Then run minimax using that score 
	return 0; 
} 

// A method that initalizes the game  
void FLWC_Test(){
	// First Generate the data structures
	// While the game has not ended
		// Allow user to go 
		// Allow bot to go 
	// Free the data structures

}

#endif
