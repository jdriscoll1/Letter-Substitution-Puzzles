#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "../includes/GameFunctions.h"
#include "../includes/PathGameComponents.h"
#include "../includes/UserInput.h"

#include "../../structs/includes/GenericLinkedListNode.h"


extern int numLetters;

struct GameComponents *InitializeGameComponents(struct wordDataArray* IntToWord_HashMap, int minConnections, struct WordSet *wordSet){
		//Instantiate the Structure
	struct GameComponents* gameComponents = malloc(sizeof(struct GameComponents)); 
	do{
		//This chooses the start word
		gameComponents->start = ChooseStart(IntToWord_HashMap);   
		//Finds the goal word 
		gameComponents->goal = BreadthFirstSearch_Distance_Goal(gameComponents->start, minConnections, IntToWord_HashMap, wordSet); 
	}while(gameComponents->goal == -1); 
	
	//Sets the minimum number of connection
	gameComponents->minConnections = minConnections; 
	//Sets the number of moves
	gameComponents->numMoves = 0;
	//Instantiates the number of undo calls 
	gameComponents->undoCalls = 0; 
 	//Instantiates the number of hint points
 	gameComponents->hc = init_HintComponents(); 
 	setHintFound(gameComponents->start, IntToWord_HashMap); 

	gameComponents->prevInput = gameComponents->start; 
	
	//Initialize the arrayList 
	gameComponents->aList = init_ArrayList(numLetters * (minConnections * 1.5), numLetters * (minConnections), STR); 
	
	//Instantiate the input storage 
	gameComponents->storage = malloc(sizeof(struct GenericLinkedListNode)); 
	gameComponents->storage->next = NULL; 
	gameComponents->storage->prev =  NULL; 

	//Creates the storage header
	gameComponents->storageHeader = gameComponents->storage; 

	//Instantiates the user connection
	gameComponents->userConnections = malloc(sizeof(struct intList)); 
	gameComponents->userConnections->size = 0; 
	gameComponents->userConnections->next = NULL; 
	//There is no input to be freed
	//Insert the word into the back of the word linked list
	AddToBack_IntLL(gameComponents->start, gameComponents->userConnections); 
 
 	addString_ArrayList(Convert_IntToWord(gameComponents->start, IntToWord_HashMap), numLetters, gameComponents->aList); 
 	 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(gameComponents->storage, INT_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(gameComponents->userConnections, gameComponents->storage, 1, INT_LL);

	return gameComponents;

}


void ResetGameComponents(struct GameComponents *gc, struct wordDataArray* IntToWord_HashMap){
	gc->numMoves = 0;
	//Instantiates the number of undo calls 
	gc->undoCalls = 0; 
 	//Instantiates the number of hint points
	
	//Sets the previous input to the starting word
	gc->prevInput = gc->start; 
 	//Frees, then initializes the array list
	free_ArrayList(gc->aList); 
	//Initialize the arrayList 
	gc->aList = init_ArrayList(numLetters * (gc->minConnections * 1.5), numLetters * (gc->minConnections), STR); 
	
	Free_GenericLinkedList(gc->storageHeader); 
	
	//Instantiate the input storage 
	gc->storage = malloc(sizeof(struct GenericLinkedListNode)); 
	gc->storage->next = NULL; 
	gc->storage->prev =  NULL; 

	//Creates the storage header
	gc->storageHeader = gc->storage; 

	Free_IntLL(gc->userConnections); 
	//Instantiates the user connection
	gc->userConnections = malloc(sizeof(struct intList)); 
	gc->userConnections->size = 0; 
	gc->userConnections->next = NULL; 
	//There is no input to be freed


	//Insert the word into the back of the word linked list
	AddToBack_IntLL(gc->start, gc->userConnections); 
 
 	addString_ArrayList(Convert_IntToWord(gc->start, IntToWord_HashMap), numLetters, gc->aList); 
 	 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(gc->storage, INT_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, INT_LL);


}


char* RemoveWord_Struct(struct GameComponents* gc, char* input, int freeInput, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//If I have previously undone a move, I need to free that move
	if(gc->undoCalls != 0){
		ResetUndo(gc->storageHeader, &(gc->storage), gc->userConnections, &(gc->undoCalls)); 
	
	}
	//First we have to remove the -
	char* word = substr(input, 1, numLetters + 1, freeInput);
	if(safeStrLen(word) > numLetters){
		printf("Word is too long\n"); 
		return word; 
	}
	else if(safeStrLen(word) < numLetters){
		printf("Word is too short\n"); 
		return word; 
	}
	int wordID = Convert_WordToInt(word, WordToInt_HashMap); 
	//Then we have to Remove it from the list, and all the words after it 
	RemoveFrom_IntLL(wordID, gc->userConnections->next);

	//We have to change the word to be compared to the last word in the list
	gc->prevInput = FindLast_IntLL(gc->userConnections);  		
	//This adds to the storage such that the user can undo a move
	AddToFront_GenericLinkedListNode(gc->storage, INT_LL); 
	CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, INT_LL); 
	
	//You've gotta remove the current array list and replace it 
	//First, empty the current array list string
	CopyWordLLOntoArrayList(gc, IntToWord_HashMap); 
	++gc->numMoves; 
	//Frees the word
	return word; 	
}

int inDictionary(int word){
	return (word == -1) ? 0 : 1; 
	
}


void Undo_Struct(struct GameComponents* gc, struct wordDataArray* IntToWord_HashMap){
	
	if(gc->numMoves == 0){
		//printf("No move to return to.\n");
	}
	else{
		//There has been an undo
		(gc->undoCalls)++; 
		//The number of moves has gone up 
		(gc->numMoves)--; 
  
		gc->storage = gc->storage->next; 

		gc->prevInput = FindLast_IntLL((gc->storage)->next->listHeader); 
		//ALthough I would like to just remove the last word. The issue is if the person removes a bunch of words, that won't work
		CopyWordLLOntoArrayList(gc, IntToWord_HashMap); 		 
	
	}
}
void Redo_Struct(struct GameComponents* gc, struct wordDataArray* IntToWord_HashMap){
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
		gc->prevInput = FindLast_IntLL((gc->storage)->next->listHeader);   
		//This resets the array list with the game component
		CopyWordLLOntoArrayList(gc, IntToWord_HashMap); 
	}
}
int AddWord_Struct(struct GameComponents* gc, char* input, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//Checks if the word is valid based on the previous input 
	int isValid = Check_Input(gc->prevInput, (const char*)input, WordToInt_HashMap, IntToWord_HashMap); 
	
	if(isValid == VALID){
			//Once it is valid, we can convert it into an integer
			int newWord = Convert_WordToInt(input, WordToInt_HashMap); 
			//If I have previously undone a move, I need to free that move		
			if(gc->undoCalls != 0){
				ResetUndo(gc->storageHeader, &(gc->storage), gc->userConnections, &(gc->undoCalls));
			}
			//Adds the word to the back of the linked list
			AddToBack_IntLL(newWord, gc->userConnections);
			
		
			addString_ArrayList("->", 2, gc->aList); 
		
			//Copies the new word into the string arraylist
			addString_ArrayList((const char*)Convert_IntToWord(newWord, IntToWord_HashMap), numLetters, gc->aList); 
			
			//Copies the new word into the prev input
			gc->prevInput = newWord; 
			//Frees a space for the Generic Linked List Storage
			AddToFront_GenericLinkedListNode(gc->storage, INT_LL); 
			//Copies userConnections list to the front of the Generic Linked List Node
			CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, INT_LL); 
			//Put the new word into the Hash Set
			setHintFound(newWord, IntToWord_HashMap);  
			//Another move added
			gc->numMoves++;
	}

	return isValid; 

	
}

void FreeGameComponents(struct GameComponents *gc, struct wordDataArray* IntToWord_HashMap){
	Free_IntLL(gc->userConnections); 
	
	free_HintComponents((uintptr_t)gc->hc, IntToWord_HashMap); 
 	//Only want to free the goal, because the start is in the array of all the words
	Free_GenericLinkedList(gc->storageHeader); 
	free_ArrayList(gc->aList); 
	free(gc);  

}

void CopyWordLLOntoArrayList(struct GameComponents *gc, struct wordDataArray* IntToWord_HashMap){

	char* empty = ""; 
	char* list = (char*)gc->aList->list; 
	safeStrcpy(&list, (const char*) empty, 1, gc->aList->currSize); 
	gc->aList->currSize = gc->aList->initSize; 
	gc->aList->currPrecision = 0; 
	char* output = toString_IntLL(gc->storage->next->listHeader, LINKED, IntToWord_HashMap); 

	int listSize = ((struct intList*)(gc->storage->next->listHeader))->size; 
	int length = (numLetters * (listSize)) + (2 * (listSize - 1)); 
	addString_ArrayList(output, length, gc->aList); 

	free(output); 
}
