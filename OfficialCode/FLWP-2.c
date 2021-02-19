/**********************************
Name: Jordan D. 
Date: Feb 2, 2020
Purpose: The four letter pathfinder, this time better 
**********************************/
int numLetters = 4; 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#include "HashMap.h"

#include "Arrays.h"

#include "BreadthFirstSearch.h"

#include "GenericLinkedListNode.h"

#include "HashSet.h"

#include "HashFunctions.h"

#include "HashMap.h"

#include "UserInput.h"

#include "GameFunctions.h"

#include "PathGameComponents.h"

#include "ArrayList.h"

#include "DepthFirstSearch.h"

#include "ConnectionDistanceStats.h"

#include "PathfinderGame.h"

void WordConnections(); 
void MainProgram(char* w1, char* w2, struct wordConnections ***HashMap); 
void GenericLLExample(); 
void WordLLTest();
void HashSetExample(); 
void OneDimArray(); 
void CreateBasicWordLL(); 
void RandomizeArrayExample(); 
void RandomizedList(); 
void WeBeginTheGame(); 
void AllConnections(); 
void AVLTree(); 
void FreeMainComponents(struct wordConnections*** HashMap, char** wordStorage, char** allWords); 
void PathfinderGameMain(); 
int ConnectionOptimization(); 
void FLWP(); 


int main(){  
	int length = 10; 
	int* a = FillRange_Array(0, length, 0);
	struct DummyHeadNode *tree = Allocate_TreeSet((void*)a); 
	int i = 0; 
	for(i = 1; i < length; i++){
	
		printf("%d\n", (void*)&(a+i)); 
	} 
	Print_TreeSet(tree->start, INT); 
	free(a); 
	Free_TreeSet(tree->start); 
	free(tree); 
	return 0;  
}



void FLWP(){
	srand(time(0)); 
	struct wordConnections **(*HashMap) = AllocateHashMap();	
	char** wordStorage = FillHashMap(HashMap, 0);
	char** allWords = ExtrapolateAllWords();
	Play_FLWP(HashMap, wordStorage, allWords); 
	//Play_FLWP(HashMap, wordStorage, allWords); 
	FreeMainComponents(HashMap, wordStorage, allWords); 

}
//So, I need to get a mean, and the upper and lower quartile 
//What I have to do, Is I have to run through all of the words, and all of their number of connections from 2 to 15 and find
//Now, how in the hell am I going to be able to redo a round? 
//I have to ask the user if they'd like to move on to the next round
//They will have 3 options, yes, no, or redo
//If they choose yes, it will move them along
//If they choose no, it will end the game
//If they choose retry, it'll re-initialize the game Components, but this time the score and the goal will be the old score and also goal 
//So, I think I have no choice but to create a structure that stores everything 
//it'll store their score
//The the round they are on
//Their total score
void MainProgram(char* w1, char* w2, struct wordConnections ***HashMap){

	struct word *connections;  
	
	connections = (struct word*)BreadthFirstSearch_Dest_WordLL(w1, w2, HashMap, HASH_SET); 
	printf("\n"); 
	if(connections != NULL){
		Print_WordLL(connections, LINKED); 
		Free_WordLL(connections);
	
	}

	
}

void RandomizedList(){
	srand(time(0)); 
	int i; 
	for(i = 0; i < 100; i++){
	struct wordConnections **(*HashMap) = AllocateHashMap();
	char** wordStorage = FillHashMap(HashMap, 1); 
	struct word* connection = DFS_Lobby("pies", "best", HashMap); 
	if(connection != NULL){
		Print_WordLL(connection, LINKED);
		Free_WordLL(connection); 
	}
	FreeWordStorage(wordStorage); 
	FreeHashMap(HashMap);
	}

}


void WordLLTest(){
	struct word *header = malloc(sizeof(struct word)); 
	header->next = NULL; 
	AddToBack_WordLL("pies", header, 0);
	AddToBack_WordLL("ties", header, 0); 
	AddToBack_WordLL("lies", header, 0); 
	AddToFront_WordLL("dies", header, 0); 
	Remove_WordLL("pies", header); 
	Print_WordLL(header, SEPERATED); 
	Free_WordLL(header);   
	
}
void WordConnections(){
	struct wordConnections **(*HashMap) = AllocateHashMap();
	char** wordStorage = FillHashMap(HashMap, 0);
	printf("pies:\n"); 
	Print_WordLL(hashMapOutput("pies", HashMap), LINES); 
}

void OneDimArray(){
	int* array = (int*)Allocate_Array(5);  
	int i = 0; 
	/*From Here*/ 
	for(i = 0; i < 5; i++){
		*((int*)array + i) = i; 
	}
	
	/*To Here, without this snippit of code it works perfectly*/
	free(array);
}

void CreateBasicWordLL(){
	struct word *testList = malloc(sizeof(struct word)); 
	testList->next = NULL; 
	AddToBack_WordLL("pies", testList, 0);
	AddToBack_WordLL("dies", testList, 0);
	AddToBack_WordLL("lies", testList, 0);
	AddToBack_WordLL("ties", testList, 0);
	AddToBack_WordLL("vies", testList, 0);
	AddToBack_WordLL("yies", testList, 0);


	char** array = ConvertWordLLTo2DArray(testList);
	Print_2DArray(6, (void***)array, STRING); 
	 
	Free_2DArray(6, (void***)array, 0); 
	Free_WordLL(testList);  
}

void RandomizeArrayExample(){
	srand(time(0)); 
	char** array = (char**)Allocate_2DArray(6, numLetters + 1);
	strcpy(*(array + 0), "pies");
	strcpy(*(array + 1), "dies"); 
	strcpy(*(array + 2), "lies"); 
	strcpy(*(array + 3), "vies"); 
	strcpy(*(array + 4), "aids");
	strcpy(*(array + 5), "pigs");   
	//for loop that makes it randomize itself several times
	

	char** array2 = (char**)Randomize_2DArray(6, numLetters + 1, (void***)array, STRING); 
	Print_2DArray(6, (void***)array2, STRING); 
	printf("\n");
	free(array);   
	free(array2); 
}


void AllConnections(){
		srand(time(0));
	int i;  
	for(i = 0; i < 100; i++){
	struct wordConnections **(*HashMap) = AllocateHashMap();
	char** wordStorage = FillHashMap(HashMap, 1);  
	struct word *connections;  
	
	connections = DFS_Lobby("pies", "west", HashMap); 

	if(connections != NULL){
		printf("\n"); 
		Print_WordLL(connections, LINKED); 
		Free_WordLL(connections);
	
	}

	FreeWordStorage(wordStorage); 
	FreeHashMap(HashMap);
	}
}

void AVLTree(){
	srand(time(0)); 
	int min = 0; 
	int max = 200; 
	char* x[] = {"pies", "ties", "lies", "dies"}; 
	int length = sizeof(x) / sizeof(char*);  
	 
	struct DummyHeadNode *header = Allocate_TreeSet((void*)(*x)); 
	int i = 0; 
	for(i = 1; i < length; i++){ 
		AddNode_TreeSet((void*)*(x+i), (void*)header, header->start, DUMMY, INTEGER);
		 
	}
	printf("Originally: "); 
	Print_TreeSet(header->start, WORD);
	printf("\n"); 
	char* o[] = {"ties", "west"};
	char **p = o;
	int lengthRem = sizeof(o)/sizeof(char*);  
	for(i = 0; i < lengthRem; i++){
		
		Remove_TreeSet((void*)*(p + i), header, header->start, DUMMY, WORD); 
	}

	printf("After Removal: "); 
	Print_TreeSet(header->start, WORD);
	
 
 	printf("\n\n  RESULTS:\n"); 
	printf("  Balanced: %d", checkBalance(header->start)); 

	printf("\n  Header: %s. Depth: %d", (char*)header->start->data, header->start->depth); 
	printf("\n  Header->greater: %s. Depth: %d", (char*)header->start->greater->data, header->start->greater->depth); 
	printf("\n  Header->smaller: %s. Depth: %d", (char*)header->start->smaller->data, header->start->smaller->depth); 
	//printf("\n  Header->smaller->smaller: %d. Depth: %d", *(int*)header->start->smaller->smaller->data, header->start->smaller->smaller->depth);
	//printf("\n  Header->smaller->greater: %d. Depth: %d", *(int*)header->start->smaller->greater->data, header->start->smaller->greater->depth); 
	//printf("\n  Header->greater->greater: %d. Depth: %d", *(int*)header->start->greater->greater->data, header->start->greater->greater->depth);
	//printf("\nHeader->greater->smaller: %d", *(int*)header->start->greater->smaller->data); 
	//printf("\nHeader->smaller->smaller->greater: %d", *(int*)header->start->smaller->smaller->greater->data);
	//printf("\nHeader->smaller->smaller->smaller: %d", *(int*)header->start->smaller->smaller->smaller->data);
	//printf("\nHeader->smaller->greater->smaller: %d", *(int*)header->start->smaller->greater->smaller->data);
	//printf("\nHeader->smaller->greater->greater: %d. Depth: %d", *(int*)header->start->smaller->greater->greater->data, header->start->smaller->greater->greater->depth); 
	//printf("\nHeader->greater->greater->smaller: %d", *(int*)header->start->greater->greater->smaller->data); 
	//printf("\nHeader->greater->greater->greater: %d", *(int*)header->start->greater->greater->greater->data);
	//printf("\nHeader->greater->smaller->smaller: %d", *(int*)header->start->greater->smaller->smaller->data);
	
	//printf("\nHeader->greater->smaller->greater: %d", *(int*)header->start->greater->smaller->greater->data);    
	//printf("\nHeader->greater->smaller->smaller->smaller: %d", *(int*)header->start->greater->smaller->smaller->smaller->data);  
	//printf("\nHeader->greater->smaller->smaller->greater: %d", *(int*)header->start->greater->smaller->smaller->greater->data);  
	//printf("\nHeader->greater->greater->greater->greater: %d", *(int*)header->start->greater->greater->greater->greater->data);
	//printf("\nHeader->greater->greater->greater->smaller: %d", *(int*)header->start->greater->greater->greater->smaller->data);
	//printf("\nHeader->smaller->smaller->smaller->greater: %d", *(int*)header->start->smaller->smaller->smaller->greater->data); 
	//printf("\nHeader->smaller->smaller->greater->smaller: %d", *(int*)header->start->smaller->smaller->greater->smaller->data); 
	//printf("\nHeader->smaller->greater->greater->greater: %d", *(int*)header->start->smaller->greater->greater->greater->data); 
	//printf("\nHeader->smaller->greater->greater->smaller: %d", *(int*)header->start->smaller->greater->greater->smaller->data); 
	
	Free_TreeSet(header->start);
	free(header);
	
	
}

	
void FreeMainComponents(struct wordConnections*** HashMap, char** wordStorage, char** allWords){
	//used to free the array
	int totalWordCount[3] = {30, 590, 2235}; 
	Free_2DArray(totalWordCount[numLetters - 2], (void***)allWords, 0); 
	FreeWordStorage(wordStorage); 
	FreeHashMap(HashMap); 
	
}
/*NEW PLAN: 
- We need to make FLWP
 - Take User Input
  - Start
  - End Game/Q
  - Help
  - Add Word
  - Remove Word
  - Undo
  -  
 - Win Condition --> If the word is identical to the goal 
 
 - Lose Condition --> Non existant (no timer)
 - First, what should the game look like
  - It should Open up and say, welcome to the four letter word game
  - Type (s)tart to begin, q, or end game to quit, or (h)elp for help 
  - If you type help, it should give you a readable help link that will give you all the commands 
  - If you type quit, the program should shut down
  - If you type start, it should give you the difficulty option. Depending on whether the user types 0, 1, or 2 will determine the connections
  - Then, the game will say, connect w1 to w2
  - You will be given commands
  - You can either do: add, remove, end game or undo 
   - Add <word>: 
    - The word will be added, and it will print out the list 
    - Checks: Make sure the word is exactly three letters different than the previous
              Make sure it is exactly numLetters Letters
              
   - Remove <word>: 
    - The word will be removed, and all words after it will be removed 
    - Make sure that the word actually exists 
  
   - End Game: 
    - The game will end, and everything will be freed 
  
   - Undo
    - It will undo the previous action, this will repeat until the very first move 
    
    
    Plan of Attack: 
     First, get the game playable before you do a start menu 
      First, Add a "get user input"
       - Implement add word
       - Check The Word to make sure it is available
       - Implement remove word
       - Implement end game
       - Implement undo 

  
*/ 


