#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../includes/Minimax.h"
#include "../includes/MinimaxTests.h"

#include "../../structs/includes/IntLinkedList.h"


/*This is for all of the unofficial minimax algorithms
This is entirely for testing purposes. To see if there are potential algorithms that may be better than the minimax
Author: Jordan Driscoll
Date: May 17th, 2021*/


/*********************************

	
	

	MINIMAX BUT IT ADDS UP ALL OF THE CHILDREN NODE, AND MULTIPLIES THEM BY .1 IF IT REACHES DEPTH 0






*********************************/










struct minimaxOutput* minimax_CountAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	//printf("%d\n", depth); 

	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//Then I would also add it into the transposition hash
	//We will place this node in the HahsMap 
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg_CountAtZero(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap, wordSet); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg_CountAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
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
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			//If the depth is equal to 0, we only want to know the number of connections, not go deeper
			if(depth != 0){
				//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
				
				struct minimaxOutput* potential = minimax_CountAtZero(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap, wordSet); 
				
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
				
			}
				//Make sure the number of connections goes up
			numConnections++; 
		}
		//Go to the next node
		currConnection = currConnection->next; 
		
	}
	
	if(depth == 0){
		//Free the absolute evaluation
		free(absEval); 
		markUnused_WordSet(id, wordSet);
		if(numConnections > 0){
			return createOutput(0, numConnections * .1, depth, id); 
		}
		//It also knocks the id off of the HashMap
		return (isMaximizingPlayer == 1) ? createOutput(-1, 0, depth, id) : createOutput(1, 1, depth, id);  
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
		markUnused_WordSet(id, wordSet); 
		//I would also remove it from the transposition hash
		
	}
	
	
	


	//Return the minimum evaluation
	return absEval; 

	
}

































/************************************


	MINIMAX BUT IT SETS THE SCORE TO 50% IF IT REACHES A ZERO THAT STILL HAS CONNECTIONS





*************************************/

struct minimaxOutput* minimax_FiftyFifty(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet){
	//printf("%d\n", depth); 

	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//Then I would also add it into the transposition hash
	//We will place this node in the HahsMap 
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg_FiftyFifty(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap, wordSet); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg_FiftyFifty(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
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
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			//If it's at 0 depth, if there still exists options, it return 50% as score
			if(depth == 0){
				free(absEval); 
				markUnused_WordSet(id, wordSet);
				//It also knocks the id off of the HashMap
				return createOutput(0, .5, 0, id);  
			}
			
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax_FiftyFifty(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap, wordSet); 
			
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
	
	if(depth == 0){
		free(absEval); 
		markUnused_WordSet(id, wordSet);
		//It also knocks the id off of the HashMap
		return (isMaximizingPlayer == 1) ? createOutput(-1, 0, depth, id) : createOutput(1, 1, depth, id);  
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
		markUnused_WordSet(id, wordSet); 
		//I would also remove it from the transposition hash
		
	}
	
	
	


	//Return the minimum evaluation
	return absEval; 

	
}




/********************************************





 MINIMAX BUT AT A DEPTH OF 0 IT IMMEDIATELY SHUTS DOWN







**************************************************/



struct minimaxOutput* minimax_QuitAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
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
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg_QuitAtZero(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap, wordSet); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg_QuitAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
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
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			
		
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax_QuitAtZero(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap, wordSet); 
			
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
		markUnused_WordSet(id, wordSet); 
		//I would also remove it from the transposition hash
		
	}
	
	
	


	//Return the minimum evaluation
	return absEval; 

	
}






/*****************************************************




	CHOOSES THE FIRST VALUE THAT IT CAN FIND, NO MINIMAX





*****************************************************/



int chooseFirst(int id, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	//Take the id, and find the first available option
	struct intList *options = IntToWord_HashMap->array[id]->connectionHeader; 
	//Loop through all of the words that connect
	while(options->next != NULL){
		options = options->next; 
		//If it finds a word who has yet to be found
		if(checkIfUsed_WordSet(options->data, wordSet) == 0){
			return options->data; 
		}
	}
	return -1; 
	
}

int chooseRandom(int id, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
	//The linked list
	struct intList* listHeader = IntToWord_HashMap->array[id]->connectionHeader;
	struct intList* curr = listHeader;  
	//The total number of options
	int totalOptions = IntToWord_HashMap->array[id]->numConnections; 
	//if there is nowhere to connect, the game is lost
	if(totalOptions == 0){
		return -1; 
	}
	//First, choose a random number
	int randID = rand() % totalOptions + 1; 

	int currID = randID; 
	//Walk to that point in the linked list
	
	int i = 0; 
	
	for(i = 0; i < randID; i++){
		curr = curr->next; 
	}
	//Once that number is reached, it will check if it has been taken
	if(checkIfUsed_WordSet(curr->data, wordSet) == 0){
		return curr->data; 
	}
	currID++; 
	//If it has, it will walk until it reaches a null value
	while(currID != randID){
		curr = curr->next; 
		currID++; 
		
		//If it reaches a null value, it will teleport to the front
		if(curr == NULL){
			curr = listHeader->next; 
			currID = 1; 	
		}
		//Once that number is reached, it will check if it has been taken
		if(checkIfUsed_WordSet(curr->data, wordSet) == 0){
			return curr->data; 
		}	
	}
	
	//Once it reaches the original node, that will mean it tried every option, and did not have a choice
	return -1; 
	
	
}










/**************************************************************


	MINIMAX BUT THE BETA SCORE, WHEN DEPTH IS 0, IS DETERMINED BY THE NUMBER OF OPTIONS TO WHICH THE DEPTH CAN CONNECT






*****************************************************************/

struct minimaxOutput* minimax_ZeroOptions(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	if(depth == 0){
		return createOutput(0, .5, 0, id);  
	}
	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//Then I would also add it into the transposition hash
	//We will place this node in the HahsMap 
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg_ZeroOptions(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap, wordSet); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg_ZeroOptions(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
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
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			
		
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax_ZeroOptions(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap, wordSet); 
			
			winPercent += potential->winPercent; 
		
			//Set the min eval to the min between the algEval or the current minEval		
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
		markUnused_WordSet(id, wordSet); 
		//I would also remove it from the transposition hash
		
	}
	
	
	


	//Return the minimum evaluation
	return absEval; 

	
}




















/**************************************************************

		MINIMAX WITHOUT THE BETA VARIABLE BEING CONSIDERED


***************************************************************/


struct minimaxOutput* minimax_NoBeta(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
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
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	return minimaxAlg_NoBeta(id, depth, maxDepth, isMaximizingPlayer, currConnection, IntToWord_HashMap, wordSet); 
	


	
	
	
}


struct minimaxOutput* minimaxAlg_NoBeta(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
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
		if(checkIfUsed_WordSet(currID, wordSet) == 0){
			
		
			//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
			struct minimaxOutput* potential = minimax_NoBeta(currID, depth - 1, maxDepth, (isMaximizingPlayer == 1) ? 0 : 1, IntToWord_HashMap, wordSet); 
			
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

int compareOutput_NoBeta(struct minimaxOutput* curr, struct minimaxOutput* potential, int isMaximizingPlayer){

	//maximizer chooses max value, so if the potential score is greater than the current score. It will want to return 1. 
	if(potential->score > curr->score){
		return 1; 
	}
	else if(potential->score == curr->score){
		int primary = potential->score; 
		//Next, it is necessary to check the depth 
		return compareDepth(curr, potential, primary, isMaximizingPlayer); 
		
	}
	return 0; 
	
}





/*MINIMAX BUT WITHOUT EASY READING METHODS*/


struct minimaxOutput* minimax_Unmethodized(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){

	if(depth == 0){
		return createOutput(0, .5, 0, id); 
	}
	//First, let's get the list of nodes that we can go to 
	struct intList* currConnection = getConnections(id, IntToWord_HashMap); 
	//Avoid the header
	currConnection = currConnection->next; 
	//We will place this node in the HahsMap 
	markUsed_WordSet(id, wordSet); 
	//If the depth is equal to 0, or there are no nodes to go to (how to determine that?)
		//Return the static evaluation of the position
	
	//Otherwise, it's time for recursion  
	//if it is the maximizing player's turn, then we want to return the best move
	if(isMaximizingPlayer == 1){
		//The current maximimum evaluation is going to be -infinity, since we want something higher than that 
		struct minimaxOutput* maxEval = createOutput(-100, 0, -1, -1); 
		//The current id to go with
		int currID; 
		//Integer that keeps track of number of connections
		int numConnections = 0; 
		//Go through each child node
		while(currConnection != NULL){
			currID = currConnection->data; 
			//Make sure that it is not in the hash map
			if(checkIfUsed_WordSet(currID, wordSet) == 0){
				
				//Set the algorithm evaluation to the minimax, making sure that, when setting the params, the depth goes down by 1, that it is false, and that it is putting in the child ID
				struct minimaxOutput* potential = minimax_Unmethodized(currID, depth - 1, maxDepth, 0, IntToWord_HashMap, wordSet); 
				//printf("At %d: %d or %d (max) Choice: %d\n", id, potential->id, maxEval->id, (compareOutput(maxEval, potential) == 1) ? potential->id : maxEval->id); 
				//printf("Choose Between: %d %d. Compare Max: %d, %d. Output: %d\n", maxEval->id, potential->id, maxEval->score, potential->score, compareOutput(maxEval, potential)); 
				//Set the max eval to the max between the algEval or the current maxEval
				if(compareOutput(maxEval, potential, isMaximizingPlayer) == 1){
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
			
			maxEval = createOutput(-1, 0, depth, id); 
		}
		
		if(depth != maxDepth){	
			markUnused_WordSet(id, wordSet);
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
		struct minimaxOutput* minEval = createOutput(100, 0, -1, -1);  
		//The current word being checked
		int currID; 
		//Integer that keeps track of number of connections
		int numConnections = 0; 
		//This is used by the Case B to determine, for case A, what is most likely to win. Case b calculates it, Case A Applies it 
		double winPercent = 0.0; 
		//Go through each child node
		while(currConnection != NULL){
			currID = currConnection->data; 
			
			//Make sure that the word has not already been found in the hash set
			if(checkIfUsed_WordSet(currID, wordSet) == 0){
				
			
				//Set the algorithm evaluation to minimax making usre that when setting the params, the depth goes down by 1, that the isMinimaxPlayer is true, and that it is putting in the child's ID
				struct minimaxOutput* potential = minimax_Unmethodized(currID, depth - 1, maxDepth, 1, IntToWord_HashMap, wordSet); 
				winPercent += potential->winPercent; 
				//printf("\nAt %d: %d or %d (min) Choice: %d\n", id, potential->id, minEval->id, (compareOutput(minEval, potential) == 0) ? potential->id : minEval->id); 
				//printf("Choose Between: %d %d. Compare Min: %d, %d. Output: %d\n", minEval->id, potential->id, minEval->score, potential->score, compareOutput(minEval, potential));
				//Set the min eval to the min between the algEval or the current minEval
				if(compareOutput(minEval, potential, isMaximizingPlayer) == 0){
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
		
		
		winPercent /= (double)numConnections;
		minEval->winPercent = winPercent; 	
		
		//If the number of connections is none, create the minimaxOutput node
		if(numConnections == 0){
			//printf("No Connections\n"); 
			free(minEval); 
			//If there are no connections, the algorithm has, albeit sadly, lost. 
			if(depth == maxDepth){
				return NULL; 
			}
			
			minEval = createOutput(1, 100, depth, id); 
		}
		if(depth != maxDepth){
		
			markUsed_WordSet(id, wordSet); 
		}
		if(depth == 0){
			return createOutput(0, .5, 0, id); 
		}
		if(depth != maxDepth){
			minEval->id = id; 
		} 
		
		//Return the minimum evaluation
		return minEval; 
	}
	
	return NULL;
}

