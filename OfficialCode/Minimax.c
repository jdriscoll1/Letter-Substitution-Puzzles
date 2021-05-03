#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Minimax.h"
#include "IntLinkedList.h"

/*This is the C file for the minimax algorithm

Description: The four letter word game involves pathfinding via letter substitution to trap your opponent into a spot at which there are no places to go 
To Do So, I will make use of the minimax algorithm*/

#define MAX 100000

struct minimaxOutput* minimax(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap){
	//printf("%d\n", depth); 
	if(depth == 0){
		return createOutput(0, .5, id); 
		
	}
	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//We will place this node in the HahsMap 
	setAlgFound(id, IntToWord_HashMap); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap){
	//The current minimum evaluation is going to be +infinity since we want something lower than that
	struct minimaxOutput* absEval = (isMaximizingPlayer == 1) ? createOutput(-100, -100, -1) : createOutput(100, 100, -1);  
	//The current word being checked
	int currID; 
	//Integer that keeps track of number of connections
	int numConnections = 0; 
	//Go through each child node
	while(currConnection != NULL){
		currID = currConnection->data; 
		//Make sure that the word has not already been found in the hash set
		if(getAlgFound(currID, IntToWord_HashMap) == 0){
			
		
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax2(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap); 
			//printf("\nAt %d: %d or %d (min) Choice: %d\n", id, potential->id, minEval->id, (compareOutput(minEval, potential) == 0) ? potential->id : minEval->id); 
			//printf("Choose Between: %d %d. Compare Min: %d, %d. Output: %d\n", minEval->id, potential->id, minEval->score, potential->score, compareOutput(minEval, potential));
			//Set the min eval to the min between the algEval or the current minEval
			if(compareOutput(absEval, potential) == isMaximizingPlayer){
				free(absEval); 
				absEval = potential; 
				
			}
			else{
				free(potential); 
			}
			//Make sure the number of connections goes up
			numConnections++; 
		}
		//Go to the next node
		currConnection = currConnection->next; 
	}
	//If the number of connections is none, create the minimaxOutput node
	if(numConnections == 0){
		//printf("No Connections\n"); 
		free(absEval); 
		//If there are no connections, the algorithm has, albeit sadly, lost. 
		if(depth == maxDepth){
			return NULL; 
		}
		
		absEval = (isMaximizingPlayer == 1) ? createOutput(-1, 0, id) : createOutput(1, 100, id); 
	}
	if(depth != maxDepth){
	
		removeAlgFound(id, IntToWord_HashMap); 
	}
	//if the depth is 0, it cannot go any further, and this node remains a mystery as to whether it is advantageous to go here
	if(depth == 0){
		return createOutput(0, .5, id); 
	}
	if(depth != maxDepth){
		absEval->id = id; 
	} 
	
	//Return the minimum evaluation
	return absEval; 

	
}

void DFS(int id, struct wordDataArray* IntToWord_HashMap){
	//Start at a node
	
	//Set that node to true in the hash map
	setAlgFound(id, IntToWord_HashMap); 
	//Output the node
	//Obtain node's connections
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Get off of the header
	currConnection = currConnection->next; 
	//Loop through those connections, running DFS
	while(currConnection != NULL){
		if(getAlgFound(currConnection->data, IntToWord_HashMap) == 0){
			DFS(currConnection->data, IntToWord_HashMap); 
		}
		currConnection = currConnection->next; 
		
	}
	//Set node found to false
	removeAlgFound(id, IntToWord_HashMap); 
	
}

int compareOutput(struct minimaxOutput* curr, struct minimaxOutput* potential){
	//We want to return who ever has a higher score
	//We want to keep the earlier ones becaues they should have less connections, so between the two we do not change the current output
	if(potential->score > curr->score){
		return 1; 
	}
	else{
		return 0; 
	}
	
}

struct minimaxOutput* createOutput(int score, int winPercent, int id){
	struct minimaxOutput* newOutput = malloc(sizeof(struct minimaxOutput)); 
	newOutput->score = score; 
	newOutput->winPercent = winPercent; 
	newOutput->id  = id; 
	return newOutput; 
	
}


struct minimaxOutput* minimax2(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap){

	if(depth == 0){
		return createOutput(0, .5, id); 
	}
	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//We will place this node in the HahsMap 
	setAlgFound(id, IntToWord_HashMap); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	//Otherwise, it's time for recursion  
	//if it is the maximizing player's turn, then we want to return the best move
	if(isMaximizingPlayer == 1){
		//The current maximimum evaluation is going to be -infinity, since we want something higher than that 
		struct minimaxOutput* maxEval = createOutput(-100, -100, -1); 
		//The current id to go with
		int currID; 
		//Integer that keeps track of number of connections
		int numConnections = 0; 
		//Go through each child node
		while(currConnection != NULL){
			currID = currConnection->data; 
			//Make sure that it is not in the hash map
			if(getAlgFound(currID, IntToWord_HashMap) == 0){
				
				//Set the algorithm evaluation to the minimax, making sure that, when setting the params, the depth goes down by 1, that it is false, and that it is putting in the child ID
				struct minimaxOutput* potential = minimax(currID, depth - 1, maxDepth, 0, IntToWord_HashMap); 
				//printf("At %d: %d or %d (max) Choice: %d\n", id, potential->id, maxEval->id, (compareOutput(maxEval, potential) == 1) ? potential->id : maxEval->id); 
				//printf("Choose Between: %d %d. Compare Max: %d, %d. Output: %d\n", maxEval->id, potential->id, maxEval->score, potential->score, compareOutput(maxEval, potential)); 
				//Set the max eval to the max between the algEval or the current maxEval
				if(compareOutput(maxEval, potential) == 1){
					free(maxEval); 
					maxEval = potential; 
				}
				 
				
				
				else{
					//Free whichever structure does not get chosen
					free(potential); 	
				}
				numConnections++; 
			}
			
			currConnection = currConnection->next; 
			
		}
		//If no numbers connect, then we create the node, and set it to the max
		if(numConnections == 0){
			//printf("No Connections\n"); 
			//We free maxEval to make space for a new one
			free(maxEval); 
			//End the game if the algorithm has lost
			if(depth == maxDepth){
				return NULL; 
			}
			
			maxEval = createOutput(-1, 0, id); 
		}
		
		if(depth != maxDepth){	
			removeAlgFound(id, IntToWord_HashMap);
		}
 		//Real Quick Explanation: We want to change the node to the current id of the output for all of them except for the last one
 		//Meaning, if currDepth == maxDepth, we do not want to change the output's id to the curr id, otherwise, we do. Otherwise, it will only have the absolute bottom node's ID. Which is not good. 
		if(depth != maxDepth){
			maxEval->id = id; 
		} 

		//Return the maximum evaluation
		return maxEval; 
		
	}
	
	
	
	
	//if it is the minimizing player's turn, then we want to return the min move
	if(isMaximizingPlayer == 0){
		//The current minimum evaluation is going to be +infinity since we want something lower than that
		struct minimaxOutput* minEval = createOutput(100, 100, -1);  
		//The current word being checked
		int currID; 
		//Integer that keeps track of number of connections
		int numConnections = 0; 
		//Go through each child node
		while(currConnection != NULL){
			currID = currConnection->data; 
			//Make sure that the word has not already been found in the hash set
			if(getAlgFound(currID, IntToWord_HashMap) == 0){
				
			
				//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
				struct minimaxOutput* potential = minimax(currID, depth - 1, maxDepth, 1, IntToWord_HashMap); 
				//printf("\nAt %d: %d or %d (min) Choice: %d\n", id, potential->id, minEval->id, (compareOutput(minEval, potential) == 0) ? potential->id : minEval->id); 
				//printf("Choose Between: %d %d. Compare Min: %d, %d. Output: %d\n", minEval->id, potential->id, minEval->score, potential->score, compareOutput(minEval, potential));
				//Set the min eval to the min between the algEval or the current minEval
				if(compareOutput(minEval, potential) == 0){
					free(minEval); 
					minEval = potential; 
					
				}
				else{
					free(potential); 
				}
				//Make sure the number of connections goes up
				numConnections++; 
			}
			//Go to the next node
			currConnection = currConnection->next; 
		}
		//If the number of connections is none, create the minimaxOutput node
		if(numConnections == 0){
			//printf("No Connections\n"); 
			free(minEval); 
			//If there are no connections, the algorithm has, albeit sadly, lost. 
			if(depth == maxDepth){
				return NULL; 
			}
			
			minEval = createOutput(1, 100, id); 
		}
		if(depth != maxDepth){
		
			removeAlgFound(id, IntToWord_HashMap); 
		}
		if(depth == 0){
			return createOutput(0, .5, id); 
		}
		if(depth != maxDepth){
			minEval->id = id; 
		} 
		
		//Return the minimum evaluation
		return minEval; 
	}
	

}

