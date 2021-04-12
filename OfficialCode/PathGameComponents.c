#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "WordLinkedList.h"
#include "GameFunctions.h"
#include "PathGameComponents.h"
#include "UserInput.h"
#include "Arrays.h"
#include "GenericLinkedListNode.h"
#include "HashSet.h"
#include "HashFunctions.h"

extern int numLetters;

struct GameComponents *InitializeGameComponents(char** allWords, struct DummyHeadNode **(*HashMap), int minConnections){
		//Instantiate the Structure
	struct GameComponents* gameComponents = malloc(sizeof(struct GameComponents)); 
	do{
		//This chooses the start word
		gameComponents->start = ChooseStart(allWords, HashMap, 0);   
		//Finds the goal word 
		gameComponents->goal = BreadthFirstSearch_Distance_Goal(gameComponents->start, minConnections, HashMap); 
		
	}while(gameComponents->goal == NULL); 
	
	//Sets the minimum number of connection
	gameComponents->minConnections = minConnections; 
	//Sets the number of moves
	gameComponents->numMoves = 0;
	//Instantiates the number of undo calls 
	gameComponents->undoCalls = 0; 
 	//Instantiates the number of hint points
 	gameComponents->hc = init_HintComponents(); 
 	AddToHashSet(gameComponents->start,  gameComponents->hc->wordsGiven, 0); 
	//Allocates space for the previous input 
	gameComponents->prevInput = malloc(numLetters + 1);
	
	//Sets the previous input to the starting word
    safeStrcpy(&gameComponents->prevInput, (const char*)gameComponents->start, numLetters, numLetters + 1);
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
    gameComponents->userConnections->size = 0; 
	//Insert the word into the back of the word linked list
	AddToBack_WordLL(strdup(gameComponents->start), gameComponents->userConnections, 1); 
 
 	addString_ArrayList(gameComponents->start, numLetters, gameComponents->aList); 
 	 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(gameComponents->storage, WORD_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(gameComponents->userConnections, gameComponents->storage, 1, WORD_LL);

	return gameComponents;

}


void ResetGameComponents(struct GameComponents *gc){
	gc->numMoves = 0;
	//Instantiates the number of undo calls 
	gc->undoCalls = 0; 
 	//Instantiates the number of hint points
	
	//Sets the previous input to the starting word
    safeStrcpy(&gc->prevInput, (const char*)gc->start, numLetters, numLetters + 1); 
    
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

	Free_WordLL(gc->userConnections); 
	//Instantiates the user connection
	gc->userConnections = malloc(sizeof(struct word)); 
	gc->userConnections->next = NULL; 
	//There is no input to be freed
	gc->userConnections->dataMalloc = 0; 
	gc->userConnections->size = 0;

	//Insert the word into the back of the word linked list
	AddToBack_WordLL(strdup(gc->start), gc->userConnections, 1); 
 
 	addString_ArrayList(gc->start, numLetters, gc->aList); 
 	 
	//Allocates space at the beginning of the generic linked list node
	AddToFront_GenericLinkedListNode(gc->storage, WORD_LL); 

	//Insert the word into the front of the Generic Linked List
	CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, WORD_LL);


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
	safeStrcpy(&gc->prevInput, (const char*) FindLast_WordLL(gc->userConnections), numLetters, numLetters + 1); 		
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
int inDictionary(const char* word, struct DummyHeadNode** (*HashMap)){
	int element1 = FirstHashFunction(word[0]); 
	int element2 = SecondHashFunction(word);  
	//Safeguard to verify that there will be no misconduct
	if(element1 == -1 || element2 == -1){
		return 0; 
	}
	struct DummyHeadNode* wordOptions = HashMap[element1][element2];  
	return (Search_TreeSet((void*)word, wordOptions->start, WORD4LL) == NULL) ? 0 : 1 ;
	
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

		safeStrcpy(&gc->prevInput, (const char*)FindLast_WordLL((gc->storage)->next->listHeader), numLetters, numLetters + 1); 
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
		safeStrcpy(&gc->prevInput, (const char*)FindLast_WordLL((gc->storage)->next->listHeader), numLetters, numLetters + 1);  
		//This resets the array list with the game component
		CopyWordLLOntoArrayList(gc); 
	}
}
int AddWord_Struct(struct GameComponents* gc, const char* newWord, struct DummyHeadNode** (*HashMap)){
	//Checks if the word is valid based on the previous input 
	int isValid = Check_Input(gc->prevInput, newWord, HashMap); 
	if(isValid == 1){
			//If I have previously undone a move, I need to free that move		
			if(gc->undoCalls != 0){
				ResetUndo(gc->storageHeader, &(gc->storage), gc->userConnections, &(gc->undoCalls));
			}
			//Adds the word to the back of the linked list
			AddToBack_WordLL(strdup(newWord), gc->userConnections, 1);
			
		
			addString_ArrayList("->", 2, gc->aList); 
		
			//Copies the new word into the string arraylist
			addString_ArrayList(newWord, numLetters, gc->aList); 
			
			//Copies the new word into the prev input
			safeStrcpy(&gc->prevInput, newWord, numLetters, numLetters + 1); 
			//Frees a space for the Generic Linked List Storage
			AddToFront_GenericLinkedListNode(gc->storage, WORD_LL); 
			//Copies userConnections list to the front of the Generic Linked List Node
			CopyInto_GenericLinkedListNode(gc->userConnections, gc->storage, 1, WORD_LL); 
			//Put the new word into the Hash Set
			AddToHashSet(strdup((char*)newWord), gc->hc->wordsGiven, 1);  
			//Another move added
			(gc->numMoves)++;
	}
	//TO-DO:Change isValid to have more specific outputs (Probably just 0 or 1. Perhaps have 2 for not long enough. And 3 for too long?) 
	return isValid; 

	
}

void FreeGameComponents(struct GameComponents *gameComponents){
	free(gameComponents->prevInput);
	Free_WordLL(gameComponents->userConnections); 
	
	free_HintComponents((unsigned long long)gameComponents->hc); 
 	//Only want to free the goal, because the start is in the array of all the words
    free(gameComponents->goal); 
	Free_GenericLinkedList(gameComponents->storageHeader); 
	free_ArrayList(gameComponents->aList); 
	free(gameComponents);  

}

void CopyWordLLOntoArrayList(struct GameComponents *gc){

	char* empty = ""; 
	char* list = (char*)gc->aList->list; 
	safeStrcpy(&list, (const char*) empty, 1, gc->aList->currSize); 
	gc->aList->currSize = gc->aList->initSize; 
	gc->aList->currPrecision = 0; 
	char* output = toString_WordLL(gc->storage->next->listHeader, LINKED); 

	int listSize = ((struct word*)(gc->storage->next->listHeader))->size; 
	int length = (numLetters * (listSize)) + (2 * (listSize - 1)); 
	addString_ArrayList(output, length, gc->aList); 
	free(output); 
}
