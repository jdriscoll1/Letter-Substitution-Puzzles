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

// Returns the number of connections a word has, given the TSN in which it is encapsulated
int getNumConnectionsFromTSN(struct TreeStorageNode* node, struct DataStructures* data){
	return data->I2W->array[node->id]->numConnections; 
}


// This generates multiple words for a game of FLWC the users compete to get to 
int* getGoalWordSet(int startId, int distance, struct DataStructures* data){

	// Get list of all words 2 words away from care
	struct BFSResults equidistantWordsResult = BreadthFirstSearch_Distance(startId, distance, data->I2W, data->wordSet); 
	struct arrayList* options = equidistantWordsResult.list; 
	
	
	
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
	int numConnections = data->I2W->array[baseWordId]->numConnections; 
	
	struct arrayList* wordsWithSameNumConnections = init_ArrayList(10, 10, NUM); 
	// Create a list of words that are within delta Connections
	for (int i = 0; i < options->currPrecision; i++){
		struct TreeStorageNode* currNode = ((struct TreeStorageNode**)options->list)[i];
		int wordId = currNode->id; 
		int potentialWordNumConnections = getNumConnectionsFromTSN(currNode, data);
		if(potentialWordNumConnections == numConnections){
			add_ArrayList((void*)&wordId, wordsWithSameNumConnections, NUM); 
		}
	}
	// Now we ought to choose a partner word
	int numWordsWithEqualNumberOfConnectionsAsBaseWord = wordsWithSameNumConnections->currPrecision; 
	if(numWordsWithEqualNumberOfConnectionsAsBaseWord == 0){
		return NULL;  
	}

	int chosenWordId = rand() % numWordsWithEqualNumberOfConnectionsAsBaseWord; 
	int* chosenWords = (int*)calloc(2, sizeof(int)); 
	chosenWords[0] = baseWordId; 
	chosenWords[1] = ((struct TreeStorageNode**)options->list)[chosenWordId]->id;
	free_ArrayList(equidistantWordsResult.list); 
	Free_BFSComponents(equidistantWordsResult.dataStorage, data->wordSet); 
	free_ArrayList(wordsWithSameNumConnections); 
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

	// Choose the word that starts the game	(temporarily care)
	int word = 0; //ChooseStart(data->I2W); 
	
	//  The distance the goal words should be from the start 
	int difficulty = 2; 

	// Obtain the goal words from the start -- this needs to be ran before the words are marked as used
	int* goals = getGoalWordSet(word, difficulty, data); 

	// The start word is now marked as used and cannot be used again
	markUsed_WordSet(word, data->wordSet); 

	
	
	// The winner will start as -2, as a const to say, game not over yet
	int winner = -2; 

	// How far out the algorithm looks when posed with a goal word  -- at present irrelevant
	int depth = 8; 

	// The number of ply's taken before a win 
	int rounds = 0; 

	// Determines whose turn it is currently 
	int whoseTurn = 0; 

	printf("Welcome To The Four Letter Word Challenge!\n"); 
	printf("Your Goal Is To Navigate to %s\n", Convert_IntToWord(goals[0], data->I2W)); 
	printf("Your Opponent's Goal Is To Navigate to %s\n", Convert_IntToWord(goals[1], data->I2W)); 
	printf("Good Luck!\n\n\n");
	printf("Starting Word: %s", Convert_IntToWord(word, data->I2W));


	while (winner == -2){
		if(whoseTurn){
			// Temporarily do not let the bot have a turn  word = botPly_FLWC(word, depth, data); 
			word = userPly(word, data->W2I, data->I2W, data->wordSet); 
		}
		else{
			word = userPly(word, data->W2I, data->I2W, data->wordSet); 
		}

		whoseTurn = (whoseTurn + 1) % 2; 

		// If the word is -1, that means that the user has quit and it's a tie (unfair but temporary) 
		if(word == -1){
			winner = -1; 
		}

		// If the word is equal to player 0's goal word, player 0 wins 
		if (word == goals[0]){
			winner = 0; 
		}

		// If the word is equal to player 1's goal word, player 1 wins 
		if (word == goals[1]){
			winner = 1; 
		}

		printf("Chosen Word: %s\n", Convert_IntToWord(word, data->I2W)); 
		rounds++; 
	}
	switch (winner){
		case -1: 
			break ;
		case 0: 
			break ;
		case 1: 
			break ;
	}
	printf("%c is the winner", (winner == 0)  ? 'A': 'B'); 
	// Allow user to go 
	// Allow bot to go 
	// Free the data structures
}
}

#endif
