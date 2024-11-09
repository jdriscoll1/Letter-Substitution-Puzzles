#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "../includes/BreadthFirstSearch.h"


#include "../../structs/includes/GenericLinkedListNode.h"
#include "../../structs/includes/ArrayList.h"
#include "../../structs/includes/IntLinkedList.h"
#include "../../structs/includes/HashMap.h"


#include "../../flwp/includes/Hints.h"
#include "../../flwp/includes/UserInput.h"
#include "../../flwp/includes/GameFunctions.h"

extern int numLetters; 




struct BFSComponents* init_BFSComponents(int start, struct WordSet* wordSet){

	struct BFSComponents* bc = malloc(sizeof(struct BFSComponents)); 

	//Add the word to the hash set

	markUsed_WordSet(start, wordSet);

	
	
	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	bc->ReverseTreeHeader = Allocate_TreeStorageNode(start, 0); 
	//This method keeps track of where the previous connection is. Example, pies pins pier --> pies (Keeps this at O(1) time
	bc->prevConnection = bc->ReverseTreeHeader; 
	bc->End = bc->ReverseTreeHeader; 
	
	return bc; 

};

void Free_BFSComponents(struct BFSComponents* bc, struct WordSet *wordSet){
	//Frees the Tree Storage
	Free_TreeStorageNode(bc->ReverseTreeHeader); 
	

	reset_WordSet(wordSet); 
	
	//Frees the structure
	free(bc); 
	
} 


/*Things to keep in mind: 
- assuming minConnections
- Go past 8 by 1, that is to 9. Store all of them that are equal to 8
- Do I really need the whole path? 
- So I need an array list 
- Once it's hit 8, there has to be an is valid variable
- If it's null and its isValid == 0, then it has to stop
- If it's null and its isValid == 1, then it from there has to choose (meaning that 8 connections was it's max point)
*/ 
struct BFSResults BreadthFirstSearch_Distance(int start, int minConnections, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){

	//If the number of connections is less than 2, it is pointless. 1? pies->ties. 0. pies->pies -1->???
	if(minConnections < 2){
		printf("MinConnections < 2 [BFS_Distance]"); 
		exit(0); 
	}

	//This array determines how much an array list should start and grow. [minConnections - 2][0] = initSize [minConnections - 2][1] = move size
	//These numbers were found by taking the mean, and the avg of the mean and the max
	int arraySize[][2] = {{43, 61},{152, 141}, {351, 183}, {516, 150}, {427, 188}, {277, 239}, {164, 300}, {83, 21}, {39, 332},{18, 361}, {15, 200}, {10, 354}, {8, 143}, {2, 28}, {2, 4}, {2, 4}}; 

	int arrListInitSize = (minConnections < 13) ? arraySize[minConnections - 2][0] : 5; 

	int arrListMoveSize = (minConnections < 13) ? arraySize[minConnections - 2][1] : 5; 
	
	// Instantiates the necessary BFS Components
	struct BFSComponents* bc = init_BFSComponents(start, wordSet);
	bc->End = bc->prevConnection->next; 

	//This is the array list that stores the words that are options
	struct arrayList* options = init_ArrayList(50, 50, TSN); 
	//Initalize the game Components
	bool goalFound = false;
	
	while(goalFound == false){
	
		bc->prevConnection = bc->prevConnection->next;

		bc->End = AddToTreeStorage_BFS(bc, minConnections, options, IntToWord_HashMap, wordSet); 
		
		
		//If it cannot connect as far out as intended
		if(bc->prevConnection->next == NULL){			  
			//It cannot return or else there will be memory leaks 
			goalFound = -1;
		}
		
		 
		//If it sees that the current depth is > minConnections it'll return NULL
		//With this in mind, this means that it has acheived the current depth 
		
		if(bc->End == NULL){
			goalFound = true; 
		}

	}

	//This is the path from start to end 
	int goal; 

	struct BFSResults results; 
	results.list = (goalFound == -1) ? NULL :  options; 
	results.dataStorage  = (goalFound == -1) ? NULL :bc; 
	return results; 

}

int BreadthFirstSearch_Distance_Goal(int start, int minConnections, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	//If the number of connections is less than 2, it is pointless. 1? pies->ties. 0. pies->pies -1->???
	if(minConnections < 2){
		printf("MinConnections < 2 [BFS_Distance]"); 
		exit(0); 
	}
	//This array determines how much an array list should start and grow. [minConnections - 2][0] = initSize [minConnections - 2][1] = move size
	//These numbers were found by taking the mean, and the avg of the mean and the max
	int arraySize[][2] = {{43, 61},{152, 141}, {351, 183}, {516, 150}, {427, 188}, {277, 239}, {164, 300}, {83, 21}, {39, 332},{18, 361}, {15, 200}, {10, 354}, {8, 143}, {2, 28}, {2, 4}, {2, 4}}; 
	int arrListInitSize = (minConnections < 13) ? arraySize[minConnections - 2][0] : 5; 
	int arrListMoveSize = (minConnections < 13) ? arraySize[minConnections - 2][1] : 5; 
	
	//Instantiates the necessary BFS Components
	struct BFSComponents* bc = init_BFSComponents(start, wordSet);
	bc->End = bc->prevConnection->next; 
	//This is the array list that stores the words that are options
	struct arrayList* options = init_ArrayList(arrListInitSize, arrListMoveSize, TSN); 
	//Initalize the game Components
	bool goalFound = false;
	
	while(goalFound == false){
	
		bc->prevConnection = bc->prevConnection->next;

		bc->End = AddToTreeStorage_BFS(bc, minConnections, options, IntToWord_HashMap, wordSet); 
		
		
		//If it cannot connect as far out as intended
		if(bc->prevConnection->next == NULL){			  
			//No need to let the user know this lol
		
			//It cannot return or else there will be memory leaks 
			goalFound = -1;
		}
		
		 
		//If it sees that the current depth is > minConnections it'll return NULL
		//With this in mind, this means that it has acheived the current depth 
		
		if(bc->End == NULL){
			goalFound = true; 
		}
	}
	//This is the path from start to end 
	int goal; 
	
	//If the goal isn't found, there will be no options to choose from 
	if(goalFound != -1){
		 //This is the total number of options
		int numOptions = options->currPrecision; 
		
		//This is what the computer chooses
		int choiceIndex = rand() % numOptions;
		//This the node that has been chosen
		struct TreeStorageNode* chosenNode = ((struct TreeStorageNode**)options->list)[choiceIndex]; 
		goal = chosenNode->id; 
	
	}	
	Free_BFSComponents(bc, wordSet);
	free_ArrayList(options); 

	return (goalFound == -1)?-1:goal;

}



struct TreeStorageNode* AddToTreeStorage_Dist_BFS(struct BFSComponents *bc, int goal, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet){
	//The word whose connections we're going to find, and add to the TreeStorageNode
	int baseWord = bc->prevConnection->id; 
	
	struct intList* newWords = getConnections(baseWord, IntToWord_HashMap);

	//Then, while there are still connections in the list,
	while(newWords->next != NULL){
		//We want to move off of the header of the 2Dconnection 
		newWords = newWords->next; 
		int currWord = newWords->data; 
	
		if(checkIfUsed_WordSet(currWord, wordSet) == 0){
			bc->End = Add_TreeStorageNode(currWord, bc->prevConnection, bc->End, 0); 
			markUsed_WordSet(currWord, wordSet); 
		}
	
		if(currWord == goal){
			return bc->End; 
		}
	} 
	
	//So we want to be returned the very last node every single time, 
	return bc->End; 
}



//This Adds the Upcoming Words Into the Tree Storage
struct TreeStorageNode* AddToTreeStorage_BFS(struct BFSComponents *bc, int minConnections, struct arrayList *options, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet){
	//The word whose connections we're going to find
	int baseWord = bc->prevConnection->id; 
	//Variable prev depth + 1 = currDepth -- How far out we immediately are
	int currDepth = bc->prevConnection->depth + 1;
	
	//If it has passed it, I will return NULL immediately -- Returns NULL because if it returns the currEnd, it would still be at a depth of minConnections, and would try again
	if(currDepth > minConnections){
		return NULL; 
	} 

	struct intList* newWords = getConnections(baseWord, IntToWord_HashMap);

	//Then, while the link output still words in the list,
	while(newWords->next != NULL){
		//We want to move off of the header of the 2Dconnection 
		newWords = newWords->next; 
		int currWord = newWords->data; 
		//This is the tree storage node that we just added. It is the most recently added connection, which is also the furthest out 
		//Curr End is now at the end

		//Print_TreeStorageNode(bc->ReverseTreeHeader); 
		
		if(checkIfUsed_WordSet(currWord, wordSet) == 0){
	 
			bc->End = Add_TreeStorageNode(currWord, bc->prevConnection, bc->End, 0); 
			markUsed_WordSet(currWord, wordSet); 
			if(currDepth == minConnections){
				add_ArrayList(bc->End, options, TSN); 			
			}
		}
	}
	//So we want to be returned the very last node every single time, 
	return bc->End; 
}









