#ifndef seenFLWC
#define seenFLWC

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <algorithm>

extern "C" {
extern int numLetters; 
#include "../../FLWG-API.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../algs/includes/Minimax-2.h"
#include "../../flwg/includes/FLWGGame.h"
#include "../../structs/includes/ArrayList.h"
#include "../../flwc/includes/FLWC.h"
// Choose the goal word
// struct map* IntToWord_HashMap
// int distance
// int startWord
// Returns the goal word 
int getNumConnectionsFromTSN(struct TreeStorageNode* node, struct DataStructures* data){
	return data->I2W->array[node->id]->numConnections; 
}

//int compareByNumConnectionsFunction(void* node0, void* node1, void* data){
//
//	// Convert the Integer to a Word for confirmation
//	return getNumConnectionsFromTSN((struct TreeStorageNode*)node0, (struct DataStructures*) data) - getNumConnectionsFromTSN((struct TreeStorageNode*)node1, (struct DataStructures*)data);
//
//
//}

int* getGoalWordSet(int startId, int distance, struct DataStructures* data){

	// Get list of all words 2 words away from care
	struct arrayList* options = BreadthFirstSearch_Distance(0, 2, data->I2W, data->wordSet); 
	
	
	int i = 0; 

	// # of choices 
	int n =  options->currPrecision; 	

	// to be a valid pairing, the number of connections a word and its neighbor must differ by, shall be no more than deltaConnections
	int maxDeltaConnections = 3; 
	
	// list of neighbors within deltaConnections
	struct arrayList* validPairings = init_ArrayList(10, 10, NUM); 

        auto compareByNumConnectionsFunction = [&data](struct TreeStorageNode* node0, struct TreeStorageNode* node1) {
	  // Convert the Integer to a Word for confirmation
	  return getNumConnectionsFromTSN(node0, data) < getNumConnectionsFromTSN(node1, data);
        };

	// Sort the list of words by number of connections
	std::sort(
		// The first value in the list 
		(struct TreeStorageNode**)options->list,
		// the length of the list 
		((struct TreeStorageNode**)options->list) + options->currPrecision, 
		// the comparator function
		compareByNumConnectionsFunction
	); 	

	// if number of valid groups of words is 0, return error
	while(i < n-1){

		// Choose the next node in the list 
		struct TreeStorageNode* node0 = ((struct TreeStorageNode**) options->list)[i];
		struct TreeStorageNode* node1 = ((struct TreeStorageNode**) options->list)[i+1];
		
		int wordId0 = node0->id;  
		int wordId1 = node1->id; 

		// Convert the Integer to a Word for confirmation
		// number of connections on first word
		int numConnections0 = data->I2W->array[wordId0]->numConnections; 
		int numConnections1 = data->I2W->array[wordId1]->numConnections; 
		
		printf("Word: %s, NC: %d\n", Convert_IntToWord(wordId0 , data->I2W), numConnections0); 
		printf("Word: %s, NC: %d\n", Convert_IntToWord(wordId1 , data->I2W), numConnections1); 
		int deltaConnections = numConnections1 - numConnections0; 
		printf("DeltaConnections: %d\n", deltaConnections); 

		// number of connections on second word

		i++; 
	}

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
}

#endif
