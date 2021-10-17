#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "../includes/Minimax.h"
#include "../../structs/includes/IntLinkedList.h"

/*This is the C file for the minimax algorithm

Description: The four letter word game involves pathfinding via letter substitution to trap your opponent into a spot at which there are no places to go 
To Do So, I will make use of the minimax algorithm*/

#define MAX 100000
//Uses 50/50 chances for beta variable
struct minimaxOutput* minimax(int id, int depth, int maxDepth, int isMaximizingPlayer, struct minimaxOutput alpha, struct minimaxOutput beta, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet, unsigned long hash){
	//update the hash 
	hash ^= wordSet->words[id  / sizeof(unsigned long)]; 
	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//Then I would also add it into the transposition hash
	//We will place this node in the HahsMap 
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg(id, depth, maxDepth, isMaximizingPlayer, currConnection, alpha, beta, IntToWord_HashMap, wordSet, hash); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct minimaxOutput alpha, struct minimaxOutput beta, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet, unsigned long hash){

	//The current minimum evaluation is going to be +infinity since we want something lower than that
	struct minimaxOutput* absEval = (isMaximizingPlayer == 1) ? createOutput(-100, 0, -1, -1) : createOutput(100, 1, -1, -1);  
	
	//The current word being checked
	int currID; 
	
	//Integer that keeps track of number of connections
	int numConnections = 0; 
	
	//This keeps track of the likelyhood of winning in a given round
	double winPercent = 0; 
	
	int isPruned = 0; 
	//Go through each child node
	while(currConnection != NULL){
		currID = currConnection->data; 
		//Make sure that the word has not already been found in the hash set
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			
			//Make sure the number of connections goes up
			numConnections++; 

			//If it's at 0 depth, if there still exists options, it return 50% as score
			if(depth == 0){
				free(absEval); 
				markUnused_WordSet(id, wordSet);
				//It also knocks the id off of the HashMap
				return createOutput(0, .5, 0, id);  
			}

			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax(
				// the current word id
				currID, 
				// the depth, new search makes it go down
				depth - 1, 
				// the furthest its able to search, the starting point
				maxDepth, 
				// whether its considering in terms of itself or its enemy 
				(isMaximizingPlayer == 1) ? 0 : 1, 
				//alpha in alpha beta pruning
				alpha, 
				//beta in alpha beta pruning
				beta, 
				//the "game board" 
				IntToWord_HashMap, 
				//the set of used words
				wordSet,
				//the current game state hash 
				hash); 

			//This only matters during the minimizer's turn
			if(potential->score <= 1 && potential->score >= -1){
				winPercent += potential->winPercent;
			}
			//Print_MinimaxOutput(potential); 
			
			/*
			Maximizer: potential > absEval -- choose abs eval. abs eval > potential -- chose abs Eval
			compareOutput(a,b) == 1 --> b > a
			compareOutput(a,b) == 0 --> a > b
			
			*/
		
			//Compares the absolute value to the potential 
			//IF it is the maximizer and it outputs 1, it should choose the first one
			//If it is the minimizer and it outputs 0, it should choose the first one
			//If it is the maximizer and it outputs 0, it should choose the second one
			//If it is the minimizer and it outputs 1, it should choose the second one
 			//if(depth == maxDepth){
			 	/*printf("Comparing Between:\n");
 				printf("%s\n", Convert_IntToWord(absEval->id, IntToWord_HashMap));
 				Print_MinimaxOutput(absEval);
 				printf("Or:\n%s\n", Convert_IntToWord(potential->id, IntToWord_HashMap));
 				printOptions(potential->id, IntToWord_HashMap);
 				Print_MinimaxOutput(potential);
 				if(potential->id == 419){
 					printf("\nHere\n");
				 }
 				printf("\n");
 				*/
		//	}
			if(compare_mo(absEval, potential, isMaximizingPlayer) == isMaximizingPlayer){
				free(potential);	
			}
			
			else{
				free(absEval); 
				absEval = potential; 
			}
			
			
			
			//Checks to see if it _really_ needs to check this tree
			if(AlphaBetaPruning(&alpha, &beta, absEval, isMaximizingPlayer) == 1){isPruned = 1;break;}//return absEval;}
			
			
			 
			
			
		}
		//Go to the next node
		currConnection = currConnection->next; 
		
	}
	
	//if there are no connections, and the depth is 0
	/*
	if(depth == 0){
		free(absEval); 
		removeAlgFound(id, IntToWord_HashMap);
		//It also knocks the id off of the HashMap
		return (isMaximizingPlayer == 1) ? createOutput(-1, 0, depth, id) : createOutput(1, 1, depth, id);  
	}
	*/

	//if it found no nodes
	if(numConnections == 0 && isPruned == 0){
		free(absEval); 
		//If there are no connections, the algorithm has, albeit sadly, lost. 
		if(depth == maxDepth){
			return NULL; 
		}
		absEval = (isMaximizingPlayer == 1) ? createOutput(-1, 0, depth, id) : createOutput(1, 1, depth, id); 
	}
	
	else if(numConnections != 0){
	
		winPercent /= (double)numConnections; 
		absEval->winPercent = winPercent; 
	}
	if(depth != maxDepth){
		absEval->id = id; 
		markUnused_WordSet(id, wordSet); 
		//I would also remove it from the transposition hash
		
	}
	
	
	


	//Return the minimum evaluation
	return absEval; 

	
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

int copy_mo(struct minimaxOutput* a, struct minimaxOutput* b){
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
	printf("%d: {%d, %d%%, %d}\n", mo->id, mo->score, (int)(mo->winPercent * 100.0), mo->depth); 
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
