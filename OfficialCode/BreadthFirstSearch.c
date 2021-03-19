#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>


#include "HashMap.h"
#include "HashFunctions.h"
#include "HashSet.h"
#include "GenericLinkedListNode.h"
#include "GameFunctions.h"
#include "BreadthFirstSearch.h"
#include "Arrays.h"
#include "ArrayList.h"
#include "Hints.h"
#include "UserInput.h"
#include "HashMap2.h"

#define true 1
#define false 0
typedef int bool; 

extern int numLetters; 




struct BFSComponents* init_BFSComponents(char* start){
	struct BFSComponents* bc = malloc(sizeof(struct BFSComponents)); 
 

	

	//the Hash Set, we allocate it 

	bc->HashSet = AllocateHashSet();
	AddToHashSet(start, bc->HashSet, 0); 

	
	
	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	bc->ReverseTreeHeader = Allocate_TreeStorageNode(start); 
	//This method keeps track of where the previous connection is. Example, pies pins pier --> pies (Keeps this at O(1) time
	bc->prevConnection = bc->ReverseTreeHeader; 
	bc->End = bc->ReverseTreeHeader; 
	
	return bc; 

};

void Free_BFSComponents(struct BFSComponents* bc){
	//Frees the Tree Storage
	Free_TreeStorageNode(bc->ReverseTreeHeader); 
	

	Free_HashSet(bc->HashSet); 
	
	
	//Frees the structure
	free(bc); 
	
} 

struct word* BreadthFirstSearch_Dest_WordLL(char* start, char* goal, struct DummyHeadNode **(*HashMap)){
	 
	//If the start word and goal word are equal, it returns 0
	if(strcmp(start,goal) == 0){
		printf("EqualWords_[BreadthFirstSearch_Dest]\n"); 
		exit(0); 
	}
	
	struct BFSComponents* bc = init_BFSComponents(start);
	bool goalFound = false; 

	
	//Until the goal word is found it is going to spread out through every connection, and all of those connections until it finds the goal word
	while(goalFound == false){
 
		bc->prevConnection = bc->prevConnection->next;
		
		bc->End = AddToTreeStorage_Dist_BFS(bc, goal, HashMap); 
		if(strcmp(bc->End->word, goal) == 0){
			goalFound = true; 
		}

		
		if(bc->prevConnection == NULL){			  
			printf("\n%s cannot connect with %s\n", start, goal);
			goalFound = -1;
		}
		
		//The previous connection moves forward one, going to the next word whose connections should be searched
		
		
		
	}
	struct word* path; 
	path = malloc(sizeof(struct word)); 
	path->dataMalloc = 0; 
	path->next = NULL; 
	path = Convert_TreeStorageNodeToWordLL(path, bc->End);
	
	//Frees the structure
	Free_BFSComponents(bc); 
	
	return (goalFound == -1)?NULL:path; 
	
	 
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
char** BreadthFirstSearch_Distance(char* start, int minConnections, struct DummyHeadNode **(*HashMap)){
	//If the number of connections is less than 2, it is pointless. 1? pies->ties. 0. pies->pies -1->???
	if(minConnections < 2){
		printf("MinConnections < 2 [BFS_Distance]"); 
		exit(0); 
	}
	//This array determines how much an array list should start and grow. [minConnections - 2][0] = initSize [minConnections - 2][1] = move size
	//These numbers were found by taking the mean, and the avg of the mean and the max
	int arraySize[][2] = {{43, 61},{152, 141}, {351, 183}, {516, 150}, {427, 188}, {277, 239}, {164, 300}, {83, 21}, {39, 332},{18, 361}, {15, 200}, {10, 354}, {8, 143}, {2, 28}, {2, 4}, {2, 4}}; 
	
	//Instantiates the necessary BFS Components
	struct BFSComponents* bc = init_BFSComponents(start);
	bc->End = bc->prevConnection->next; 
	//This is the array list that stores the words that are options
	struct arrayList* options = init_ArrayList(arraySize[minConnections - 2][0], arraySize[minConnections - 2][1], TSN); 
	//Initalize the game Components
	bool goalFound = false;
	
	//Is it possible to connect this far out
	bool isPossible = false;  
	
	while(goalFound == false){
	
		bc->prevConnection = bc->prevConnection->next;

		
		
		bc->End = AddToTreeStorage_BFS(bc, minConnections, options, HashMap); 
		
		//If it sees that the current depth is > minConnections it'll return NULL
		//With this in mind, this means that it has acheived the current depth 
		if(bc->End == NULL){
			goalFound = true; 
		}
		
		//If it cannot connect as far out as intended
		if(bc->prevConnection->next == NULL){			  
			printf("\nThere are no words %d connections away that connect with %s\n", minConnections, start); 
			//It cannot return or else there will be memory leaks 
			goalFound = -1;
		}
		
		//Free_WordLL(list);	
		
		
		
	}
	//This is the path from start to end 
	char** path; 
	
	//If the goal isn't found, there will be no options to choose from 
	if(goalFound != -1){
		 //This is the total number of options
		int numOptions = options->currPrecision; 
		
		//This is what the computer chooses
		int choiceIndex = rand() % numOptions;
	 
		//This the node that has been chosen
		struct TreeStorageNode* chosenNode = ((struct TreeStorageNode**)options->list)[choiceIndex]; 
		
		//This allocates the 2D array, num letters has to be plus one, because of weird indexing, num letters has to be plus one to, for sake of null terminator
		path = (char**)Allocate_2DArray(minConnections + 1, numLetters + 1); 
		
		//Convert the TreeStorageList To Array
		Convert_TreeStorageNodeTo2DArray(path, chosenNode, minConnections);
	}	
	Free_BFSComponents(bc);
	free_ArrayList(options); 
	
	return (goalFound == -1)?NULL:path;

}



char* BreadthFirstSearch_Distance_Goal(char* start, int minConnections, struct DummyHeadNode **(*HashMap)){
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
	struct BFSComponents* bc = init_BFSComponents(start);
	bc->End = bc->prevConnection->next; 
	//This is the array list that stores the words that are options
	struct arrayList* options = init_ArrayList(arrListInitSize, arrListMoveSize, TSN); 
	//Initalize the game Components
	bool goalFound = false;
	
	//Is it possible to connect this far out
	bool isPossible = false;  
	
	while(goalFound == false){
	
		bc->prevConnection = bc->prevConnection->next;
	
		bc->End = AddToTreeStorage_BFS(bc, minConnections, options, HashMap); 
		 
		//If it sees that the current depth is > minConnections it'll return NULL
		//With this in mind, this means that it has acheived the current depth 
		if(bc->End == NULL){
			goalFound = true; 
		}
		
		//If it cannot connect as far out as intended
		if(bc->prevConnection->next == NULL){			  
			//No need to let the user know this lol
		
			//It cannot return or else there will be memory leaks 
			goalFound = -1;
		}
		
	
		
		
	}
	//This is the path from start to end 
	char* goal; 
	
	//If the goal isn't found, there will be no options to choose from 
	if(goalFound != -1){
		 //This is the total number of options
		int numOptions = options->currPrecision; 
		
		//This is what the computer chooses
		int choiceIndex = rand() % numOptions;

		//This the node that has been chosen
		struct TreeStorageNode* chosenNode = ((struct TreeStorageNode**)options->list)[choiceIndex]; 
		goal = malloc(numLetters + 1); 
		
		safeStrcat(&goal, (const char*) chosenNode->word, numLetters + 1, numLetters, 0);  
	
	}	
	Free_BFSComponents(bc);
	free_ArrayList(options); 

	return (goalFound == -1)?NULL:goal;

}







int BreadthFirstSearch_DistanceOptions(char* start, int minConnections, struct DummyHeadNode **(*HashMap)){
	//If the number of connections is less than 2, it is pointless. 1? pies->ties. 0. pies->pies -1->???
	if(minConnections < 2){
		printf("MinConnections < 2 [BFS_Distance]"); 
		return 0; 
	}
	//Instantiates the necessary BFS Components
	struct BFSComponents* bc = init_BFSComponents(start);
	bc->End = bc->prevConnection->next; 
	//This is the array list that stores the words that are options
	struct arrayList* options = init_ArrayList(20, 5, TSN); 
	//Initalize the game Components
	bool goalFound = false;
	
	//Is it possible to connect this far out
	bool isPossible = false;  
	
	while(goalFound == false){
		bc->prevConnection = bc->prevConnection->next;


		bc->End = AddToTreeStorage_BFS(bc, minConnections, options, HashMap); 
		
		//If it sees that the current depth is > minConnections it'll return NULL
		//With this in mind, this means that it has acheived the current depth 
		if(bc->End == NULL){
			goalFound = true; 
		}
		
		//If it cannot connect as far out as intended
		if(bc->prevConnection->next == NULL){			  
			//It cannot return yet or else there will be memory leaks 
			goalFound = -1;
		}
		
	
		
		
		
	}
	int output = options->currPrecision; 

	//Frees everything
	Free_BFSComponents(bc);
	free_ArrayList(options); 

	return output; 

}





struct TreeStorageNode* AddToTreeStorage_Dist_BFS(struct BFSComponents *bc, char* goal,  struct DummyHeadNode **(*HashMap)){
	//The word whose connections we're going to find, and add to the TreeStorageNode
	char* baseWord = bc->prevConnection->word; 
 	int currDepth = bc->prevConnection->depth + 1; 
	
	struct word* newWords = getList(baseWord, HashMap);

	//Then, while there are still connections in the list,
	while(newWords->next != NULL){
		//We want to move off of the header of the 2Dconnection 
		newWords = newWords->next; 
		char* currWord = newWords->word; 
		
		
		
		
	
		if(Search_HashSet(currWord, bc->HashSet) == 0){
			bc->End = Add_TreeStorageNode(currWord, bc->prevConnection, bc->End, currDepth); 
			AddToHashSet(currWord, bc->HashSet, 0); 
		}
		if(strcmp(currWord, goal) == 0){
			return bc->End; 
		}
	} 
	
		
		


		
	
	//So we want to be returned the very last node every single time, 
	return bc->End; 


	
	
}



//This Adds the Upcoming Words Into the Tree Storage
struct TreeStorageNode* AddToTreeStorage_BFS(struct BFSComponents *bc, int minConnections, struct arrayList *options, struct DummyHeadNode **(*HashMap)){
	//The word whose connections we're going to find
	char* baseWord = bc->prevConnection->word; 
	//Variable prev depth + 1 = currDepth -- How far out we immediately are
	int currDepth = bc->prevConnection->depth + 1;
	
	//If it has passed it, I will return NULL immediately -- Returns NULL because if it returns the currEnd, it would still be at a depth of minConnections, and would try again
	if(currDepth > minConnections){
		return NULL; 
	} 

	//We want to get the current word's location in the HashMap
	//First, we have to get the 2D Linked List that has all of the connections of the letter

	struct word* newWords = getList(baseWord, HashMap);

	//Yo, don't forget to add teh Tree Set, DUDE!
	//Then, while the link output still words in the list,
	while(newWords->next != NULL){
		//We want to move off of the header of the 2Dconnection 
		newWords = newWords->next; 
		char* currWord = newWords->word; 
		//This is the tree storage node that we just added. It is the most recently added connection, which is also the furthest out 
		//Curr End is now at the end


		

		if(Search_HashSet(currWord, bc->HashSet) == 0){
			bc->End = Add_TreeStorageNode(currWord, bc->prevConnection, bc->End, currDepth); 
			AddToHashSet(currWord, bc->HashSet, 0); 
			if(currDepth == minConnections){
				add_ArrayList(bc->End, options, TSN); 			
			}
		}

		
			//If currDepth == minConnections, I will add it to the arrayList list
		


		
	}
	//So we want to be returned the very last node every single time, 
	return bc->End; 
}









