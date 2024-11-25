#ifndef seenFLWC
#define seenFLWC

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

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
int* getGoalWordSet(int distance, struct DataStructures* data){

	
	// While it is unable to find a word whose number of connections is shared by another word equidsitant from the start word 
	int startId = -1;  
	int baseWordId = -1; 

	//  This stores all of the words who are equidistant from the chosen start word
	struct BFSResults equidistantWordsResult; 

	// This list stores the words that have the same number of connections as player 1's words
	struct arrayList* wordsWithSameNumConnectionsAsBaseWord = NULL; 

	// This is the number of words that share the same number of connections as the base word chosen later
	int numWordsWithEqualNumberOfConnectionsAsBaseWord  = -1; 
	while (true){

		startId = ChooseStart(data->I2W); 

		// Get list of all words d words away from care
		equidistantWordsResult = BreadthFirstSearch_Distance(startId, distance, data->I2W, data->wordSet); 
		struct arrayList* options = equidistantWordsResult.list; 
			
		/*for(int o = 0; o < options->currPrecision; o++){
			int curr_id = ((struct TreeStorageNode**)options->list)[o]->id;
			char* curr_str = Convert_IntToWord(curr_id, data->I2W); 
			int curr_conns = data->I2W->array[curr_id]->numConnections; 

			printf("%d: %s [%d]\n", o, curr_str, curr_conns); 
		}*/


		// if there aren't any words n away
		if(options->currPrecision == 0){

			free_ArrayList(equidistantWordsResult.list); 

			Free_BFSComponents(equidistantWordsResult.dataStorage, data->wordSet); 

			continue; 	
		}

		// First choose a random word out of the words n distance away
		int baseWordIndex = rand() % options->currPrecision; 
		// Get the word's node
		baseWordId = ((struct TreeStorageNode**)options->list)[baseWordIndex]->id;
		// Get the number of connections the base word has 
		int baseWordNumConnections = data->I2W->array[baseWordId]->numConnections; 

		
		wordsWithSameNumConnectionsAsBaseWord = init_ArrayList(10, 10, NUM); 
		// Loop through all of the options determining which words have the same number of connections as the base word 
		for (int i = 0; i < options->currPrecision; i++){
			struct TreeStorageNode* currNode = ((struct TreeStorageNode**)options->list)[i];
			int wordId = currNode->id; 
			if(wordId == baseWordId){
				continue; 
			}
			int potentialWordNumConnections = getNumConnectionsFromTSN(currNode, data);
			if(potentialWordNumConnections == baseWordNumConnections){
				add_ArrayList((void*)&wordId, wordsWithSameNumConnectionsAsBaseWord, NUM); 
			}
		}
		
		// Now we ought to choose a partner word
		numWordsWithEqualNumberOfConnectionsAsBaseWord = wordsWithSameNumConnectionsAsBaseWord->currPrecision; 

		if(numWordsWithEqualNumberOfConnectionsAsBaseWord > 0){
			break ;
		}

		free_ArrayList(equidistantWordsResult.list); 

		Free_BFSComponents(equidistantWordsResult.dataStorage, data->wordSet); 

		free_ArrayList(wordsWithSameNumConnectionsAsBaseWord); 


	}

	int chosenWordId = rand() % numWordsWithEqualNumberOfConnectionsAsBaseWord; 

	int* chosenWords = (int*)calloc(3, sizeof(int)); 

	// The first word in the array is the starting word
	chosenWords[0] = startId; 
	
	// The first word is the word for the first player 
	chosenWords[1] = baseWordId; 

	// The second word is the word for the second player 
	chosenWords[2] = ((int*)(wordsWithSameNumConnectionsAsBaseWord)->list)[chosenWordId]; 

	// This the array list of words who are equidistant from the chosen word
	free_ArrayList(equidistantWordsResult.list); 

	Free_BFSComponents(equidistantWordsResult.dataStorage, data->wordSet); 

	free_ArrayList(wordsWithSameNumConnectionsAsBaseWord); 

	return chosenWords; 
	
}

// BotPly method -- bot needs to determine the best way to go
// Returns the word the bot has chosen
int botPly_FLWC(int word, int goalId, int depth, struct DataStructures* data){

	// Create alpha & beta
	struct score a = createScore(-1, -100, 0, 100); 
	struct score b = createScore(-1, 100, 1, 100); 
	struct score score = minimax2(word, goalId, depth, depth, 1, a, b, data, flwc_score);

	if(score.wordId != -1){
		markUsed_WordSet(score.wordId, data->wordSet); 
	}

	// Then run minimax using that score 
	return score.wordId; 
} 

int botPly_Random(int word, struct DataStructures* data){

	// Create alpha & beta
	int resultId = choose_random_word(word, data); 	

	if(resultId != -1){
		markUsed_WordSet(resultId, data->wordSet); 
	}
	// Then run minimax using that score 
	return resultId; 
} 


int FLWC(struct DataStructures* data){

	
	//  The distance the goal words should be from the start 
	int minDistance = 4; 

	// Obtain the goal words from the start -- this needs to be ran before the words are marked as used
	int* goals = getGoalWordSet(minDistance, data); 

	// Choose the word that starts the game	(temporarily care)
	int word = goals[0]; //ChooseStart(data->I2W); 

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
	printf("Your Goal: %s\n", Convert_IntToWord(goals[1], data->I2W)); 
	//printf("FLWC's Goal: %s\n\n\n", Convert_IntToWord(goals[2], data->I2W)); 
	printf("Good Luck!\n\n\n");
	printf("Starting Word: %s\n", Convert_IntToWord(word, data->I2W));



	// if winner is -1 it is a tie so therefore -2 is gg
	while (winner == -2){
		if(whoseTurn){
			// Temporarily do not let the bot have a turn  word = botPly_FLWC(word, depth, data); 
			word = userPly(word, data->W2I, data->I2W, data->wordSet); 
			//word = botPly_Random(word, data); 
			//printf("Random Chooses: %s\n", Convert_IntToWord(word, data->I2W)); 
		}
		else{
			word = botPly_FLWC(word, goals[2], depth, data); 
			//word = botPly_Random(word, data); 
			printf("FLWC Chooses: %s\n", Convert_IntToWord(word, data->I2W)); 
		}

		whoseTurn = (whoseTurn + 1) % 2; 

		// If the word is -1, that means that the user has quit and it's a tie (unfair but temporary) 
		if(word == -1){
			winner = -1; 
		}

		// If the word is equal to player 0's goal word, player 0 wins 
		if (word == goals[1]){
			winner = 0; 
		}

		// If the word is equal to player 1's goal word, player 1 wins 
		if (word == goals[2]){
			winner = 1; 
		}

		rounds++; 
	}
	switch (winner){
		case -1: 
			printf("Tie\n"); 
			break ;
		case 0: 
			printf("You Win\n"); 
			break ;
		case 1: 
			printf("Bot Wins\n"); 
			break ;
	}
	
	free(goals); 

	reset_WordSet(data->wordSet); 

	return winner; 
}

void FLWC_Test(struct DataStructures* data){
	int numGames = 800; 
	int currGame = 0; 
	int botAWins = 0; 
	int botBWins = 0; 
	int ties = 0; 
	// Run 100 tests and keep track of whose won 
	for (int currGame = 0; currGame < numGames; currGame++ ){
		//printf(" Game #%d\n", currGame);
		int winner = FLWC(data);
		// ifit retursn 0, then
		switch (winner){
		case -1: 
			ties++; 
			break ;
		case 0: 
			botAWins++;
			break ;
		case 1: 
			botBWins++; 
			break ;
		}
	}

	printf("\nA Wins: %d\n", botAWins); 
	printf("B Wins: %d\n", botBWins); 
	printf("Ties: %d\n", ties); 
}
#endif
