extern int numLetters; 

#ifndef seenFLWC
#define seenFLWC

#include <fcntl.h>
#include <unistd.h>
#include "../../FLWG-API.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../algs/includes/Minimax-2.h"
#include "../../flwg/includes/FLWGGame.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
// Returns the goal word 
int* getGoalWordList(int word, int distance, struct DataStructures* data){

	// find a list of all words that are equidistant from the start given start word and distance
	//int* equidistantWords = getAllEquidistantWords(int word, int distance, struct DataStructures*); 

	// sort words by number of neighbors, then determine which words have a close enough number of neighbros to be valid
	// if number of valid groups of words is 0, return error

	// if number of valid groups of words is > 0, choose random, the harder the difficulty the less neighbors (ie, further along list)
	//int* validIndices = getAllAdjacencyGro(); 
	

	return NULL;  
}

// BotPly method -- bot needs to determine the best way to go
// Returns the word the bot has chosen
int botPly_FLWC(int word, int depth, struct DataStructures* data){
	// Create alpha & beta
	struct score a = createScore(-1, -100, 0, 100); 
	struct score b = createScore(-1, 100, 1, 100); 
	struct score score = minimax2(word, depth, depth, 1, a, b, data);

	if(score.wordId != -1){
        		markUsed_WordSet(score.wordId, data->wordSet); 
	}
	// Then run minimax using that score 
	return score.wordId; 
} 

int botPly_Random(int word, struct DataStructures* data){
	// Create alpha & beta
	struct score a = createScore(-1, -100, 0, 100); 
	struct score b = createScore(-1, 100, 1, 100); 
	struct score score = minimax2(word, 1, 1, 1, a, b, data);

	if(score.wordId != -1){
        		markUsed_WordSet(score.wordId, data->wordSet); 
	}
	// Then run minimax using that score 
	return score.wordId; 
} 


// A method that initalizes the game  
void FLWC(struct DataStructures* data){
	// Choose the word that starts the game	
	int word = 0;//ChooseStart(data->I2W); 
	printf("Starting Word: aaaa"); 
	markUsed_WordSet(word, data->wordSet); 
	int winner = -1; 
	int depth = 8; 
	int rounds = 0; 
	int whoseTurn = 0; 
	while (winner == -1){
		if(whoseTurn){
			word = botPly_FLWC(word, depth, data); 
		}
		else{
			word = userPly(word, data->W2I, data->I2W, data->wordSet); 
		}
		whoseTurn = (whoseTurn + 1) % 2; 
		if(word == -1){
			winner = whoseTurn; 
		}
		printf("Chosen Word: %s\n", Convert_IntToWord(word, data->I2W)); 
		rounds++; 
	}
	if (winner == 0){
		printf("New Minimax Wins");
	}	
	
	else {
		printf("Old Minimax Wins");
	}
	printf("%c is the winner", (winner == 0)  ? 'A': 'B'); 
	// Allow user to go 
	// Allow bot to go 
	// Free the data structures
}

#endif
