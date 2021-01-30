#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "WordLinkedList.h"
#include "GameFunctions.h"
#include "PathGameComponents.h"
#include "UserInput.h"
#include "Arrays.h"
#include "GenericLinkedListNode.h"
#include "HashMap.h"

extern int numLetters;

struct GameComponents *InitializeGameComponents(char** allWords, struct wordConnections **(*HashMap), int minConnections){
		//Instantiate the Structure
	struct GameComponents* gameComponents = malloc(sizeof(struct GameComponents)); 

	//Create the shortest path from the beginning word and the goal word, whcih shall both be determined in this method
	gameComponents->shortestPath = ChoosePath(allWords, HashMap, minConnections); 

	//Sets the minimum number of connection
	gameComponents->minConnections = minConnections; 
	//Sets the number of moves
	gameComponents->numMoves = 0;
	//Instantiates the number of undo calls 
	gameComponents->undoCalls = 0; 
 
	//Allocates space for the previous input 
	gameComponents->prevInput = malloc(numLetters + 1);
	
	//Sets the previous input to the starting word
	strcpy(gameComponents->prevInput, gameComponents->shortestPath[0]); 
 
	
	
	//Instantiate the input storage 
	gameComponents->storage = malloc(sizeof(struct GenericLinkedListNode)); 
	gameComponents->storage->next = NULL; 
	gameComponents->storage->prev =  NULL; 

	//Creates the storage header
	gameComponents->storageHeader = gameComponents->storage; 

	//Instantiates the user connection
	gameComponents->userConnections = malloc(sizeof(struct word)); 
	gameComponents->userConnections->next = NULL; 
	//There is no input to be freed
	gameComponents->userConnections->dataMalloc = 0; 

	//Insert the word into the back of the word linked list
	AddToBack_WordLL(strdup(gameComponents->shortestPath[0]), gameComponents->userConnections, 1); 
 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(gameComponents->storage, WORD_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(gameComponents->userConnections, gameComponents->storage, 1, WORD_LL);

	return gameComponents;

}
void RemoveWord_Struct(struct GameComponents* gc){
	
	
	
	
	
	
	
}
void Undo_Struct(struct GameComponents* gc){
}
void Redo_Struct(struct GameComponents* gc){
}
int AddWord_Struct(struct GameComponents* gc, const char* newWord, struct wordConnections **(*HashMap)){
	//Checks if the word is valid based on the previous input 
	int isValid = Check_Input(gc->prevInput, newWord, HashMap);  
	if(isValid == 1){
			//If I have previously undone a move, I need to free that move		
			if(gc->undoCalls != 0){
				ResetUndo(gc->storageHeader, &(gc->storage), gc->userConnections, &(gc->undoCalls));
			}
			//Adds the word to the back of the linked list
			AddToBack_WordLL(strdup(newWord), gc->userConnections, 1);
			//Copeis the new word into the prev input
			strcpy(gc->prevInput, newWord); 
			//Frees a space for the Generic Linked List Storage
			AddToFront_GenericLinkedListNode(gc->storage, WORD_LL); 
			//Copies userConnections list to the front of the Generic Linked List Node
			CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, WORD_LL); 
			//Anotehr move :)
			++gc->numMoves;
	}
	//TO-DO:Change isValid to have more specific outputs (Probably just 0 or 1. Perhaps have 2 for not long enough. And 3 for too long?) 
	return isValid; 

	
}

void FreeGameComponents(struct GameComponents *gameComponents){
	free(gameComponents->prevInput);
	Free_WordLL(gameComponents->userConnections); 
	
	
	Free_2DArray(gameComponents->minConnections + 2, (void***)(gameComponents->shortestPath), 0);   
	Free_GenericLinkedList(gameComponents->storageHeader); 
	free(gameComponents);  

}
