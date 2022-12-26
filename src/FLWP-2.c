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
#include "./structs/includes/IntLinkedList.h"


#include "./flwp/includes/PathfinderGame.h"

#include "./flwg/includes/FLWGGame.h"
#include "./flwg/includes/MultiplayerFLWG.h"

void testAlpaBetaPruning(struct wordDataArray *IntToWord_HashMap); 
void runMultiplayerFLWG();
void runFLWG();
void testMCTS();
void initializeStructures();





/*
int main(){

	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	//char* path = "../docs/t/mini.txt"; 
	char* path = "../docs/4a.txt";
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet* wordSet = init_WordSet(5361);
	//FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet); 
	FLWG_Test(IntToWord_HashMap, wordSet);
	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	
	//nook --> veer
	int i1 = Convert_WordToInt("nook", WordToInt_HashMap);
	int i2 = Convert_WordToInt("veer", WordToInt_HashMap);
	struct intList* list = BreadthFirstSearch_Goal(i1, i2, IntToWord_HashMap, wordSet);
	char* output = toString_IntLL(list, LINKED, IntToWord_HashMap); 
	printf("%s", output);

	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//FLWP("dill", "toss", WordToInt_HashMap, IntToWord_HashMap, wordSet);
	
	free_WordSet(wordSet); 
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
	
	return 0;
}
*/










void testMCTS(){
	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	//char* path = "../docs/t/mini.txt"; 
	char* path = "../docs/4.txt";

	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet* wordSet = init_WordSet(IntToWord_HashMap->numWords);
	
	Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	/***************INITIALIZATION COMPLETE*******************/
	
	//This is the starting word
	//int rootID = 2000; 


	//int s = montyCarlosTreeSearch(rootID, wordSet, IntToWord_HashMap);

	//Simulates mcts

	/*for(run = 0; run < 1; run++){
		//Set the root ID to found 
		markUsed_WordSet(rootID, wordSet);


		int i; 
		for(i = 0; i < 10; i++){
		
			int s = montyCarlosTreeSearch(rootID, wordSet, IntToWord_HashMap);
		    printf("MCST Result: %d\n", s);
			int y = botPly(rootID, 15, IntToWord_HashMap, wordSet, minimax);
			printf("Optimal Result: %d\n", y);
			markUnused_WordSet(y, wordSet);
			printf("\n");
        }
		//printf("\nCorrect Result: %d", y);
	}*/
		
		
		
		
	

	//reset_WordSet(wordSet);

	/*Obtain the correct answer to be compared w/ the mcts results*/
	//int y = botPly(rootID, 15, IntToWord_HashMap, wordSet, minimax);
	
	//outputs the correct results
	//printf("\nCorrect Result: %d", y);
	

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
	
	FLWG_Test(IntToWord_HashMap, wordSet);
	
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
