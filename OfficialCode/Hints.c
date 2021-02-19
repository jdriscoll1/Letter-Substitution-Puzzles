#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hints.h"
#include "PathGameComponents.h"
#include "HashSet.h"
#include "BreadthFirstSearch.h"
#include "Arrays.h"
#include "HashMap.h"
#include "UserInput.h"

#define SIZE 255
#define NUM_LETTERS 26

extern int numLetters;

struct HintComponents* init_HintComponents(){
	struct HintComponents* hc = malloc(sizeof(struct HintComponents)); 
	
	//The hint has not been used at the beginning of the round
	hc->hint1Used = 0; 
	
	//This allocates the Hash Set
	hc->wordsGiven = AllocateHashSet(); 
	
	//Allocates the Generic Linked List Node
	hc->letterOptions = malloc(sizeof(struct GenericLinkedListNode)); 
	hc->letterOptions->next = NULL; 
	hc->letterOptions->prev = NULL; 
	hc->letterOptions->listType = HINT3;
	hc->letterOptionsHeader = hc->letterOptions; 
	
	//The starting amount of hint points
	hc->hintPoints = 50; 
	//The weight of hint 1
	hc->hint1Weight = 35;
	//The weight of hint 2
	hc->hint2Weight = 30;
	//The weight of hint 3 
	hc->hint3Weight = 15; 
	
	hc->lettersGiven = calloc(NUM_LETTERS, sizeof(bool)); 
	int i; 
	for(i = 0; i < NUM_LETTERS; i++){
		hc->lettersGiven[i] = false; 
	} 
	
	 
	hc->hint2OutputQuantity = 3; 
}
//The first hint -- gives them the minimum number of connections
char* hint1(unsigned long long gcLong){
	char* output = malloc(SIZE); 
	struct GameComponents *gc = (struct GameComponents*)gcLong; 
	if(gc->hc->hintPoints < gc->hc->hint1Weight){
		snprintf(output, SIZE, "Insufficent hint points. Only %d\n", gc->hc->hintPoints);  
	}
	else if(gc->hc->hint1Used == 1){
		snprintf(output, SIZE, "Hint 1 already used, %d min connections\n", gc->minConnections); 
	
	}
	else{
	
		gc->hc->hintPoints -= gc->hc->hint1Weight; 
		snprintf(output, SIZE, "Hint -- The absolute minimum number of connections is: %d.\nYou now have %d hint points", gc->minConnections, gc->hc->hintPoints);
		gc->hc->hint1Used = 1; 
	}
	return output; 
}
/*Hint 2: Offers the user a word options from their current locations
@param gc --> The number of game components
@param HashMap --> How the game finds the connections
@return --> Returns what it describes to the user*/ 
char* hint2(unsigned long long gcLong, struct wordConnections **(*HashMap)){
	char* output = malloc(SIZE); 
	struct GameComponents *gc = (struct GameComponents*)gcLong; 
	if(gc->hc->hintPoints < gc->hc->hint2Weight){
		snprintf(output, SIZE, "Not enough hint points. Only %d hint points.\n", gc->hc->hintPoints);
	}
	else{
		int start = 0; 
		gc->hc->hintPoints -= gc->hc->hint2Weight; 
		
		//This goes through and find all of the acceptable words in the Hash Map output
		struct word* options = hashMapOutput_Restrictions(gc->prevInput, gc->hc->wordsGiven, gc->hc->hint2OutputQuantity, HashMap);
		//This creates a header so that when the options move, they can still be freed
		struct word* optionsHeader = options; 
		//Skips the header
		options = options->next; 
		
		//In order to get hte \0 to work with the string cat, we have to move the start back one, except for the first one 
		int notFirst = 0; 
		if(options == NULL){
			strcpy(output, "Hint -- I'd Recommend You Undo at least once.\n"); 
			start = 46; 
		}
		else{
			strcpy(output, "Hint -- Some options are: "); 
			start = strlen(output); 
			int wordOutLen = numLetters + 3; 
			int i; 
			//It loops through the outputs until the next one after the next one is null
			for(i = 0; i < (gc->hc->hint2OutputQuantity - 1) && options->next != NULL; i++){
				char* hintWord = malloc(wordOutLen); 
				snprintf(hintWord, wordOutLen, "%s, ", options->word);
				int prevStart = start; 
				start = safeStrcat(&output, (const char*) hintWord, SIZE, wordOutLen, start - notFirst);
				
				notFirst = 1; 
				options = options->next;
				free(hintWord);   
			}
				
			char* hintWord = malloc(wordOutLen); 
			snprintf(hintWord, wordOutLen, "%s.\n", options->word); 
			start =  safeStrcat(&output, (const char*) hintWord, SIZE, wordOutLen, start - notFirst);
			free(hintWord); 
			notFirst = 1; 
		}
		//This is the new number of points
		char* newPoints = malloc(50);
		snprintf(newPoints, SIZE, "You now have %d hint points\n", gc->hc->hintPoints);
		int size = strlen(newPoints); 
		start = safeStrcat(&output, (const char*)newPoints, SIZE, size, start - notFirst);   
		free(newPoints);  
		Free_WordLL(optionsHeader); 
		
 	}
	return output; 
	
}
/*Hint 3: Offers the user a letter that is used be from the first word to the last word
@param gc --> The current game componenents
@param HashMap --> How the user figures out a letter*/
char* hint3(unsigned long long gcLong, struct wordConnections **(*HashMap)){
	//This is the output 
	char* output = malloc(SIZE); 
	//Converts the long into a structure
	struct GameComponents *gc = (struct GameComponents*)gcLong; 
	struct HintComponents *hc = gc->hc; 
	char* currWord = gc->prevInput; 
	if(hc->hintPoints < hc->hint3Weight){
		snprintf(output, SIZE, "Not enough hint points. Only %d hintpoints.\n", gc->hc->hintPoints); 
	}
	//If have enough points
	else{
		
		hc->hintPoints -= hc->hint3Weight; 

		bool isFound = false; 
		//So, first I have to find if this has been added in the Generic Linked List
		hc->letterOptions = hc->letterOptions->next; 
		while(hc->letterOptions != NULL && isFound == false){
			//Check if the word is equal to the word being searched for
			if(strcmp(((struct hint3Struct*)(hc->letterOptions->listHeader))->word, currWord) == 0){
				//If it is equal stop the search
				isFound = true; 
		
			}
			else{ 
				//If it's not equal move on to the next one 
				hc->letterOptions = hc->letterOptions->next;
			}
		}
 
		
		//If there is no structure for this particular word, I need to
		if(isFound == false){
			init_hint3(hc, currWord, gc->goal, HashMap); 
		
		}	
		
		struct hint3Struct *h3 = ((struct hint3Struct*)(hc->letterOptions->listHeader));  
		//NOTE: I will not eliminating letters that are same as the current letter they have, because they can still figure out, "oh, I need to keep this letter"
		//Then, randomly choose one of the letters from the array 
		int numOptions = h3->letters->currPrecision; 
		//If letter3Storage->minConnections = 1 -- the word is only one connection away 
		if(h3->minConnections == 1){
			strcpy(output, "Take a closer look.\n"); 
		}
		else if(numOptions == 0){
			char* out = "Hint -- No more letters to give\n"; 
			int start = strlen(out);  
			char* newPoints = malloc(50);
			snprintf(newPoints, SIZE, "You now have %d hint points\n", gc->hc->hintPoints);
			int size = strlen(newPoints); 
			start = safeStrcat(&output, (const char*)newPoints, SIZE, size, start);  	
			free(newPoints); 
		}
		else{
			int choiceIndex = rand() % numOptions; 
			//The character the user will be given
			char hintChar = ((char*)(h3->letters->list))[choiceIndex]; 
			snprintf(output, SIZE, "Hint -- A letter to consider: %c\n", hintChar); 
			int start = 32; 
			char* hintCharPtr = &hintChar; 
			
			//Remove the hint from the array list
			remove_ArrayList(hintCharPtr, h3->letters, CHARACTER);
			
			char* newPoints = malloc(50);
			snprintf(newPoints, SIZE, "You now have %d hint points\n", gc->hc->hintPoints);
			int size = strlen(newPoints); 
			start = safeStrcat(&output, (const char*)newPoints, SIZE, size, start);  	
			free(newPoints); 
			 
		}
		 		

		
	
	}
	hc->letterOptions = hc->letterOptionsHeader; 
	return output; 
	
	
}

void Convert_TreeStorageNodeArrayList_HintRestrictions(struct arrayList* aList, bool* lettersGiven, struct TreeStorageNode *End){
	/*While the location is not equal to null, make the index go down*/
	/*The index starts at minConnection -1 , because if it started at minConection it would go 3:3 2:2 1:1, when it needs to be 3:2 2:1 1:0*/ 
	struct TreeStorageNode *currNode = End;
	
	//This goes to the next location
	currNode = currNode->prev; 
	
	//If the next node is null, we don't want to 
	if(currNode != NULL){
		
		//We want to stop before it reaches the starting word, we don't care about the starting word
		while(currNode->prev != NULL){ 
			char* currWord = currNode->word; 
			int i; 
			for(i = 0; i < numLetters; i++){
				//This index in the array of booleans
				int key = currWord[i] - 97; 
				//Make sure it's not in the Hash Set
				if(lettersGiven[key] == false){
					//A pointer to the character that is about to be added
					//Needed in order to make it a void*
					char* c = &currWord[i];
					add_ArrayList((void*)c, aList, CHARACTER); 
					lettersGiven[key] = true; 
				}
			}
			//This'll go through the word and add letters from the word to the Array List checking the Hash Set to make sure there are no repeat letters 
			currNode = currNode->prev; 
		
		} 
	}  	
}


void free_HintComponents(unsigned long long hcLong){
	struct HintComponents *hc = (struct HintComponents*)hcLong; 
	Free_HashSet(hc->wordsGiven);
	Free_GenericLinkedList(hc->letterOptionsHeader); 
	free(hc->lettersGiven); 
	free(hc);  
}




struct arrayList* BreadthFirstSearch_Dest_HintRestrictions(char* start, char* goal, struct wordConnections **(*HashMap), struct hint3Struct *hc, bool* HashSet, enum FoundWordStorage storageType){
	 
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
		
		bc->End = AddToTreeStorage_Dist_BFS(bc, goal, storageType,  HashMap);  
		if(strcmp(bc->End->word, goal) == 0){
			goalFound = true; 
		}

		
		if(bc->prevConnection == NULL){			  
			printf("\n%s cannot connect with %s\n", start, goal);
			goalFound = -1;
		}
		
		//The previous connection moves forward one, going to the next word whose connections should be searched
		
		
		
	}

	//This is how many connections it takes to get from the top of the list to the bottom 
	int length = bc->End->depth; 
	hc->minConnections = length; 
	/*This initializes the array list
	@component --> (length - 2) --> The length, not including the first one, nor the last one
	@param length -2 * numLetters --> This is the official size of the array list
	@return output --> This is the array list that'll store the letters that will be given to the user*/
	struct arrayList* output = init_ArrayList((length - 1) * numLetters, numLetters, STR);

	Convert_TreeStorageNodeArrayList_HintRestrictions(output, HashSet, bc->End);

	//Frees the structure
	Free_BFSComponents(bc, storageType); 
	
	return (goalFound == -1)?NULL:output; 
	
	 //Things to keep in mind
	 //When I add words, I don't want to add letters that are in the hash set meaning that I will have to: 
	 	//Program a method in that adds words in by character, and checks with the hash set to see if it can be added
	 	//Change the ArrayList method to make it so that when it adds, it skips the first and last words
	 	//If there are no words in between, that is, if the connection is direct
}

void init_hint3(struct HintComponents *hc, char* currWord, char* goal, struct wordConnections **(*HashMap)){
	//First, get back to the front of the generic linked list 
	hc->letterOptions = hc->letterOptionsHeader; 
	//Then, I have to add a new location (which I do believe gets added to the front)
	AddToFront_GenericLinkedListNode(hc->letterOptions, HINT3); 
	//Then, I have to allocate the structure
	//Get to the current location
	hc->letterOptions = hc->letterOptions->next; 
	hc->letterOptions->listHeader = malloc(sizeof(struct hint3Struct)); 
	((struct hint3Struct*)(hc->letterOptions->listHeader))->word = currWord; 
	//Then, find the available letters using the Breadth First Search Distance array and set that to the array list 
	((struct hint3Struct*)(hc->letterOptions->listHeader))->letters = BreadthFirstSearch_Dest_HintRestrictions(currWord, goal, HashMap, ((struct hint3Struct*)(hc->letterOptions->listHeader)), hc->lettersGiven, HASH_SET); 

	
}

//Things to Do: 
/*
Make sure it doesn't give any words the user has said  

*/


