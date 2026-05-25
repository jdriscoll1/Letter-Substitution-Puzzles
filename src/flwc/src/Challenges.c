#include <string.h>

#include "../includes/Challenges.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../../algs/includes/TreeStorageNode.h"
#include "../../structs/includes/Queue.h"

void swapAvoidGoal(struct StartWordParametersFLWC* p);
int all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(int id, int minDistance, int maxDistance, struct WordSet* goalWords, struct WordSet *avoidWords, struct DataStructures* data);
int is_game_winnable_FLWC(
	// the current id being searched	
	
	int id, 
	// this is the current depth 
	int depth, 
	// is it currently the player or bot's perspective	
	int isPlayerPerspective, 
	// the set of goal words	
	struct WordSet* goalWords, 
	// the set of avoid words
	struct WordSet* avoidWords, 
	// the words must not be too close
	struct DataStructures* data, 
	// alpha --> Initialized to -100
	int alpha, 
	// beta  --> Initialized to +100
	int beta);

int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	
	// The array of valid words
	struct arrayList* validWords = init_ArrayList(20, 10, NUM); 	
	
	
	// for wordId in allWords
	for(int i = 0; i < data->I2W->numWords; i++){
		// CHECK #0: The Current Word Is Not in the goal word nor avoid word set
		if(checkIfUsed_WordSet(i, p.goalWords) || checkIfUsed_WordSet(i, p.avoidWords)){
			continue; 
		}

		// CHECK #1: Does the word have a # of adjacencies in Range
		int n = data->I2W->array[i]->numConnections; 
		if(n < p.minAdjacencies || n > p.maxAdjacencies){
			continue; 
		} 

	
		// CHECK #2: If there exists a goal word < the minimum distance, continue 
		// CHECK #3: If there are no goal words < the maximum distance, continue 
		if(!all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(i, p.minGoalDistance, p.maxGoalDistance, p.goalWords, p.avoidWords, data)){
			continue; 
		
		}

		// CHECK #4: If there exists an avoid word < the minimum distance, continue 
		// CHECK #5: If there are no avoid words < the maximum distance, continue 
		if(!all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(i, p.minAvoidDistance, p.maxAvoidDistance, p.avoidWords, p.goalWords, data)){
			continue; 
		}
		

		// CHECK #6: If the user cannot force a win, continue 
		// num turns does not apply to FLWGP therefore
		if(p.numTurns != -1){
			if(!is_game_winnable_FLWC(i, p.numTurns, 1, p.goalWords, p.avoidWords, data, -100, 100)){
				continue; 
			} 
		}
		

		add_ArrayList(&i, validWords, NUM); 
	}

	// If the Array Length Is Empty - There Are No Valid Words
	if(validWords->currPrecision == 0){
		printf("There are no valid words!!!\n"); 
		free_ArrayList(validWords); 
		return -1; 
	}
	
	int choiceId = rand() % validWords->currPrecision; 
	int startWordId = ((int*)validWords->list)[choiceId]; 

	
	free_ArrayList(validWords); 
	
	
	return startWordId; 
}








// If there exists a goal word that's less than teh minimum distance, return true
int all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(int id, int minDistance, int maxDistance, struct WordSet* goalWords, struct WordSet *avoidWords, struct DataStructures* data){


	// if the min distance and max distance are both 0, return false
	if(minDistance == 1 && maxDistance == 1){
		return 1; 
	}
	// Intitiate a Queue
	struct Queue* q = init_Queue(); 
	struct WordSet* exploredNodes = init_WordSet(data->I2W->numWords); 
	markUsed_WordSet(id, exploredNodes); 

	// Get Options
	enqueue(id, 0, NULL, q); 
	// there_exists_a_word_in_the_wordset_that_is_less_than_the_min_distance SUCCESS ON FALSE
	int min_distance_constraint = 0; 
	//bool there_exists_a_word_in_the_wordset_that_is_less_than_the_max_distance = false; SUCCESS ON TRUE 
	int max_distance_constraint = 0;
	

	while(!isEmpty_Queue(q)){
		
		// We get the parent node on a dequeue
		struct QueueNode* parent = dequeue(q); 
		// We get the parent's distance
		int distance = parent->data->distance; 

		// The current node's distance is an increment of the parent's distance
		int childDistance = distance + 1; 

		// we're viewing the parent's node
		int currId = parent->data->id; 
		int wordInSet = checkIfUsed_WordSet(currId, goalWords); 
		if(wordInSet){
			if(distance < minDistance){
				min_distance_constraint = 1; //true
				break; 
			}
			if(distance <= maxDistance){
				max_distance_constraint = 1; //true
			}
		}
		if(distance >= maxDistance){
			continue;  	
		}
		struct intList* conn = getConnections(currId, data->I2W); 
		while(conn->next != NULL){
			conn = conn->next; 
			int currConnId = conn->data; 
			int isCurrConnExplored = checkIfUsed_WordSet(currConnId, exploredNodes);
			int isCurrConnInAvoidWords = checkIfUsed_WordSet(currConnId, avoidWords);
			if(isCurrConnExplored || isCurrConnInAvoidWords){
				continue; 
			}
			enqueue(currConnId, childDistance, parent, q); 
			markUsed_WordSet(currConnId, exploredNodes); 
		}
	}
	free_Queue(q); 
	free_WordSet(exploredNodes); 
	return !min_distance_constraint && max_distance_constraint;


}



// Get IS GAME WINNABLE FLWC 
// 0 --> TIE
// -1 --> NOT WINNABLE
// 1 --> WINNABLE
int max(int a, int b) {
	return (a > b) ? a : b; 
}
int min(int a, int b){
	return (a < b) ? a : b; 
}
int is_game_winnable_FLWC(
	// the current id being searched	
	
	int id, 
	// this is the current depth 
	int depth, 
	// is it currently the player or bot's perspective	
	int isPlayerPerspective, 
	// the set of goal words	
	struct WordSet* goalWords, 
	// the set of avoid words
	struct WordSet* avoidWords, 
	// the words must not be too close
	struct DataStructures* data, 
	// alpha --> Initialized to -100
	int alpha, 
	// beta  --> Initialized to +100
	int beta){


	/*SECTION #1: CHECK TO SEE IF CURRENT WORD IS A LEAF NODE*/
	int isGoalWord = goalWords && checkIfUsed_WordSet(id, goalWords); 
	int isAvoidWord = avoidWords && checkIfUsed_WordSet(id, avoidWords); 
	// WIN if goal word reached
	if(isGoalWord){
		return 1; 
	}
	// LOSS if avoid word reached - if it's not in the avoid range it doesn't count :( 
	if(isAvoidWord){
		return 0; 
	}

	// WIN if loss avoided, but LOSS if goal word missed
	if(depth <= 0) {
		// if there are goal words, we did not reach them and lost
		return !goalWords; 
	}
	
	/****SECTION 2: CHECK WORDS LEAVES TO SEE IF ANY ARE AVOID/GOAL WORDS***/
	
	markUsed_WordSet(id, data->wordSet); 

	// Get the Current Words Adjacencies
	struct intList* options = getConnections(id, data->I2W); 
	options = options->next; 

	// Start of by doing max scores
	int result = (isPlayerPerspective) ?  -100 : 100; 

	while(options != NULL){
	
		int optionId = options->data; 
		// if the curr option is used, skip 
		if(checkIfUsed_WordSet(optionId, data->wordSet)){
			options = options->next; 
			continue; 
		}
		int option_score = is_game_winnable_FLWC(
			optionId, 
			depth - 1, 
			!isPlayerPerspective, 
			goalWords, 	
			avoidWords, 	
			data, 
			alpha, 
			beta
		);

		// if it's the player persepctive return max(option_score, result) otherwise min(option_score, result)
		result = (isPlayerPerspective) ? max(option_score, result) : min(option_score, result); 

		if(isPlayerPerspective){
			result = max(option_score, result); 
			alpha = max(alpha, result); 
		}
		else{
			result = min(option_score, result); 
			beta = min(beta, result); 
		}
		if(alpha >= beta){
			break; 
		}
		// Alpha Beta Pruning??
		options = options->next; 
	}
	markUnused_WordSet(id, data->wordSet); 
	return result; 
	
}

		/*
		if(p.minGoalDistance != 1 || p.maxGoalDistance != 1){
			isValid_Goal = isWordValid_StartWordParameters(i, p, data); 
		}
		if(p.minAvoidDistance != 1 || p.maxAvoidDistance != 1){
			swapAvoidGoal(&p); 
			isValid_Avoid = isWordValid_StartWordParameters(i, p, data); 
			swapAvoidGoal(&p); 
		}
		*/ 
/*
	struct WordSet *goalWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strstr(w1, "oo") != NULL){
			markUsed_WordSet(i, goalWordSet); 
		}
	}
	flwcComponents->goalWords = goalWordSet; 
	p.goalWords = goalWordSet; 

	// Avoid Words
	struct WordSet *avoidWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strchr(w1, 'd') != NULL){
			markUsed_WordSet(i, avoidWordSet); 
		}
	}
	flwcComponents->avoidWords = avoidWordSet; 
	p.avoidWords = avoidWordSet; 
	*/
	
struct WordSet* convertCharPtrPtrToWordSet(char** words, struct DataStructures* data){

	struct WordSet* wordSet = init_WordSet(data->I2W->numWords);
	int i = 0; 
	int wordId; 
	while(words[i] != NULL){
		
		wordId = convertWordToInt(words[i], data);
	 	markUsed_WordSet(wordId, wordSet); 	
		i++; 
	}
	return wordSet; 
}

int isWordValid_StartWordParameters(int i, struct StartWordParametersFLWC p, struct DataStructures* data){
	struct arrayList* pathToNearestWord = getPathToNearestWordInWordSet(i, p, data);  
	int distanceFromGoal = pathToNearestWord->currPrecision; 
	free_ArrayList(pathToNearestWord); 
	return distanceFromGoal >= p.minGoalDistance && distanceFromGoal <= p.maxGoalDistance; 

}
void swapAvoidGoal(struct StartWordParametersFLWC* p) {
    int a = p->minAvoidDistance;
    int b = p->maxAvoidDistance;
    struct WordSet* c = p->avoidWords;
    
    p->minAvoidDistance = p->minGoalDistance;
    p->maxAvoidDistance = p->maxGoalDistance;
    p->avoidWords = p->goalWords;
    
    p->minGoalDistance = a;
    p->maxGoalDistance = b;
    p->goalWords = c;
}

void getSetOfSurroundingWords(int id, int distance, struct WordSet* wordSet, struct DataStructures* data){

	struct BFSResults result = BreadthFirstSearch_Distance(id, distance, data->I2W, data->wordSet); 

	struct TreeStorageNode* curr = result.dataStorage->ReverseTreeHeader; 
	curr = curr->next;

	while(curr != NULL){
		markUsed_WordSet(curr->id, wordSet);
		curr = curr->next; 
	}

	free_ArrayList(result.list); 
	Free_BFSComponents(result.dataStorage, data->wordSet);

}



