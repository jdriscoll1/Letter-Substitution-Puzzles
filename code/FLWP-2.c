/**********************************
Name: Jordan D. 
Date: Feb 2, 2020
Purpose: The four letter pathfinder, this time better 
**********************************/
int numLetters = 4; 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "./algs/includes/Minimax.h"
#include "./algs/includes/MinimaxTests.h"
#include "./algs/includes/BreadthFirstSearch.h"
#include "./algs/includes/MaxN.h"
#include "./algs/includes/MontyCarlosTreeSearch.h"

#include "./structs/includes/HashMap.h"
#include "./structs/includes/TreeSet.h"
#include "./structs/includes/WordSet.h"


#include "./flwp/includes/PathfinderGame.h"

#include "./flwg/includes/FLWGGame.h"
#include "./flwg/includes/MultiplayerFLWG.h"

void testAlpaBetaPruning(struct wordDataArray *IntToWord_HashMap); 
void runMultiplayerFLWG();
void runFLWG();
void testMCTS();
void initializeStructures();


struct t{int w;int c;};



int main(){
	testMCTS();
	
	return 0;
}

void testMCTS(){
	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet* wordSet = init_WordSet(IntToWord_HashMap->numWords);


	/***************INITIALIZATION COMPLETE*******************/
	
	int rootID = 2000; 
	
	/*The # of connections the root word has*/
	int n = IntToWord_HashMap->array[rootID]->numConnections; 
	
	/*The data structure that stores the mcts results*/
	struct t chosenCount[n];

	
	int i;
	
	/*All of the nodes connecting to the root */
	struct intList* o = IntToWord_HashMap->array[rootID]->connectionHeader->next; 
	
	//Initializes the results structure
	for(i = 0; i < n; i++){
		//gives the chosen word its id 
		chosenCount[i].w = o->data;
		//# of times chosen initially = 0 
		chosenCount[i].c = 0;

		//goes to the next word ID to be inserted 
		o = o->next; 
	} 
	

	int numRuns = 500; 
	
	//Runs MCTS numRuns times
	for(i = 0; i < numRuns; i++){
		
		//Updates user on how many games it has played 
		if(i % ((int)(numRuns / 10)) == 0){
			printf("%d", i);
		}
		//Simulates mcts
		int x = montyCarlosTreeSearch(rootID, wordSet, IntToWord_HashMap);
		
		//updates the chosen word in the data structure
		chosenCount[x].c++;
	
	}
	
	//Once it has finished simulating mcts
	
	//Outputs the results
	printf("\n\nRESULTS: \n");
	
	//outputs all of the elements in the output data structure
	for(i = 0; i < n; i++){
		printf("MCTS: %d: %d\n", chosenCount[i].w, chosenCount[i].c);
		
	
		
		
		
		
	}



	/*Obtain the correct answer to be compared w/ the mcts results*/
	int y = botPly(rootID, 10, IntToWord_HashMap, wordSet, minimax);
	
	//outputs the correct results
	printf("\nCorrect Result: %d", y);
	

	/***************FREEING BEGINS*********************/
	free_WordSet(wordSet); 
	
	//Frees the hash maps
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
	
	
}

void runMultiplayerFLWG(){
	srand(time(0)); 
	
	
    
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);

	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);

	
	Multiplayer_FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//MultiplayerTest(IntToWord_HashMap);
	
	free_WordSet(wordSet);
	
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);

	
}
void testAlpaBetaPruning(struct wordDataArray *IntToWord_HashMap){
	struct WordSet* wordSet = init_WordSet(IntToWord_HashMap->numWords);
	int depth = 4;  
	struct minimaxOutput* a = createOutput(-100, 0, -1, -1); 
	struct minimaxOutput* b = createOutput(100, 1, -1, -1); 
	struct minimaxOutput* output = minimax(0, depth, depth, 1, *a, *b, IntToWord_HashMap, wordSet); 
	printf("Final Choice: %d", output->id); 
	
	free(output);
	free(a);
	free(b); 
	free_WordSet(wordSet);
}

void runFLWG(){
		srand(time(0));
	
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word
	struct wordDataArray *IntToWord_HashMap; 
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	//Allocates the Int2Word HashMap
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	//Initializes both hash maps, filling them out with their paths and words
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);
	//FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//int s  = ChooseStart(IntToWord_HashMap);
	//printf("%s", Convert_IntToWord(s, IntToWord_HashMap));
	FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	
	free_WordSet(wordSet); 
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
}

void initializeStructures(){
	
	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);
	
	
	/**********INSERT CODE HERE*********************/
	
	free_WordSet(wordSet); 
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);

}
