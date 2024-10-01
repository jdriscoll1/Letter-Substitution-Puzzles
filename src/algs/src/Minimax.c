#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "../includes/Minimax.h"
#include "../../structs/includes/IntLinkedList.h"


int TOTAL_MOVES = 0;
int TRANS_SAVED = 0; 

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

struct minimaxOutput* minimax(int id, int currDepth, int maxDepth, int isMaximizingPlayer, struct minimaxOutput alpha, struct minimaxOutput beta, struct wordDataArray* wordGraph, struct WordSet* wordSet){

	// Acquires the front of the list of adjacencies
	struct intList* currConnection = getConnections(id, wordGraph); 
	
	// Moves off of the dummy header node
	currConnection = currConnection->next; 

	// Mark the current word as used
	markUsed_WordSet(id, wordSet); 
	
	// The initial worst score is going to be +/- infinity
	struct minimaxOutput* maxScore = (isMaximizingPlayer == 1) ? createOutput(-100, 0, -1, -1) : createOutput(100, 1, -1, -1);  
	
	// Number of connections a word has 
	int numConnections = 0; 
	
	// Likelyhood of winning in a given round
	double winPercent = 0; 
	
	// Boolean that determines if a word is pruned
	int isPruned = 0; 
	
	// Go through each child node and give it a score
	while(currConnection != NULL){
		
		// The current word being checked
		int currID = currConnection->data; 
		
		// Verify that the word has not already been found in the hash set
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			
			// Increment the parent's words number of connections
			numConnections++; 

			// When its depth reaches 0, it is forced to quit for computation limits and returns a score of uncertainty 
			if(currDepth == 0){
				// Frees the score node
				free(maxScore); 
				
				// Marks the current word as unused
				markUnused_WordSet(id, wordSet);
				
				// Returns a score stating it is uncertain whether it is good or bad 
				return createOutput(0, .5, 0, id);  
			}

			// Rerun the minimax algorithm with the current child as the node being scored
			struct minimaxOutput* currScore = minimax(
				// the current node's id
				currID, 
				// It is branching out a node dfs-style, the depth must be decremented
				currDepth - 1, 
				// the furthest its able to search, the starting point
				maxDepth, 
				// Switch whether it is taking the perspective of itself or its opponent 
				(isMaximizingPlayer == 1) ? 0 : 1, 
				// Alpha in alpha beta pruning
				alpha, 
				// Beta in alpha beta pruning
				beta, 
				//the "game board" / The Graph 
				wordGraph, 
				// The set tracking which words are used
				wordSet
				); 


			// It will add the likelihood of this node being good or bad if the child isn't a certain win or loss
			if(currScore->score >= -1 && currScore->score <= 1){
				winPercent += currScore->winPercent;
			}
	
	
			// Compares the best score to the word being analyzed, if the maximum is better, than it chooses it
			if(compare_mo(maxScore, currScore, isMaximizingPlayer) == isMaximizingPlayer){
				free(currScore);	
			}
			
			// If the current score is better it is replaced with the max
			else{
				free(maxScore); 
				maxScore = currScore; 
			}
			
			
			// Alpha-Beta Pruning Algorithm That Prevents Node From Exploring Further in nodes that are guarenteed losses
			if(AlphaBetaPruning(&alpha, &beta, maxScore, isMaximizingPlayer) == 1){
				isPruned = 1;
				break;
			}
			
			
			 
			
			
		}
		// Explore & score the next node
		currConnection = currConnection->next; 
		
	}
	
	// After all of the child nodes have been explored

	// If the parent node has no children to explore, it will return this either as a winning or losing node
	if(numConnections == 0 && isPruned == 0){

		//It frees the absolute node
		free(maxScore); 
		
		// If the root node has no connections, the algorithm will return it has lost 
		if(currDepth == maxDepth){
			// returns node with word id set to -1 indicating that the game is lost
			return createOutput(-1, 0, -1, -1); 
		}
		
		// If has no connections from a deeper node, it will return this as either a winning/losing node
		maxScore = (isMaximizingPlayer == 1) ? createOutput(-1, 0, currDepth, id) : createOutput(1, 1, currDepth, id); 
	}
	
	// If the parent node has unvisited child nodes it will set the likelihood of winning
	else if(numConnections != 0){
		// Divide the likelihood of winning by the number of connections 
		winPercent /= (double)numConnections; 
		maxScore->winPercent = winPercent; 
	}
	
	// If it is not at the root node, it sets the max score to the current id, marks the current word as unused
	// This is necessary because we don't want the root node to be explorable 
	if(currDepth != maxDepth){
		maxScore->id = id; 
		markUnused_WordSet(id, wordSet); 
		
	}

	// Return the best possible option of the parent node's children depending on the perspective
	return maxScore; 
	
}







/* 
KEEP NOTE:
1 = Good for Maximizer
0 = Good for Minimizer 
*/
//If a is a better score than b, return 1
//If b is a better score than a, return 0
//If a is better minimizer chooses b
//If b is better minimizer chooses a
//If they are both equal, they both want to choose a 
int compare_mo(struct minimaxOutput* a, struct minimaxOutput* b, int isMaximizingPlayer){

	//If a > b, return 1 
	if(a->score > b->score){return 1;}
 
	//If b > a return 0
	if(a->score < b->score){return 0;}
	//If b == a, check beta score
	
	//If a has a higher chance of winning, return 1 
	if(a->winPercent > b->winPercent){return 1;}
	//If b has a higher chance of winning, return 0
	if(b->winPercent > a->winPercent){return 0;}
	
	//If it is the maximizing player
	if(isMaximizingPlayer == 1){
		//if the maximizing player is winning choose the greatest depth
		if(a->score == 1){
			//If a is greater than b, choose a
			if(a->depth > b->depth){return 1;}
			//If b is greater than a, choose b
			if(b->depth > a->depth){return 0;}
		}
		//If the maximizer is losing, choose the smaller depth
		if(a->score == -1){
			//If a is closer than b, choose b
			if(a->depth > b->depth){return 0;}
			//If b is closer than a, choose a
			if(a->depth < b->depth){return 1;}
		}
		//If the maximizing player is losing choose the smallest depth 
		
	}
	//If it is the minimizing player
	if(isMaximizingPlayer == 0){
		//If the maximzing player is winning
		if(a->score == 1){
			//We want to choose the shortest route -- the higher number
			//If a is closer than b, we want to choose b, therefore return 0, because b is good for the minimizer
			if(a->depth > b->depth){return 1;}
			return 0;
		}
		if(a->score == -1){
			if(a->depth >= b->depth){return 0;}
			return 1; 
		}
	}
	
	//And lastly, if they are all exaaactly equal, return 1
	return 1; 
	
	
}
int AlphaBetaPruning(struct minimaxOutput *alpha, struct minimaxOutput* beta, struct minimaxOutput* absEval, int isMaximizingPlayer){
	
	
	//Alpha Beta Pruning Here
	//If it is the maximizer's turn
	if(isMaximizingPlayer == 1){
		
		//Compares the output of the alpha and the output
		//alpha = max(alpha, absEval)
		if(compare_mo(absEval, alpha, 1) == 1){
			 //Alpha becomes absEval 
			 copy_mo(alpha, absEval); 
		}
		//If alpha is greater 
		//Alpha stays the same
	
		
		//Compares alpha to beta 
		//If alpha is greater than or equal to beta (if beta is less than or equal to alpha)
		//if betea <= alpha
		if(compare_mo(alpha, beta, 1) == 1){
			//it pruned
			//break
			return 1;  
			
		}
		
	}
	//If it is the minimizer's turn
	else{
		

		//absEval = min(absEval, beta)
		if(compare_mo(absEval, beta, 0) == 0){
			copy_mo(beta, absEval); 
		}
		

		//if beta <= alpha
		if(compare_mo(beta, alpha, 0) == 0){
			//it pruned
			return 1; 
		}
	}
	return 0; 
			
}


int compareOutput(struct minimaxOutput* curr, struct minimaxOutput* potential, int isMaximizingPlayer){

	//maximizer chooses max value, so if the potential score is greater than the current score. It will want to return 1. 
	if(potential->score > curr->score){
		return 1; 
	}
	else if(potential->score == curr->score){
		int primary = potential->score; 
		//If it's the maximizer, it has to check if the winPercentage of this is higher
		//If it's the minimizer, it will immediately compare the depth, and not check the percentage (the secondary variable is only for if the algorithm is worse than this algorithm. Meaning it would be pointless to have the minimizer check it)
		if(isMaximizingPlayer == 1 && potential->winPercent != curr->winPercent){
			return compareWinPercent(potential->winPercent, curr->winPercent); 
		
		}
		//Next, it is necessary to check the depth 
		return compareDepth(curr, potential, primary, isMaximizingPlayer); 
		
	}
	return 0; 
	
}

void copy_mo(struct minimaxOutput* a, struct minimaxOutput* b){
	a->id = b->id; 
	a->score = b->score; 
	a->winPercent = b->winPercent; 
	a->depth = b->depth; 
	
	
}

int compareDepth(struct minimaxOutput* curr, struct minimaxOutput* potential, int primary, int isMaximizingPlayer){
	int currDepth = curr->depth; 
	int potDepth = potential->depth; 
	switch(primary){
		//Good For Primary
		case 1:
			//Maximizer only wants potential if it has less moves
			if(potDepth >= currDepth){return 1;}
			break;
		//Does Not Matter
		case 0: 
			return (isMaximizingPlayer == 0) ? 1 : 0; 
			 
		case -1:
			//Maximizer only wants potential if it has more moves
			if(potDepth <= currDepth){return 1;}
			break; 
		
		
		
	}
	
	return 0; 
	
	
}

int compareWinPercent(double potential, double curr){
	if(potential > curr){
		return 1; 
	}
	return 0; 
}



struct minimaxOutput* createOutput(int score, double winPercent, int depth, int id){
	struct minimaxOutput* newOutput = malloc(sizeof(struct minimaxOutput)); 
	newOutput->score = score; 
	newOutput->winPercent = winPercent; 
	newOutput->id  = id; 
	newOutput->depth = depth; 
	return newOutput; 
	
}

void Print_MinimaxOutput(struct minimaxOutput *mo){
	printf("%d: {%d, %d%%, %d}\n", 
		mo->id, 
		mo->score, 
		(int)(mo->winPercent * 100.0), 
		mo->depth); 
}




void DFS(int id, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	//Start at a node
	
	//Set that node to true in the hash map
	markUsed_WordSet(id, wordSet); 
	//Output the node
	//Obtain node's connections
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Get off of the header
	currConnection = currConnection->next; 
	//Loop through those connections, running DFS
	while(currConnection != NULL){
		
		if(checkIfUsed_WordSet(currConnection->data, wordSet) == 0){
			DFS(currConnection->data, IntToWord_HashMap, wordSet); 
			
		}
		printf("%d ", currConnection->data);
		currConnection = currConnection->next; 
		
		
	}

	//Set node found to false
	markUnused_WordSet(id, wordSet); 
	
}
