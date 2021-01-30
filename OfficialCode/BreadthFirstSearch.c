
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>


//#include "WordLinkedList.h"


#include "HashMap.h"
#include "HashFunctions.h"

//#include "2DLinkedList.h"
#include "HashSet.h"
#include "GenericLinkedListNode.h"

#include "GameFunctions.h"
#include "BreadthFirstSearch.h"

#include "Arrays.h"
#define true 1
#define false 0
typedef int bool; 

extern int numLetters; 

void* BreadthFirstSearch(char* start, char* goal, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType){

	/*If the words are the same, or the connection is less than 2*/
	if(minConnections < 2){
		if(minConnections == -1){
		
			if(strcmp(start,goal) == 0){
				printf("Those are the same word\n"); 
				return 0; 
			}
		}
		else{
		
			printf("Cannot connect to anything"); 
			return 0;
		}
	}
	//Boolean that determines if the goal has been found
	bool goalFound = false; 
	/*Allocate the Tree Set -- This keeps track of the words already found*/
	
	struct DummyHeadNode *TreeHead; 
	struct word **(*HashSet); 
	if(storageType == TREE_SET){
		TreeHead = Allocate_TreeSet(start);
	}
	else{
		HashSet = AllocateHashSet();
		AddToHashSet(start, HashSet); 

	}
	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	struct TreeStorageNode *ReverseTreeHeader = Allocate_TreeStorageNode(start); 
	struct TreeStorageNode *prevConnection = ReverseTreeHeader; 
	
	/*Declares the Queue -- contains which words are to be searched through*/
	struct word *Queue = malloc(sizeof(struct word)); 
	Queue->next = NULL;
	Queue->dataMalloc = 0;  
	AddToBack_WordLL(start, Queue, 0); 
	/*This is a temporary node*/
	struct word *QueueHeader = Queue; 
	/*The last word of the queue*/
	struct TreeStorageNode *End; 
	
	/*While the goal isn't found*/
	while(goalFound == false){
		
		Queue = Queue->next;
		
		struct word *list; 

		if(storageType == TREE_SET){			
			list = linkOutput(Queue->word, HashMap[FirstHashFunction(Queue->word[0])][SecondHashFunction(Queue->word)], TreeHead, TREE_SET, 0); 
		}
		else{
			list = linkOutput(Queue->word, HashMap[FirstHashFunction(Queue->word[0])][SecondHashFunction(Queue->word)], HashSet, HASH_SET, 0); 
		}
		/*The previous connection is the word that connects to all of the word in the Reverse Tree Set*/ 
		prevConnection = Search_TreeStorageNode(Queue->word, ReverseTreeHeader);  
		/*If it is explicitly a word that it is trying to find, then it will go through and try to find the word*/ 
		
		End = Copy_WordLLToTreeStorageNode(ReverseTreeHeader, prevConnection, list, goal, minConnections); 
		if(End != NULL){
			goalFound = true; 
		}

		
		Copy_WordToWordLL(Queue, list); 
		if(Queue->next == NULL){			  
			if(minConnections == -1){
				printf("\n%s cannot connect with %s\n", start, goal);
			}
			else{
				printf("\nThere are no words %d connections away that connect with %s\n", minConnections - 1, start); 
			}
			goalFound = -1;
		}
		Free_WordLL(list);	
		
	
		
	}

	struct word *connectionList; 	
	char** connectionArray; 
	if(goalFound != -1){
		//if the minimum number of connections is equal to negative 1, then it does a linked list
		if(minConnections == -1){
			connectionList = malloc(sizeof(struct word)); 
			connectionList->next = NULL; 
			connectionList = Convert_TreeStorageNodeToWordLL(connectionList, End); 
		}
		//if it is an array
		else{
			//This allocates the 2D array, num letters has to be plus one, because of weird indexing, num letters has to be plus one to, for sake of null terminator
			connectionArray = (char**)Allocate_2DArray(minConnections + 1, numLetters + 1); 
			//Convert the TreeStorageList To Array
			Convert_TreeStorageNodeTo2DArray(connectionArray, End, minConnections);
			
			
		}
	}

	Free_TreeStorageNode(ReverseTreeHeader); 
	Free_WordLL(QueueHeader);
	/*Free The Tree Set*/
	if(storageType == TREE_SET){
	
		Free_TreeSet(TreeHead->start);
		free(TreeHead);  
	}
	else{
		Free_HashSet(HashSet); 
	}
	if(minConnections != -1){
		return (goalFound == -1)?NULL:connectionArray;
	}
	//if it cannot be found, return null. Otherwise, return the list
	return (goalFound == -1)?NULL:connectionList; 
	return NULL; 





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
/*So, it is necessary to find something in a structed data set, this is how we do it*/ 
void* BinaryFirstSearch(void* goal, void* structure, int size, enum dataType type){
	//int i = length / 2; 
	while(/*compare(structure->data, goal, type) != <-- will have to be method*/0){
		
	}
	
	
}




/*This is essentially the lobby for the depth first search. It's the room before it goes into the hallway and things get going.
It just sets allocates some memory locations, sets some variables, and frees everything after the main search happened.*/ 
struct word* DepthFirstSearch_Lobby(char* start, char* goal, struct wordConnections **(*HashMap)){
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
	struct TreeStorageNode *last = DepthFirstSearch(start, goal, HashMap, HashSet, treeStorage);
	
	if(last == NULL){
		printf("%s couldn't connect to %s", start, goal); 
		return NULL; 
	}
	 
	//struct TreeStorageNode *last = Search_TreeStorageNode(goal, treeStorage); 
	/*This is the linked list that stores the connections from the start to the goal*/ 
	struct word *connection = malloc(sizeof(struct word)); 
	connection->next = NULL; 
	/*Goes recursively through the tree storage and adds the connections to the connection linked list*/ 
	connection = Convert_TreeStorageNodeToWordLL(connection, last);  
	Free_TreeStorageNode(treeStorage); 
	Free_HashSet(HashSet);
	return connection; 
	
}
struct TreeStorageNode* DepthFirstSearch(char* start, char* goal, struct wordConnections **(*HashMap), struct word **(*HashSet), struct TreeStorageNode *treeStorage){
		/*Adds the word to the hash set such that it can't be found again'*/ 
		AddToHashSet(start, HashSet); 
		
		/*First, we create a list that stores all of the words*/
		struct word* list =  linkOutput(start, HashMap[FirstHashFunction(start[0])][SecondHashFunction(start)], HashSet, HASH_SET, 1); 
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
			AddToHashSet(goal, HashSet); 
			Free_WordLL(listHeader); 
			return End; 
		}
		/*Then, we go away from the list's header'*/ 
		list = list->next; 
		/*We add the current word to the hash set*/ 
	
	
		/*if the list is not equal to null, we can extrapolate data from it*/ 
	
		  
		
			//	printf("Word: %s, ", nextWord); 
		while(list != NULL){
			struct TreeStorageNode* finalConnection = DepthFirstSearch(list->word, goal, HashMap, HashSet, treeStorage); 
			if(finalConnection != NULL){
				Free_WordLL(listHeader); 
				return finalConnection; 
			}
			list = list->next; 	
		}

		//printf("start: %s", start); 
		Free_WordLL(listHeader); 
}

char** FindByDepth_Lobby(int minConnections, char** allWords, struct wordConnections **(*HashMap)){
	//It is necessary to choose the word here, such that if the word happens to not be able to connect 7 or 8 out, then it can retry finding the new word
	char* start = ChooseStartWord(allWords, HashMap, 1);;
	//Create the Tree 
	struct DummyHeadNode *header = Allocate_TreeSet(start); 
	//Initialize Tree Set with start word
	//Create the list of the connections which will be returned, and be of size minconecctions 
	char** connections = (char**)Allocate_2DArray(minConnections + 1, numLetters + 1); 
	strcpy(*connections, start); 
		//Probably consider malloc, or calloc
	//Go into the algorithm
	int output = FindByDepth(0, minConnections, start, connections, header, HashMap); 
	//Free Tree Set
	Free_TreeSet(header->start);
	free(header);  
	if(output == -1){
		
		return FindByDepth_Lobby(minConnections, allWords, HashMap);
	}
	//After Algorithm
	
	//return the word connections 
	return connections; 		
	
}

int FindByDepth(int index, int maxIndex, char* curWord, char** connections, struct DummyHeadNode *header, struct wordConnections **(*HashMap)) {
	
	/*If the index is == to the max index, then we return 1, commemerating our success*/ 
	if(index == maxIndex){
		return 1; 
	}
	/*We do not want to add the word in the tree set, since it has already been added*/ 
	if(index != 0){
		AddNode_TreeSet(curWord, header, header->start, DUMMY, WORD); 
	} 
	printf("\nTree Set: ");
	Print_TreeSet(header->start, WORD);  
	
	/*We take the word & get all of its connections into a linked list
	 This will be the header of the linked list*/ 
	struct word *curLink = linkOutput(curWord, HashMap[FirstHashFunction(curWord[0])][SecondHashFunction(curWord)], header, TREE_SET, 0); 
	//Of course, a perm header for the linked list such that it can be freed
	struct word *linkHeader = curLink;
	//This is the functions output, when we call it again
	int output = 0; 
	do{
		//Cur link should go to the next link in the linked list
		curLink = curLink->next;
		
		/*If it's first connection is null, return -1*/
		if(curLink == NULL){
			Free_WordLL(linkHeader); 
			return -1; 
		}
		 
	
	
		//We want to add it & all of its connections into the tree set, [that should be done in link output, but you're going to want to check]
		
		//Then we want to take the first word it connects to and shove it into the connection
		//It has to be plus one, because the cur word has already been added to the index
		strcpy(*(connections + (index+1)), curLink->word); 
		//We then want to call the function, and have it return an integer 
	//if the integer ends up being -1:
	//We try again so therefore that must be a do while, starting from line 267
	}while((output = FindByDepth(index + 1, maxIndex, curLink->word, connections, header, HashMap)) == -1); 
	//We gotta free the link
	Free_WordLL(linkHeader); 
	//return 0 
	return 0; 
}


//Infinite Pathfinds Pseudo
//Make sure they're not the same word
//set goal found variable 
//Set the tree set
//the 2 dimensional word connections multiple linked lists. 1) pies ties tins 2) pies pins tins
//set the queue and its header --> the queue stores the words that will be searched
//add the word to the queue --> teh starting word, pies
//set the reverse tree header --> keeps track of which words connect to other words. (pigs pits pins pigs dies lies digs dips hits sits wits bits)
//set number of connections --> the number of words it takes to connect, that is (1 --> pies-ties, 2 --> pies-ties-tins 3 --> pies-ties-tins-tint etc)
//set the times found variable --> keeps track of the number of paths 2^19000
//for loop: (timesFound = 0; timesFound < maxTimesFound; timesFound++) 	
	//go to the next location in the queue
	//check if it's found the word
		//if it has:
			
	//if it's not found, do the usual route 
	//add to the next list
	

	
	
struct GenericLinkedListNode *Multiple_Pathfinds(char* start, char* goal, int maxTimesFound, struct wordConnections **(*HashMap)){

	/*If the words are the same*/
	if(strcmp(start,goal) == 0){
		printf("Those are the same word\n"); 
		return 0; 
	}
	
	
	//Boolean that determines if the goal has been found
	bool goalFound = false; 
	
	
	/*Allocate the Tree Set -- This keeps track of the words already found*/
	struct word **(*HashSet) = AllocateHashSet();
	AddToHashSet(start, HashSet); 
	


	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	struct TreeStorageNode *ReverseTreeHeader = Allocate_TreeStorageNode(start); 
	struct TreeStorageNode *prevConnection = ReverseTreeHeader;
	/*The very last node of the queue, this one actually saves it's location'*/ 
	struct TreeStorageNode* lastNode;  
	/*The last word of the queue*/
	struct TreeStorageNode *End; 	
	
	
	/*Declares the Queue -- contains which words are to be searched through*/
	struct word *Queue = malloc(sizeof(struct word)); 
	Queue->next = NULL; 
	AddToBack_WordLL(start, Queue, 0); 
	/*This is a temporary node*/
	struct word *QueueHeader = Queue; 
	
	/*The connections to the word, since there will be multiple*/ 
	struct GenericLinkedListNode *Connections = malloc(sizeof(struct GenericLinkedListNode)); 
	Connections->next = NULL; 

	
	int timesFound = 0; 
	Queue = Queue->next; 
	/*While the goal isn't found*/
	while(timesFound < maxTimesFound){

		  
 		/*All of the connecting to the queue get put to this list*/ 
		struct word *list = linkOutput(Queue->word, HashMap[FirstHashFunction(Queue->word[0])][SecondHashFunction(Queue->word)], HashSet, HASH_SET, 0); 
		
		prevConnection = Search_TreeStorageNode(Queue->word, ReverseTreeHeader); 
		 
		 
		 
		lastNode = ReturnLast_TreeStorageNode(ReverseTreeHeader); 
		
		/*The purpose of this method is to add the list of words that connect to the queue header
		@return --> if any of those words happen to be the goal, then it will return the node with the goal, which stores the previous bredth first connections*/  
		End = Copy_WordLLToTreeStorageNode(ReverseTreeHeader, prevConnection, list, goal, -1); 
		if(End == NULL){
			Copy_WordToWordLL(Queue, list);
		}
 
		else{
			//The Current Word Whose Subdivions actually happened to be the goal
			char* prevWord = Queue->word; 
			//if goalFound doesn't change, then the method returns null
			goalFound = true;
			//This Queue is necessary because it declares the word that has to be removed in the TreeStorageNode
		 
			//it has to go through this, and check if the word is on it
			//once it's checked these, it starts going through those words, if those words are in it, then it checks those words until it eventually is NULL 
			struct GenericLinkedListNode *currentConnection = AddConnection(Connections, timesFound, End);  
			Print_WordLL((struct word*)currentConnection->listHeader, LINKED);
			/*When it finds the word, it has to remove all its subdivisions from the Tree Storage Node Such that it won't loop through them again
			@param lastNode --> The last spot in the Tree Storage Node before all of the Queue's subdivisions got added in 
			@param QueueHeader --> The start of the queue, since I don't dequeue and only move position, the QueueHeader still exists
			@param HashSet --> The prevWord's subdivisions recently got added to the hash set, now it is necessary to remove them */ 
			RemoveFromPoint_TreeStorageNode(lastNode->next, QueueHeader, HashSet);
			/*Want to be sure that the end of the linked list is always NULL*/ 
			lastNode->next = NULL; 
			
		 /*Removes the Queue from wherever it is in the Tree Storage Node, also removes it from the Hash Set*/ 
			Remove_TreeStorageNode(prevWord, ReverseTreeHeader, HashSet); 
			/*Not enough words are being removed, and therefore, not all of the connections are being found*/ 
			//Check to see if this word has been found as of yet. 

			//This is the queue that stores all of the connections of the word
			struct word *miniQueue = malloc(sizeof(struct word)); 
			struct word *MiniQueueHead = miniQueue; 
			miniQueue->next = NULL;   
			
			AddToFront_WordLL(prevWord, miniQueue, 0); 
			miniQueue = miniQueue->next;
			  
		
			Copy_WordToWordLL(miniQueue, linkOutput(miniQueue->word, HashMap[FirstHashFunction(miniQueue->word[0])][SecondHashFunction(miniQueue->word)], NULL, NEITHER_SET, 0)); 
			while(miniQueue != NULL){
				
				if(Search_HashSet(miniQueue->word, HashSet) == 1 && Search_WordLL(miniQueue->word, ((struct word*)(currentConnection->listHeader))) == 0){
					//if(strcmp(miniQueue->word, prevWord) != 0){
					
						Copy_WordToWordLL(miniQueue, linkOutput(miniQueue->word, HashMap[FirstHashFunction(miniQueue->word[0])][SecondHashFunction(miniQueue->word)], NULL, NEITHER_SET, 0)); 
						Remove_TreeStorageNode(miniQueue->word, ReverseTreeHeader, HashSet);
						Remove_HashSet(miniQueue->word, HashSet); 
					//}
				}
				miniQueue = miniQueue->next; 
			}
			Free_WordLL(MiniQueueHead); 
			
			//while(foundYet == 0){
				//put all of the words it links to into it
				//miniQueue = linkOutput(miniQueue->word, HashMap[FirstHashFunction(miniQueue->word[0])][SecondHashFunction(miniQueue->word)], NULL, NEITHER_SET); 
				//loop through and check if there are any matches
				//if there are, go through all of the linked list words
				//Once it reaches the end of that, if any of them have connected, keep 
				//On
		//	}  
			
			//When times found reaches a certain, maximum number, the program stops running
			timesFound++; 
		}
		
		if(Queue->next == NULL){
			if(goalFound == false){printf("Words Can't Connect");}
			
			//this ends the program
			timesFound = maxTimesFound; 
			

		}
		Free_WordLL(list);	
		Queue = Queue->next; 
		
		
	}
 	

	Free_TreeStorageNode(ReverseTreeHeader); 
	Free_WordLL(QueueHeader);
	/*Free The Tree Set*/
	Free_HashSet(HashSet); 
	//if it cannot be found, return null. Otherwise, return the list
	 
	return (goalFound == true)?Connections:NULL; 





}

struct GenericLinkedListNode *Multiple_Pathfinds2(char* start, char* goal, int maxTimesFound, struct wordConnections **(*HashMap)){

	/*If the words are the same*/
	if(strcmp(start,goal) == 0){
		printf("Those are the same word\n"); 
		return 0; 
	}
	
	
	//Boolean that determines if the goal has been found
	bool goalFound = false; 
	
	
	/*Allocate the Tree Set -- This keeps track of the words already found*/
	struct word **(*HashSet) = AllocateHashSet();
	AddToHashSet(start, HashSet); 
	


	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	struct TreeStorageNode *ReverseTreeHeader = Allocate_TreeStorageNode(start); 
	struct TreeStorageNode *prevConnection = ReverseTreeHeader;
	/*The very last node of the queue, this one actually saves it's location'*/ 
	struct TreeStorageNode* lastNode;  
	/*The last word of the queue*/
	struct TreeStorageNode *End; 	
	
	
	/*Declares the Queue -- contains which words are to be searched through*/
	struct word *Queue = malloc(sizeof(struct word)); 
	Queue->next = NULL; 
	AddToBack_WordLL(start, Queue, 0); 
	/*This is a temporary node*/
	struct word *QueueHeader = Queue; 
	
	/*The connections to the word, since there will be multiple*/ 
	struct GenericLinkedListNode *Connections = malloc(sizeof(struct GenericLinkedListNode)); 
	Connections->next = NULL; 

	
	int timesFound = 0; 
	Queue = Queue->next; 
	/*While the goal isn't found*/
	while(timesFound < maxTimesFound){
		  
 		/*All of the connecting to the queue get put to this list*/ 
		struct word *list = linkOutput(Queue->word, HashMap[FirstHashFunction(Queue->word[0])][SecondHashFunction(Queue->word)], HashSet, HASH_SET, 0); 
		
		prevConnection = Search_TreeStorageNode(Queue->word, ReverseTreeHeader); 
		lastNode = ReturnLast_TreeStorageNode(ReverseTreeHeader); 
		
		/*The purpose of this method is to add the list of words that connect to the queue header
		@return --> if any of those words happen to be the goal, then it will return the node with the goal, which stores the previous bredth first connections*/  
		End = Copy_WordLLToTreeStorageNode(ReverseTreeHeader, prevConnection, list, goal, -1); 
		//Print_TreeStorageNode(ReverseTreeHeader);
	//	printf("\n\n\n");  
		if(End == NULL){
			Copy_WordToWordLL(Queue, list); 
			
		}
 
		else{
			 
			//The Current Word Whose Subdivions actually happened to be the goal
			char* prevWord = Queue->word; 
			//if goalFound doesn't change, then the method returns null
			goalFound = true;
			//This Queue is necessary because it declares the word that has to be removed in the TreeStorageNode
		 
			//it has to go through this, and check if the word is on it
			//once it's checked these, it starts going through those words, if those words are in it, then it checks those words until it eventually is NULL 
			struct GenericLinkedListNode *currentConnection = AddConnection(Connections, timesFound, End);  

			Print_WordLL((struct word*)currentConnection->listHeader, LINKED);
			printf("\n\n\n"); 
			/*When it finds the word, it has to remove all its subdivisions from the Tree Storage Node Such that it won't loop through them again
			@param lastNode --> The last spot in the Tree Storage Node before all of the Queue's subdivisions got added in 
			@param QueueHeader --> The start of the queue, since I don't dequeue and only move position, the QueueHeader still exists
			@param HashSet --> The prevWord's subdivisions recently got added to the hash set, now it is necessary to remove them */ 
			RemoveFromPoint_TreeStorageNode(lastNode->next, QueueHeader, HashSet);
			/*Want to be sure that the end of the linked list is always NULL*/ 
			lastNode->next = NULL; 
			
		 /*Removes the Queue from wherever it is in the Tree Storage Node, also removes it from the Hash Set*/ 
			Remove_TreeStorageNode(prevWord, ReverseTreeHeader, HashSet); 
			Remove_HashSet(prevWord, HashSet); 
		
			struct word *testStructure = malloc(sizeof(struct word)); 
			testStructure->next = NULL; 
			Copy_WordToWordLL(testStructure, (struct word*)currentConnection->listHeader); 
			/*Not enough words are being removed, and therefore, not all of the connections are being found*/ 
			//Check to see if this word has been found as of yet. 
			testStructure = testStructure->next; 
			while(testStructure->next != NULL){
				
				Remove_HashSet(testStructure->word, HashSet);				 
				testStructure = testStructure->next; 
			}
			//This is the queue that stores all of the connections of the word
		

		
			//while(foundYet == 0){
				//put all of the words it links to into it
				//miniQueue = linkOutput(miniQueue->word, HashMap[FirstHashFunction(miniQueue->word[0])][SecondHashFunction(miniQueue->word)], NULL, NEITHER_SET); 
				//loop through and check if there are any matches
				//if there are, go through all of the linked list words
				//Once it reaches the end of that, if any of them have connected, keep 
				//On
			  
			
			//When times found reaches a certain, maximum number, the program stops running
			timesFound++; 
			
		}
		
		if(Queue->next == NULL){
			if(goalFound == false){printf("Words Can't Connect");}
			
			//this ends the program
			timesFound = maxTimesFound; 
			

		}
		Free_WordLL(list);	
	
		Queue = Queue->next; 
		
	}
 	

	Free_TreeStorageNode(ReverseTreeHeader); 
	Free_WordLL(QueueHeader);
	/*Free The Tree Set*/
	Free_HashSet(HashSet); 
	//if it cannot be found, return null. Otherwise, return the list
	 
	return (goalFound == true)?Connections:NULL; 





}

struct word* MultiplePathFinds_Final(char* start, char* goal, struct wordConnections **(*HashMap)){
	
	/*If the words are the same*/
	if(strcmp(start,goal) == 0){
		printf("Those are the same word\n"); 
		return 0; 
	}
	//Boolean that determines if the goal has been found
	bool goalFound = false; 
	/*Allocate the Tree Set -- This keeps track of the words already found*/
	
	struct DummyHeadNode *TreeHead; 
	struct word **(*HashSet); 

	HashSet = AllocateHashSet();
	AddToHashSet(start, HashSet); 

	
	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	struct TreeStorageNode *ReverseTreeHeader = Allocate_TreeStorageNode(start); 
	struct TreeStorageNode *prevConnection = ReverseTreeHeader; 
	
	/*Declares the Queue -- contains which words are to be searched through*/
	struct word *Queue = malloc(sizeof(struct word)); 
	Queue->next = NULL; 
	AddToBack_WordLL(start, Queue, 0); 
	/*This is a temporary node*/
	struct word *QueueHeader = Queue; 
	/*The last word of the queue*/
	struct TreeStorageNode *End; 	

	/*While the goal isn't found*/
	while(goalFound == false){
		
		Queue = Queue->next;
		struct word *list; 

		list = linkOutput(Queue->word, HashMap[FirstHashFunction(Queue->word[0])][SecondHashFunction(Queue->word)], HashSet, HASH_SET, 0); 
	
		
		prevConnection = Search_TreeStorageNode(Queue->word, ReverseTreeHeader);  
		End = Copy_WordLLToTreeStorageNode(ReverseTreeHeader, prevConnection, list, goal, -1); 
		/*If the goal is found*/ 
		if(End != NULL){
			goalFound = true; 
			if(strcmp(End->prev->word, start) != 0){
				char* rowWord = End->prev->prev->word; 
				char* columnWord = End->prev->word; 
				RemoveFromColumn_2DLL(rowWord, columnWord, HashMap[FirstHashFunction(rowWord[0])][SecondHashFunction(rowWord)]); 
			}
		}
		Copy_WordToWordLL(Queue, list); 
		if(Queue->next == NULL){			  
			printf("%s cannot connect with %s\n", start, goal);
			goalFound = -1;
		}
		Free_WordLL(list);	
		
		
		
	}

	struct word *connectionList; 
	
	if(goalFound != -1){
		//If it is a word you are trying to find, then return a Linked List

			connectionList = malloc(sizeof(struct word)); 
			connectionList->next = NULL; 
			connectionList = Convert_TreeStorageNodeToWordLL(connectionList, End); 

		//Otherwise, if it is instead a distance you are trying to traverse, it will set put it into an array
		 
	}
	Free_TreeStorageNode(ReverseTreeHeader); 
	Free_WordLL(QueueHeader);
	Free_HashSet(HashSet); 
	//If it minimum connections is not -1, then return an array
	//if it cannot be found, return null. Otherwise, return the list
	return (goalFound == -1)?NULL:connectionList; 
	return NULL; 

	
} 
/*Goal: Be able to find a certain quantity of different paths
Necessary New Variables
- int timesFound --> The number of linked lists
- int maxTimesFound --> the max number of times to run through 
- struct wordConnections **Connections --> Stores all of the ways the words can connect Connections[Number of times conneceted][Each Connection]
- struct DummyHeadNode **WordsFound --> A linked list storing Tree Sets, each linked list being another, deeper connection
 
How it works
- Each time it goes deeper, and check if a word is acceptable, it has to go through all of the previous linked list hash nodes
- Each time it goes deeper, it creates a new TreeSet, deeper in the 2D linked list 


*/ 
struct GenericLinkedListNode *AddConnection(struct GenericLinkedListNode *Connections, int timesFound, struct TreeStorageNode *End){

	//add a wordConnections row to the 2D connections variable
	Add_GenericLinkedListNode(Connections, WORD_LL);
	/*Obtaining the list that will have all the words added to it 
	@param timesFound --> the next one up, since timesFound has just gone up one
	@param Connections --> the header*/ 
	struct GenericLinkedListNode *currentConnection = ReturnIndex_GenericLinkedListNode(timesFound, Connections);  
	//go through the previous connections and add each word to a word LinkedList
	/*CRASHES HERE :(*/ 
	currentConnection->listHeader = malloc(sizeof(struct word));  
	((struct word*)(currentConnection->listHeader))->next = NULL; 
	currentConnection->listHeader = Convert_TreeStorageNodeToWordLL(((struct word*)(currentConnection->listHeader)), End);
	return currentConnection; 
}



