#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "../includes/Minimax-2.h"
#include "../../structs/includes/IntLinkedList.h"
#include "../../structs/includes/WordSet.h"

/*This is the C file for the minimax algorithm

Description: The four letter word game involves pathfinding via letter substitution to trap your opponent into a spot at which there are no places to go 
To Do So, I will make use of the minimax algorithm*/

#define MAX 100000

/*  
* Abstract goal: The game to which the minimax algorithm is applied, abstractly, is a game in which there is a set of nodes. Each node connects to other nodes
* in a 2D undirected graph. In each ply, depending on whose turn it is, the user or algorithm step from one node to another node. They are bound to each other, wherever
* one chooses to move, the other must follow. They take turns moving and once a node is visited, it is no longer an option. If, in one of the participant's moves,   
*  there are no unexplored nodes to travel to, that participant is defeated and their opponent is the victor. Thus, the goal of the game is to trap one's opponent.  
* 
* The Minimax Algorithm 
* The Minimax Algorithm is used, in a 2-player zero-sum game, to look ahead at future moves in a depth-first search pattern and assign potential moves scores. 
* Each time it steps into a deeper move, it flips its perspective from itself to that of its opponents. When it finds a node that does not have any further 
* connections, if it is in its own perspective it assigns it a negative score, because it recognizes that it would lose. If it is in the perspective of its opponent
* it would assign it a very high score because it recognizes that it would win.
*  
* Alpha Beta Pruning
* This algorithm uses a form of pruning-known as Alpha-Beta Pruning which optimizes the search by assuming that, if it finds a node where, if reached
* its opponent has an oppurtunity to win, it will win. The algorithm terminates exploring future paths from that node because it is already a losing node. 
*
* The parameters which the algorithm takes are as follows
* id --> The current parent node is being scored and whose children are being explored  
* depth --> How far it has already explroed
* maxDepth --> How deep is the algorithm willing to explore considering time and computation limits
* isMaximizingPlayer --> The algorithm's perspective
* alpha --> The positive perspective for alpha beta pruning
* beta --> The negative perspective for alpha beta pruning 
* IntToWord_HashMap --> The Game Board
* wordSet --> The current set of words
*/


struct score flwg_score(int id, struct DataStructures* data, struct score_parameters parameters){

	if(parameters.remainingDepth == 0){
		// Returns a score stating it is uncertain whether it is good or bad 
		return createScore(id, .5, .5, 0);  
	}
	// Create a set of parameters called score parameters 
	struct intList* conn = getConnections(id, data->I2W); 
	conn = conn->next; 
	while(conn != NULL){

		// if there is a single word that is not used 
		if(! checkIfUsed_WordSet(conn->data, data->wordSet)){
			return createScore(-1, 0, 0, 0); 
		}
		conn = conn->next; 	
	}
	// if all the words are used, the game is over
	return (parameters.isMaximizingPlayer) ? createScore(id, 0, 0, parameters.remainingDepth) : createScore(id, 1, 1, parameters.remainingDepth);  

}


// This is the score function for FLWC -- 
// It returns a score struct: 
// wordId: The Word ID Determines whether minimax will return this as the score, or keep searching
	// If the wordId is id, return the word in minimax
	// if the word id is -1, keep searching 
// score: It only fills out hte score if it intends to return the score
	// if it finds a word it needs to avoid, it gives  it a very low score and returns it 
	// If it finds a word it needs as a goal, it increments hte number of goal words found 
	// If it reaches the maximum depth
// winPercentage:
// depth: 
struct score flwc_score(int id, struct DataStructures* data, struct score_parameters parameters){

	// 1) Check to see if word is goal/avoid
	if(parameters.remainingDepth != parameters.startDepth){
		if (checkIfUsed_WordSet(id, parameters.goalWords)){
			parameters.goalWordsFound++; 
		}

		if (checkIfUsed_WordSet(id, parameters.avoidWords)){
			return createScore(id, -1, 0, parameters.remainingDepth); 	
		}	

	}
	// 2) if max-depth reached, return number of goal words that were found
	if(parameters.remainingDepth == 0){
		return createScore(id, parameters.goalWordsFound, parameters.goalWordsFound, 0);  
	}

	// 3) Check to make sure this is not a word without connections, if it is, return a low score 
	struct intList* conn = getConnections(id, data->I2W); 
	conn = conn->next; 
	while(conn != NULL){
		// if there is a single connection that has not been explored, return -1
		if(!checkIfUsed_WordSet(conn->data, data->wordSet)){
			return createScore(-1, 0, 0, 0); 	
		}
		conn = conn->next; 	
	}
	// if it is a leaf node, return the id 
	return createScore(id, 0, 0, 0);  
}

int compareScores(struct score a, struct score b, int isMaximizingPlayer){

	// We flip the scores to positive and negative based on whose pepersepective it is 
	if(!isMaximizingPlayer){
		a.score = 1-a.score; 
		b.score = 1-b.score; 
	}	
	
	// First check if the scores are not equal
	if (a.score != b.score){
		return (a.score > b.score) ? a.wordId : b.wordId; 
	}
	if (a.winPercentage != b.winPercentage){
		return (a.winPercentage > b.winPercentage) ? a.wordId : b.wordId; 
	}
	// 
	if(a.depth != b.depth){
		// if it is a good score for whosever perspective it is 
		if(a.score > .5){
			// then we want to return whichever has the higher depth
			return (a.depth > b.depth) ? a.wordId : b.wordId; 
			
		}
	
		return (a.depth > b.depth) ? b.wordId : a.wordId; 	
	}
	return a.wordId; 


}

struct score minimax2(int id, int remainingDepth, int isMaximizingPlayer, struct score_parameters parameters, struct score alpha, struct score beta, struct DataStructures* data){
	
	parameters.isMaximizingPlayer = isMaximizingPlayer; 
	parameters.remainingDepth = remainingDepth; 
	/*
	if(remainingDepth == 1){
		printf("\n");
	}
	*/
	//printf("Word Being Searched: %s\n", Convert_IntToWord(id, data->I2W));
	// is root node in search
	int isRoot = (parameters.remainingDepth == parameters.startDepth); 
	
	struct score leafScore = parameters.scoreFunction(id, data, parameters);

	
	// if current word has no direct, unused connections
	if(leafScore.wordId != -1){
		// if root has no connections, the bot loses
		return (isRoot) ? createScore(-1, 0, 0, 0) : leafScore; 
	}

	// TO-DO: MOVE THIS FUNCTIONALITY INTO THE SCORE SYSTEM

	// Acquires the front of the list of adjacencies
	struct intList* conn = getConnections(id, data->I2W); 
	

	// Mark the current word as used
	markUsed_WordSet(id, data->wordSet); 
	
	// The initial worst score is going to be +/- infinity
	struct score maxScore = (parameters.isMaximizingPlayer) ? createScore(-1, -100, 1, 100) : createScore(-1, 100, 1, 100);  
	
	// Number of connections a word has 
	int numConnections = 0; 
	
	// Likelyhood of winning in a given round
	double winPercentage = 0; 
	
	// Go through each child node and give it a score
	while(conn->next != NULL){
		
		conn = conn->next; 
		
		//printf("Conn: %s\n", Convert_IntToWord(conn->data, data->I2W));
		// Verify that the word has not already been found in the hash set
		if(checkIfUsed_WordSet(conn->data, data->wordSet) != 0){
			continue; 	
		} 			

		numConnections++; 

		// Rerun the minimax algorithm with the current child as the node being scored
		struct score candidate = minimax2(conn->data, remainingDepth - 1, isMaximizingPlayer - 1, parameters, alpha, beta, data); 

		// Compares the best score to the word being analyzed, if the maximum is better, than it chooses it
		int winnerId = compareScores(candidate, maxScore, parameters.isMaximizingPlayer); 

		if(winnerId == candidate.wordId){
			maxScore = candidate; 
		}
		winPercentage += maxScore.winPercentage; 
		
		
		// Alpha-Beta Pruning Algorithm That Prevents Node From Exploring Further in nodes that are guarenteed losses
		if(AlphaBetaPrune(&alpha, &beta, maxScore, parameters.isMaximizingPlayer)){
			break;
		}
		
	}
	
	// Probably do something here regarding the chance variable right? 
	maxScore.winPercentage = winPercentage / numConnections; 

	// the root node acts mildly differently 	
	if(!isRoot){
		maxScore.wordId = id; 
		markUnused_WordSet(id, data->wordSet); 
	}

	return maxScore; 	
}


struct score createScore(int wordId, double score, double winPercentage, int depth) {
	struct score s = {wordId, score, winPercentage, depth};
	return s; 
}

void printScore(struct score s){
	printf("%d: {%f, %f, %d}\n", s.wordId, s.score, s.winPercentage, s.depth); 
}


int AlphaBetaPrune(struct score *a, struct score *b, struct score x, int isMaximizingPlayer){

	// If we are the maximizing player
	if(isMaximizingPlayer){
		// Alpha = Max of Alpha & Score
		*a = (compareScores(x, *a, 1) == x.wordId) ? x : *a; 
		// return x >= beta
		return compareScores(x, *b, 1) == x.wordId; 
	}
	
	// beta gets set to minimum of beta and score
	*b = (compareScores(x, *b, 1) == x.wordId) ? *b : x; 
	// return x <= alpha
	return compareScores(x, *a, 1) == a->wordId; 	




}

int choose_random_word(int id, struct DataStructures* data){

	// The number of words that can be connected to & have not already been used 	
	int numOptions = 0; 

	// This is the list of words that are options, used and unused
	struct intList* conn = getConnections(id, data->I2W); 

	//Get off the dummy head node
	conn = conn->next; 

	// Loop through the words and derive the number of words that have not been used
	while(conn != NULL){

		// if there is a single word that is not used 
		if(! checkIfUsed_WordSet(conn->data, data->wordSet)){
			numOptions++; 
		}
		conn = conn->next; 	
	}
	if(numOptions == 0){
		return -1; 
	}
	// choose randomly between [0, numConnectiosn]
	// Allow it to be inclusive of N 
	int choiceIdRelativeToUnusedWords = rand() % numOptions; 	
	int choiceId = -1;
	int currId = 0; 
	
	// Reset the connections
	conn = getConnections(id, data->I2W); 
	conn = conn->next; 

	// While the word has not been chosen 
	while(choiceId == -1){
		
		// Check if the current word is used
		while(checkIfUsed_WordSet(conn->data, data->wordSet)){
			
			// If it is go to the next word
			conn = conn->next; 
		}
		// Otherwise, check if the the current id is equal to the choice id relative to unused words 
		if(currId == choiceIdRelativeToUnusedWords){
			choiceId = conn->data; 		
		}
		currId++; 	
		conn = conn->next; 	
		
	}
	
	return choiceId; 
}


