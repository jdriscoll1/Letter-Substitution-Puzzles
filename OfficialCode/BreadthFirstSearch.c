
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
	
	/*Declares the Queue -- contains which words are to be searched through*/
	bc->Queue = malloc(sizeof(struct word)); 
	bc->Queue->next = NULL;
	bc->Queue->dataMalloc = 0;  
	AddToBack_WordLL(start, bc->Queue, 0);
	
	//This keeps track of the QueueHeader for freeing purposes 
	bc->QueueHeader = bc->Queue; 
	
	return bc; 

};

void Free_BFSComponents(struct BFSComponents* bc, enum FoundWordStorage storageType){
	//Frees the Tree Storage
	Free_TreeStorageNode(bc->ReverseTreeHeader); 
	
	//Frees the Queue
	Free_WordLL(bc->QueueHeader);

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
		bc->Queue = bc->Queue->next; 
		
		//This stores the words that connect to the current word being tested
		struct word *list; 

		if(storageType == TREE_SET){			
			list = linkOutput(bc->Queue->word, HashMap[FirstHashFunction(bc->Queue->word[0])][SecondHashFunction(bc->Queue->word)], bc->TreeHead, TREE_SET, 0); 
		}
		else{
			list = linkOutput(bc->Queue->word, HashMap[FirstHashFunction(bc->Queue->word[0])][SecondHashFunction(bc->Queue->word)], bc->HashSet, HASH_SET, 0); 
		}
		/*The previous connection is the word that connects to all of the word in the Reverse Tree Set*/ 
		bc->prevConnection = Search_TreeStorageNode(bc->Queue->word, bc->ReverseTreeHeader);  
		/*If it is explicitly a word that it is trying to find, then it will go through and try to find the word*/ 
		
		bc->End = Copy_WordLLToTreeStorageNode(bc->ReverseTreeHeader, bc->prevConnection, list, goal, -1); 
		if(bc->End != NULL){
			goalFound = true; 
		}

		
		Copy_WordToWordLL(bc->Queue, list); 
		if(bc->Queue->next == NULL){			  
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

char** BreadthFirstSearch_Distance(char* start, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType){
	//If the number of connections is less than 2, it is pointless. 1? pies->ties. 0. pies->pies -1->???
	if(minConnections < 2){
		printf("MinConnections < 2 [BFS_Distance]"); 
		exit(0); 
	}
	//Instantiates the necessary BFS Components
	struct BFSComponents* bc = init_BFSComponents(start, storageType);
	//Initalize the game Components
	bool goalFound = false; 
	
	while(goalFound == false){
		
		bc->Queue = bc->Queue->next;
		
		struct word *list; 
     
		if(storageType == TREE_SET){			
			list = linkOutput(bc->Queue->word, HashMap[FirstHashFunction(bc->Queue->word[0])][SecondHashFunction(bc->Queue->word)], bc->TreeHead, TREE_SET, 0); 
		}
		else{
			list = linkOutput(bc->Queue->word, HashMap[FirstHashFunction(bc->Queue->word[0])][SecondHashFunction(bc->Queue->word)], bc->HashSet, HASH_SET, 0); 
		}
		/*The previous connection is the word that connects to all of the word in the Reverse Tree Set*/ 
		bc->prevConnection = Search_TreeStorageNode(bc->Queue->word, bc->ReverseTreeHeader);  
		/*If it is explicitly a word that it is trying to find, then it will go through and try to find the word*/ 
		
		bc->End = Copy_WordLLToTreeStorageNode(bc->ReverseTreeHeader, bc->prevConnection, list, NULL, minConnections); 
		if(bc->End != NULL){
			goalFound = true; 
		}
     
		
		Copy_WordToWordLL(bc->Queue, list); 
		if(bc->Queue->next == NULL){			  
		
			printf("\nThere are no words %d connections away that connect with %s\n", minConnections - 1, start); 
			goalFound = -1;
		}
		
		Free_WordLL(list);	
		
	
		
	}
	char** path; 
	//This allocates the 2D array, num letters has to be plus one, because of weird indexing, num letters has to be plus one to, for sake of null terminator
	path = (char**)Allocate_2DArray(minConnections + 1, numLetters + 1); 
	//Convert the TreeStorageList To Array
	Convert_TreeStorageNodeTo2DArray(path, bc->End, minConnections);
	Free_BFSComponents(bc, storageType);
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








