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
		return createOutput(0, .5, 0, id);  
	}
	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//Then I would also add it into the transposition hash
	//We will place this node in the HahsMap 
	setAlgFound(id, IntToWord_HashMap); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap){
	
	//The current minimum evaluation is going to be +infinity since we want something lower than that
	struct minimaxOutput* absEval = (isMaximizingPlayer == 1) ? createOutput(-100, 0, -1, -1) : createOutput(100, 1, -1, -1);  
	//The current word being checked
	int currID; 
	//Integer that keeps track of number of connections
	int numConnections = 0; 
	//This keeps track of the likelyhood of winning in a given round
	double winPercent = 0; 
	//Go through each child node
	while(currConnection != NULL){
		currID = currConnection->data; 
		//Make sure that the word has not already been found in the hash set
		if(getAlgFound(currID, IntToWord_HashMap) == 0){
			
		
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap); 
			
			winPercent += potential->winPercent; 
			//printf("\nAt %d: %d or %d (min) Choice: %d\n", id, potential->id, minEval->id, (compareOutput(minEval, potential) == 0) ? potential->id : minEval->id); 
			//printf("Choose Between: %d %d. Compare Min: %d, %d. Output: %d\n", minEval->id, potential->id, minEval->score, potential->score, compareOutput(minEval, potential));
			//Set the min eval to the min between the algEval or the current minEval
			//printf("%d -- ", depth); 
	
			
				//Print_MinimaxOutput(potential, IntToWord_HashMap); 
			
			if(compareOutput(absEval, potential, isMaximizingPlayer) == isMaximizingPlayer){
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
	winPercent /= (double)numConnections; 
	absEval->winPercent = winPercent; 
	
	//If the number of connections is none, create the minimaxOutput node
	if(numConnections == 0){
		//printf("No Connections\n"); 
		free(absEval); 
		//If there are no connections, the algorithm has, albeit sadly, lost. 
		if(depth == maxDepth){
			return NULL; 
		}
		
		absEval = (isMaximizingPlayer == 1) ? createOutput(-1, 0, depth, id) : createOutput(1, 1, depth, id); 
	}
	
	
	
	if(depth != maxDepth){
		absEval->id = id; 
		removeAlgFound(id, IntToWord_HashMap); 
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
int compareDepth(struct minimaxOutput* curr, struct minimaxOutput* potential, int primary, int isMaximizingPlayer){
	int currDepth = curr->depth; 
	int potDepth = potential->depth; 
	switch(primary){
		//Good For Primary
		case 1:
			//Maximizer only wants potential if it has less moves
			if(potDepth > currDepth){return 1;}
			break;
		//Does Not Matter
		case 0: 
			return (isMaximizingPlayer == 0) ? 1 : 0; 
			 
		case -1:
			//Maximizer only wants potential if it has more moves
			if(potDepth < currDepth){return 1;}
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

void Print_MinimaxOutput(struct minimaxOutput *mo, struct wordDataArray* IntToWord_HashMap){
	printf("%s: {%d, %d%%, %d}\n", Convert_IntToWord(mo->id, IntToWord_HashMap), mo->score, (int)(mo->winPercent * 100.0), mo->depth); 
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
	printf("%d ", id); 
	//Set node found to false
	removeAlgFound(id, IntToWord_HashMap); 
	
}
