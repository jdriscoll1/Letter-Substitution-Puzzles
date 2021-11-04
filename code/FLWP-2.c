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


int main(){
	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	char* path = "../docs/t/ab.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);

	struct WordSet* wordSet = init_WordSet(IntToWord_HashMap->numWords);
	int i = 0; 
	markUsed_WordSet(2, wordSet);
	markUsed_WordSet(3, wordSet);
	for(i = 0; i < 10; i++){
		int x = montyCarlosTreeSearch(0, wordSet, 1, IntToWord_HashMap);
		printf("%s\n", Convert_IntToWord(x, IntToWord_HashMap));
	}
	
	return 0;
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

	int d = 4; 
	int n = 2; 
	
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
