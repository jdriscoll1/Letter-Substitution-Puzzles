
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>


#include "ArrayList.h"
#include "HashMap.h"
#include "HashFunctions.h"
#include "HashSet.h"
#include "GenericLinkedListNode.h"
#include "GameFunctions.h"
#include "BreadthFirstSearch.h"


#include "Arrays.h"
#define true 1
#define false 0
typedef int bool; 

extern int numLetters; 

struct BFSComponents* init_BFSComponents(char* start, enum FoundWordStorage storageType){
	struct BFSComponents* bc = malloc(sizeof(struct BFSComponents)); 
 

	
	//If we want the tree, we allocate it  
	if(storageType == TREE_SET){
		bc->TreeHead = Allocate_TreeSet(start);
	}
	//If we want the Hash Set, we allocate it 
	else{
		bc->HashSet = AllocateHashSet();
		AddToHashSet(start, bc->HashSet); 

	}
	
	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	bc->ReverseTreeHeader = Allocate_TreeStorageNode(start); 
	//This method keeps track of where the previous connection is. Example, pies pins pier --> pies (Keeps this at O(1) time
	bc->prevConnection = bc->ReverseTreeHeader; 

	
	return bc; 

};

void Free_BFSComponents(struct BFSComponents* bc, enum FoundWordStorage storageType){
	//Frees the Tree Storage
	Free_TreeStorageNode(bc->ReverseTreeHeader); 
	
	/*Free The Tree Set*/
	if(storageType == TREE_SET){
	
		Free_TreeSet(bc->TreeHead->start);
		free(bc->TreeHead);  
	}
	//Frees the Hash Set
	else{
		Free_HashSet(bc->HashSet); 
	}
	
	//Frees the structure
	free(bc); 
	
} 

struct word* BreadthFirstSearch_Dest(char* start, char* goal, struct wordConnections **(*HashMap), enum FoundWordStorage storageType ){
	 
	//If the start word and goal word are equal, it returns 0
	if(strcmp(start,goal) == 0){
		printf("EqualWords_[BreadthFirstSearch_Dest]\n"); 
		exit(0); 
	}
	
	struct BFSComponents* bc = init_BFSComponents(start, storageType);
	bool goalFound = false; 
	
	
	//Until the goal word is found it is going to spread out through every connection, and all of those connections until it finds the goal word
	while(goalFound == false){
		bc->prevConnection = bc->prevConnection->next; 
		//This stores the words that connect to the current word being tested
		struct word *list; 
		
		if(storageType == TREE_SET){			
			list = linkOutput(bc->prevConnection->word, HashMap[FirstHashFunction(bc->prevConnection->word[0])][SecondHashFunction(bc->prevConnection->word)], bc->TreeHead, TREE_SET, 0); 
		}
		else{
			list = linkOutput(bc->prevConnection->word, HashMap[FirstHashFunction(bc->prevConnection->word[0])][SecondHashFunction(bc->prevConnection->word)], bc->HashSet, HASH_SET, 0); 
		}
		/*The previous connection is the word that connects to all of the word in the Reverse Tree Set*/ 
		
		/*If it is explicitly a word that it is trying to find, then it will go through and try to find the word*/ 
		
		bc->End = Copy_WordLLToTreeStorageNode(bc->ReverseTreeHeader, bc->prevConnection, list, goal, -1); 
		if(bc->End != NULL){
			goalFound = true; 
		}

		
		if(bc->prevConnection->next == NULL){			  
			printf("\n%s cannot connect with %s\n", start, goal);
			goalFound = -1;
		}
		Free_WordLL(list);	
		
		
		
		
		
	}
	struct word* path; 
	path = malloc(sizeof(struct word)); 
	path->dataMalloc = 0; 
	path->next = NULL; 
	path = Convert_TreeStorageNodeToWordLL(path, bc->End);
	
	//Frees the structure
	Free_BFSComponents(bc, storageType); 
	
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
char** BreadthFirstSearch_Distance(char* start, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType){
	//If the number of connections is less than 2, it is pointless. 1? pies->ties. 0. pies->pies -1->???
	if(minConnections < 2){
		printf("MinConnections < 2 [BFS_Distance]"); 
		exit(0); 
	}
	//Instantiates the necessary BFS Components
	struct BFSComponents* bc = init_BFSComponents(start, storageType);
	bc->End = bc->prevConnection->next; 
	//This is the array list that stores the words that are options
	struct arrayList* options = init_ArrayList(20, 5, TSN); 
	//Initalize the game Components
	bool goalFound = false;
	
	//Is it possible to connect this far out
	bool isPossible = false;  
	
	while(goalFound == false){
		bc->prevConnection = bc->prevConnection->next;
		
		//This is the current list of words that can be connected
		struct word *list; 
     	
     	//This grabs all of the current connections
		if(storageType == TREE_SET){			
			list = linkOutput(bc->prevConnection->word, HashMap[FirstHashFunction(bc->prevConnection->word[0])][SecondHashFunction(bc->prevConnection->word)], bc->TreeHead, TREE_SET, 0); 
		}
		else{
			list = linkOutput(bc->prevConnection->word, HashMap[FirstHashFunction(bc->prevConnection->word[0])][SecondHashFunction(bc->prevConnection->word)], bc->HashSet, HASH_SET, 0); 
		}
		
		
		//LIGHTBULB: Instead of having a link output, which is based on the queue, I have that occur during the Tree Stroage Node Copying. So, instead of being given the list. It'll be given the word, and it'll do link output while it which'll save it a a timecomplexity of 2E each loopage 
		/*If it is explicitly a word that it is trying to find, then it will go through and try to find the word*/ 
		
		bc->End = Copy_WordLL_Onto_TreeStorageNode_Distance(bc->End, bc->prevConnection, list, options, minConnections); 
		
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
		
		Free_WordLL(list);	
		
		
		
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
	Free_BFSComponents(bc, storageType);
	free_ArrayList(options); 
	
	return (goalFound == -1)?NULL:path;

}


/* This function will accept a word as an input, and output all of the connecting values */
struct word *linkOutput(char* wordInput, struct wordConnections *header, void* FoundStorage, enum FoundWordStorage storageType, int readOnly){
	/* Create the linked list into which I put the words */
	struct word *wordOutput = malloc(sizeof(struct word)); 
	wordOutput->next = NULL; 
	wordOutput->dataMalloc = 0; 
	header = header->nextRow; 
	/* Loop through the array until I find the correct word */ 
	while(strcmp(header->word, wordInput) != 0){
		header = header->nextRow; 
	}

	while(header->nextColumn != NULL){
		header = header->nextColumn; 
		/*If the word hasn't been added
		if it's a tree set that we're using, then it uses the tree set methods, otherwise it uses the hash set methods
		*/
		if(storageType == TREE_SET){
		
			if(Search_TreeSet(header->word, ((struct DummyHeadNode*)(FoundStorage))->start, WORD) == 0){
				if(readOnly == 0){	
					AddNode_TreeSet(header->word, FoundStorage, ((struct DummyHeadNode*)(FoundStorage))->start, DUMMY,  WORD);
				}
				AddToBack_WordLL(header->word, wordOutput, 0);  		
			}
		}
		
		else if(storageType == HASH_SET){
			if(Search_HashSet(header->word, FoundStorage) == 0){
				/*if it's read only, then we don't want to add it to the Hash Set, we just want to read from the hash set*/ 
				if(readOnly == 0){
					AddToHashSet(header->word, FoundStorage);
				}
				AddToBack_WordLL(header->word, wordOutput, 0);  	
			}
		}
		
		else if(storageType == NEITHER_SET){
			AddToBack_WordLL(header->word, wordOutput, 0); 
		}
		
	}
	/* Let's make a find method, 'cause quite frankly, I'm going to need that later anyways... */	
	/* Print the Linked List */
	/* Return the Linked List */
	return wordOutput; 
	
	
	
	
	
}








