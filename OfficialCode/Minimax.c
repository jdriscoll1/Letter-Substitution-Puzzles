#include "Minimax.h"

/*This is the C file for the minimax algorithm

Description: The four letter word game involves pathfinding via letter substitution to trap your opponent into a spot at which there are no places to go 
To Do So, I will make use of the minimax algorithm*/


//int minimax(int wordID, int depth, int isMaximizingPlayer, struct HashMapNode** HashMap){
	//First, let's get the list of nodes that we can go to 
	//Of course that leads to the issue of how to store the nodes that we've visited, and tbh I'm thinking int linked list 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	//Otherwise, recursion time 
	//if it is the maximizing player's turn, then we want to return the best move
		//The current maximimum evaluation is going to be -infinity, since we want something higher than that 
		//Go through each child node (how to know which nodes we've gone through (list? O(n).. no thanks (a lot of Hash Maps???)))
		//Loop through each child that connects
			//Set the algorithm evaluation to the minimax, making sure that, when setting the params, the depth goes down by 1, that it is false, and that it is putting in the child ID
			//Set the max eval to the max between the algEval or the current maxEval	
		//Returnt he maximim evaluation
	
	//if it is the minimizing player's turn, then we want to return the min move
		//The current minimum evaluation is going to be +infinity since we want something lower than that
		//Go through each child node (how to know which node's we've gone through? (Create a hash set... explore options))
		//Loop through each child node that connects
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			//Set the min eval to the min between the algEval or the current minEval
		//Return the minimum evaluation


//}


