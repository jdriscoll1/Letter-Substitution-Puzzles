#ifndef seenFLWC
#define seenFLWC

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "../../api/includes/FLWG-API.h"
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
int* getGoalWordSet(int distance, struct DataStructures* data, int numAdjacenciesToStartWord){

	
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

		startId = ChooseStart(data->I2W, numAdjacenciesToStartWord);

		// Get list of all words d words away from care
		equidistantWordsResult = BreadthFirstSearch_Distance(startId, distance, data->I2W, data->wordSet); 
		struct arrayList* options = equidistantWordsResult.list; 
			
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
		for (unsigned int i = 0; i < options->currPrecision; i++){
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
int botPly_FLWC(int word, int depth, struct WordSet* goalWords, struct WordSet* avoidWords, struct DataStructures* data){

	// Create alpha & beta
	struct score a = createScore(-1, -100, 0, 100); 
	struct score b = createScore(-1, 100, 1, 100); 
	struct score_parameters parameters = {
		.remainingDepth=depth,
		.startDepth=depth,
		.isMaximizingPlayer=1,
		.goalWords=goalWords,
		.avoidWords=avoidWords,
		.scoreFunction=flwc_score,
		.goalWordsFound=0			

	}; 
		

	struct score score = minimax2(word, depth, parameters.isMaximizingPlayer, parameters, a, b, data);

	if(score.wordId != -1){
		markUsed_WordSet(score.wordId, data->wordSet); 
	}

	// Then run minimax using that score 
	return score.wordId; 
} 

int botPly_Mirror(int word){
	return word; 
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
int botPly_MaxAdjacencies(int word, struct WordSet* goalWords, struct DataStructures* data){

    // Initialize the ID of the result, word ID to be returned
    int resultId = -1;

    // Get the set of words that could be used
    struct intList* options = getConnections(word, data->I2W);
    options = options->next;

    // otherwise, foreach option in options
    while(options != NULL){

	// get the current option
	int optionId = options->data; 

	//printf("\n\n---Choosing Between: %s and %s----\n\n", Convert_IntToWord(resultId, data->I2W), Convert_IntToWord(optionId, data->I2W)); 

	// if the word is used, continue
	if(checkIfUsed_WordSet(optionId, data->wordSet)){
		//printf("Option Is Used, NEXT\n\n"); 
		options = options->next;
		continue; 
	}
	//printf("Option Is Not Used, Moving Forward\n"); 
	// if there aren't any words currently, just grab it so long as its not used
        if (resultId == -1) {
		///printf("Active ID is -1, Choosing %d\n", optionId); 
        	resultId = optionId; 
		options = options->next;
		continue; 
        }
	//printf("Active ID Not -1, Moving Forward\n"); 

	// If the current word is in the goal list and the new one is not, choose the new one 
	if(goalWords != NULL){
		//printf("Goal Words Is Not NULL\n"); 
		int currWordInGoalWordSet = checkIfUsed_WordSet(resultId, goalWords); 
		int optionInGoalWordSet = checkIfUsed_WordSet(optionId, goalWords); 
		//printf("Curr Word In Goal Set: %s\n", (currWordInGoalWordSet) ? "true" : "false"); 
		//nprintf("Option In Goal Set: %s\n", (optionInGoalWordSet) ? "true" : "false"); 
		// if the current word is in the goal set (bot loses) and current option isn't choose the option regardless of num connections
		if(currWordInGoalWordSet && !optionInGoalWordSet){
			//printf("Curr Word is in the Goal and the Option is Not, Choosing %d\n", optionId);
			resultId = optionId; 
			options = options->next; 
			continue; 
		}
		if(!currWordInGoalWordSet && optionInGoalWordSet){
			//printf("Curr Word is not in the Goal Set and the Option is, Selecting Current Word\n");
			options = options->next; 
			continue; 
		}
		//printf("The Current Word is not in the goal set OR the Option is in the Goal Set, Moving Forward\n");
		// if the current word is in 
	}
        // Number of Options on Current Word (Right Now it needs to be updated)
	int currWordNumConnections = getNumOptions(resultId, data); 

	// Number of Options on Potential Word (Right now it needs to be updated to take into account used words)
	int optionNumConnections = getNumOptions(optionId, data); 
	
	// If the Potential Word Has More Options Than the Current
        if (currWordNumConnections >= optionNumConnections) {
		//printf("The Current Word Has More Connections than the option, NOT SELECTING THE OPTION \n"); 
		options = options->next; 
		continue; 

	}
	// if all parameters exceed, choose the result
	//printf("Success! Selecting The Option"); 
	resultId = optionId; 
        options = options->next;

    }
    if(resultId != -1){
        markUsed_WordSet(resultId, data->wordSet);
    }
    // Return the Chosen Word!!
    return resultId;
}

void generalizedFLWCGame(struct DataStructures* data){

	// I. create the word set of words to be avoided 	
	struct WordSet* goalWords = init_WordSet(data->I2W->numWords); 
	struct WordSet* avoidWords = init_WordSet(data->I2W->numWords); 
	for(int i = 0; i < data->I2W->numWords; i++){
		char* currWord = Convert_IntToWord(i, data->I2W);
		// Words to be avoided
		if(strchr(currWord, 'e') != NULL){
			markUsed_WordSet(i, avoidWords); 
		}
		// Words to be achieved
		else if(strchr(currWord, 'o') != NULL){
			//markUsed_WordSet(i, goalWords); 
		}
		
	}
	

	// II. Run the Game	
	int startWord = 0; 
	int depth = 2;   
	markUsed_WordSet(startWord, data->wordSet); 
	int word = startWord;  
	// Let teh bot take a turn 
	for(int i = 0; i < 5; i++){
		
		word = botPly_FLWC(word, depth, goalWords, avoidWords, data);
		printf("Bots Word: %s\n", Convert_IntToWord(word, data->I2W));
		word = botPly_Random(word, data); 
		printf("Random Word: %s\n", Convert_IntToWord(word, data->I2W));
	}

	// III. Free the Components 
	free_WordSet(goalWords); 
	free_WordSet(avoidWords); 
	

}
#endif
