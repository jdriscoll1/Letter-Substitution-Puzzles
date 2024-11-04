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


int* getGoalWordSet(int startId, int distance, struct DataStructures* data){

	
	// Get list of all words 2 words away from care
	struct arrayList* options = BreadthFirstSearch_Distance(0, 2, data->I2W, data->wordSet); 
	
	
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


	// First choose a random word
	int baseWordIndex = rand() % options->currPrecision; 
	struct TreeStorageNode* baseWordNode = ((struct TreeStorageNode**)options->list)[baseWordIndex];
	int baseWordId = baseWordNode->id; 
	printf("Reference Word: %s", Convert_IntToWord(baseWordId, data->I2W));
	int numConnections = data->I2W->array[baseWordId]->numConnections; 
	
	struct arrayList* wordsWithSameNumConnections = init_ArrayList(10, 10, NUM); 
	// Create a list of words that are within delta Connections
	for (int i = 0; i < options->currPrecision; i++){
		struct TreeStorageNode* currNode = ((struct TreeStorageNode**)options->list)[i];
		int wordId = currNode->id; 
		printf("Checking %s: ", Convert_IntToWord(wordId, data->I2W)); 	
		int potentialWordNumConnections = getNumConnectionsFromTSN(currNode, data);
		if(potentialWordNumConnections == numConnections){
			printf("Added!\n"); 
			add_ArrayList((void*)&wordId, wordsWithSameNumConnections, NUM); 
		}
		else{
			printf("Invalid\n");
		}
	}
	print_ArrayList(wordsWithSameNumConnections, NUM); 
	// Now we ought to choose a partner word
	int numWordsWithEqualNumberOfConnectionsAsBaseWord = wordsWithSameNumConnections->currPrecision; 
	if(numWordsWithEqualNumberOfConnectionsAsBaseWord == 0){
		return NULL;  
	}

	int chosenWordId = rand() % numWordsWithEqualNumberOfConnectionsAsBaseWord; 
	int* chosenWords = (int*)calloc(2, sizeof(int)); 
	chosenWords[0] = baseWordId; 
	chosenWords[1] = ((struct TreeStorageNode**)options->list)[chosenWordId]->id;
	return chosenWords; 

	
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
