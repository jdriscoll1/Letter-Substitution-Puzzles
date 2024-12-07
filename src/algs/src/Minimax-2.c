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
	//printf("Num Options: %d\n", numOptions); 
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
			
			//printf("Curr Word: %d, is Used: %d", conn->data, checkIfUsed_WordSet(conn->data, data->wordSet)); 
			// If it is go to the next word
			conn = conn->next; 
		}
		//printf("Broke From the Loop");
		// Otherwise, check if the the current id is equal to the choice id relative to unused words 
		if(currId == choiceIdRelativeToUnusedWords){
			choiceId = conn->data; 		
		}
		currId++; 	
		conn = conn->next; 	
		
	}
	
	return choiceId; 
}

// it needs the current word
struct score flwc_score(int id, struct DataStructures* data, struct score_parameters parameters){

	// Check to see if the current word is the goal word
	if (id == parameters.goalId){
		return createScore(id, 1, 1, 0); 
	}

	// Check to see if there are any connections, if there are return null, otherwise return a score 		
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

struct score minimax2(int id, int goalId, int remainingDepth, int startDepth, int isMaximizingPlayer, struct score alpha, struct score beta, struct DataStructures* data, struct score (*scoreFunction)(int,struct DataStructures*, struct score_parameters)){

	// Are we at the root word? 
	int isRoot = (remainingDepth == startDepth); 
	
	struct score_parameters parameters = {
		.remainingDepth=remainingDepth,
		.isMaximizingPlayer=isMaximizingPlayer, 
		.goalId=goalId
		
	}; 
	struct score leafScore = scoreFunction(id, data, parameters);

	
	// if there are no connections left
	if(leafScore.wordId != -1){
		if(isRoot){
			return createScore(-1, 0, 0, 0); 
		}
		return leafScore; 
	}

	// if it's reached its depth limit, it should return a score
	if(remainingDepth == 0){
		// Returns a score stating it is uncertain whether it is good or bad 
		return createScore(id, .5, .5, 0);  
	}

	// Acquires the front of the list of adjacencies
	struct intList* conn = getConnections(id, data->I2W); 
	

	// Mark the current word as used
	markUsed_WordSet(id, data->wordSet); 
	
	// The initial worst score is going to be +/- infinity
	struct score maxScore = (isMaximizingPlayer) ? createScore(-1, -100, 1, 100) : createScore(-1, 100, 1, 100);  
	
	// Number of connections a word has 
	int numConnections = 0; 
	
	// Likelyhood of winning in a given round
	double winPercentage = 0; 
	
	// Go through each child node and give it a score
	while(conn->next != NULL){
		
		//printf("%s: maxScore = %f alpha = %f beta = %f maximizingPlayer = %d\n", currWord, maxScore.score, alpha.score, beta.score, isMaximizingPlayer);

		conn = conn->next; 
		
		// Verify that the word has not already been found in the hash set
		if(checkIfUsed_WordSet(conn->data, data->wordSet) != 0){
			continue; 	
		} 			

		numConnections++; 
		// Rerun the minimax algorithm with the current child as the node being scored
		struct score candidate = minimax2(conn->data, goalId, remainingDepth - 1, startDepth, (isMaximizingPlayer) ? 0 : 1, alpha, beta, data, scoreFunction); 

		// Compares the best score to the word being analyzed, if the maximum is better, than it chooses it
		int winnerId = compareScores(candidate, maxScore, isMaximizingPlayer); 

		if(winnerId == candidate.wordId){
			maxScore = candidate; 
		}
		winPercentage += maxScore.winPercentage; 
		
		
		// Alpha-Beta Pruning Algorithm That Prevents Node From Exploring Further in nodes that are guarenteed losses
		if(AlphaBetaPrune(&alpha, &beta, maxScore, isMaximizingPlayer)){
                        //printf("Prune!\n");
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
