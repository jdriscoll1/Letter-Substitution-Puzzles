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
	//printf("%s", gameComponents->shortestPath[0]); 
	//Sets the minimum number of connection
	gameComponents->minConnections = minConnections; 
	//Sets the number of moves
	gameComponents->numMoves = 0;
	//Instantiates the number of undo calls 
	gameComponents->undoCalls = 0; 
 	//Instantiates the number of hint points
 	gameComponents->hintPoints = 50; 
	//Allocates space for the previous input 
	gameComponents->prevInput = malloc(numLetters + 1);
	
	//Sets the previous input to the starting word
	strcpy(gameComponents->prevInput, gameComponents->shortestPath[0]); 
 
	//Initialize the arrayList 
	gameComponents->aList = init_ArrayList(numLetters * (minConnections * 1.5), numLetters * (minConnections), STR); 
	
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
 
 	addString_ArrayList(gameComponents->shortestPath[0], gameComponents->aList); 
 	 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(gameComponents->storage, WORD_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(gameComponents->userConnections, gameComponents->storage, 1, WORD_LL);

	return gameComponents;

}
void RemoveWord_Struct(struct GameComponents* gc, char* input, int freeInput){
	//If I have previously undone a move, I need to free that move
	if(gc->undoCalls != 0){
		ResetUndo(gc->storageHeader, &(gc->storage), gc->userConnections, &(gc->undoCalls)); 
	
	}
	//First we have to remove the -
	char* word = substr(input, 1, numLetters + 1, freeInput);
	
	//Then we have to Remove it from the list, and all the words after it 
	RemoveFrom_WordLL(word, gc->userConnections->next);
			
	//We have to change the word to be compared to the last word in the list
	strcpy(gc->prevInput, FindLast_WordLL(gc->userConnections));
			
	//This adds to the storage such that the user can undo a move
	AddToFront_GenericLinkedListNode(gc->storage, WORD_LL); 
	CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, WORD_LL); 
	
	//You've gotta remove the current array list and replace it 
	//First, empty the current array list string
	CopyWordLLOntoArrayList(gc); 
	++gc->numMoves; 
	
	//Frees the word
	free(word); 
	
	
	
	
	
}
void Undo_Struct(struct GameComponents* gc){
	
	if(gc->numMoves == 0){
		printf("No move to return to.\n"); 
	}
	else{
		//There has been an undo
		(gc->undoCalls)++; 
		//The number of moves has gone up 
		(gc->numMoves)--; 
  
		gc->storage = gc->storage->next; 
		//Change previousInput. It has to be changed here so that it if I add one, it will be able to recognize it
		strcpy(gc->prevInput, FindLast_WordLL((gc->storage)->next->listHeader));  
		//ALthough I would like to just remove the last word. The issue is if the person removes a bunch of words, that won't work
		CopyWordLLOntoArrayList(gc); 		 
	
	}
}
void Redo_Struct(struct GameComponents* gc){
	/*if it is even possible for a user to redo a move*/
	if(gc->undoCalls == 0){
		printf("No move to be redone\n"); 
	}
	else{
		//Undoes an undo
		--gc->undoCalls; 
		//Brings back a move
		++gc->numMoves; 
		//Changes the turn to the previous one 
		gc->storage = (gc->storage)->prev; 
		//makes the previous input the new one
		strcpy(gc->prevInput, FindLast_WordLL((gc->storage)->next->listHeader));  
		//This resets the array list with the game component
		CopyWordLLOntoArrayList(gc); 
	}
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
			
		
			addString_ArrayList("->", gc->aList); 
		
			//Copies the new word into the string arraylist
			addString_ArrayList(newWord, gc->aList); 
			
			//Copeis the new word into the prev input
			strcpy(gc->prevInput, newWord); 
			//Frees a space for the Generic Linked List Storage
			AddToFront_GenericLinkedListNode(gc->storage, WORD_LL); 
			//Copies userConnections list to the front of the Generic Linked List Node
			CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, WORD_LL); 
			//Another move :)
			(gc->numMoves)++;
	}
	//TO-DO:Change isValid to have more specific outputs (Probably just 0 or 1. Perhaps have 2 for not long enough. And 3 for too long?) 
	return isValid; 

	
}

void FreeGameComponents(struct GameComponents *gameComponents){
	free(gameComponents->prevInput);
	Free_WordLL(gameComponents->userConnections); 
	
	
	Free_2DArray(gameComponents->minConnections + 2, (void***)(gameComponents->shortestPath), 0);   
	Free_GenericLinkedList(gameComponents->storageHeader); 
	free_ArrayList(gameComponents->aList); 
	free(gameComponents);  

}

void CopyWordLLOntoArrayList(struct GameComponents *gc){
	strcpy(gc->aList->list, ""); 
	gc->aList->currSize = gc->aList->initSize; 
	gc->aList->currPrecision = 0; 
	char* output = toString_WordLL(gc->storage->next->listHeader, LINKED); 
	addString_ArrayList(output, gc->aList); 
	free(output); 
}
