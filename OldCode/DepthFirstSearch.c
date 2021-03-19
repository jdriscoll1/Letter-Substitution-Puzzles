#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DepthFirstSearch.h"
#include "TreeStorageNode.h"
#include "HashMap.h"
#include "HashSet.h"
#include "HashFunctions.h"



/*Depth First Search*/
/*This is essentially the lobby for the depth first search. It's the room before it goes into the hallway and things get going.
It just sets allocates some memory locations, sets some variables, and frees everything after the main search happened.*/ 
struct word* DFS_Lobby(char* start, char* goal, struct wordConnections **(*HashMap)){
	if(strcmp(start, goal) == 0){
		printf("Those are the same words, moron.");
		return NULL;  
	}
	/*Gotta make a hash set*/
	struct word **(*HashSet) = AllocateHashSet(); 
	/*The Tree Storage Linked List, Stores Which Words Connect To Which, that is, if pies -> ties lies dies. It'll remember that, so when asking for the connections, it can give them
	cont) it is necessary to call pies now, because only its connections will be added. If I didn't add it now, pies would never get added*/  
	struct TreeStorageNode *treeStorage = Allocate_TreeStorageNode(start); 
	
	/*Hey! It actually searches for the word with depth first search
	Depth First Search --> Goes Down One Trail, and doesn't look back until it reaches null*/ 
	struct TreeStorageNode *last = DFS(start, goal, HashMap, HashSet, treeStorage);
	
	if(last == NULL){
		printf("%s couldn't connect to %s", start, goal); 
		return NULL; 
	}
	 
	//struct TreeStorageNode *last = Search_TreeStorageNode(goal, treeStorage); 
	/*This is the linked list that stores the connections from the start to the goal*/ 
	struct word *connection = malloc(sizeof(struct word)); 
	connection->dataMalloc = 0; 
	connection->next = NULL; 
	/*Goes recursively through the tree storage and adds the connections to the connection linked list*/ 
	connection = Convert_TreeStorageNodeToWordLL(connection, last);  
	Free_TreeStorageNode(treeStorage); 
	Free_HashSet(HashSet);
	return connection; 
	
}
struct TreeStorageNode* DFS(char* start, char* goal, struct wordConnections **(*HashMap), struct word **(*HashSet), struct TreeStorageNode *treeStorage){
		/*Adds the word to the hash set such that it can't be found again'*/ 
		AddToHashSet(start, HashSet, 0); 
		
		/*First, we create a list that stores all of the words*/
		struct word* list =  linkOutput_DFS(start, HashMap[FirstHashFunction(start[0])][SecondHashFunction(start)], HashSet, HASH_SET, 1); 
		struct word* listHeader = list; 

		//finds the location of the word in the tree storage node 
		struct TreeStorageNode *prevConnection = Search_TreeStorageNode(start, treeStorage);
		/*This copies the word that connect to the method's word to the tree storage node
		*@End --> If it is null, it means it did not find the goal in the list, if it is not null, it did find the word in the list
		*@param treeStorage --> The Tree Storage Node
		*@param prevConnection --> the location of the word that all the connections directly connect to in the tree set. 
		*cont) That is to say --> the location of the word that was called in this method in the tree storage list
		*@param list --> the words that connect to the word called into the method
		*cont) These words will be added to the back of the Tree Storage Linked List
		@param goal --> What is trying to be found. 
		*cont) It is necessary to add it here, because if it is found, then it will return itself as a node*/ 
		struct TreeStorageNode *End = Copy_WordLLToTreeStorageNode(treeStorage, prevConnection, list, goal, -1);  
		if(End != NULL){
			AddToHashSet(goal, HashSet, 0); 
			Free_WordLL(listHeader); 
			return End; 
		}
		/*Then, we go away from the list's header'*/ 
		list = list->next; 
		/*We add the current word to the hash set*/ 
	
	
		/*if the list is not equal to null, we can extrapolate data from it*/ 
	
		  
		
			//	printf("Word: %s, ", nextWord); 
		while(list != NULL){
			struct TreeStorageNode* finalConnection = DFS(list->word, goal, HashMap, HashSet, treeStorage); 
			if(finalConnection != NULL){
				Free_WordLL(listHeader); 
				return finalConnection; 
			}
			list = list->next; 	
		}

		//printf("start: %s", start); 
		Free_WordLL(listHeader); 
}



/* This function will accept a word as an input, and output all of the connecting values */
struct word *linkOutput_DFS(char* wordInput, struct wordConnections *header, void* FoundStorage, int readOnly){
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
					AddToHashSet(header->word, FoundStorage, 0);
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
