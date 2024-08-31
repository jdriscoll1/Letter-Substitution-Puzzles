extern int numLetters; 

#ifndef seenFLWC
#define seenFLWC

#include <fcntl.h>
#include <unistd.h>
#include "../../FLWG-API.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../algs/includes/Minimax-2.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
// Returns the goal word 
int getGoalWord(int word, int distance, struct DataStructures* data){
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
	int fd = open("../../docs/maxn/a.txt", 'r'); 
	// First Generate the data structures
	struct DataStructures* data = initDataStructures(fd); 
	// While the game has not ended
	int w = 0; //ChooseStart(data->I2W); 
	struct score a = createScore(-1, -100, 0, 100); 
	struct score b = createScore(-1, 100, 1, 100); 
	int d = 1; 
	struct score s = minimax2(0, d, d, 1, a, b, data); 
		// Allow user to go 
	freeDataStructures(data); 
	// Allow bot to go 
	// Free the data structures
	close(fd); 
}

#endif
