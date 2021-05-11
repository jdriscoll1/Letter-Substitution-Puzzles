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

#include "Arrays.h"
#include "HashMaps.h"
#include "TreeSet.h"
#include "BreadthFirstSearch.h"
#include "PathfinderGame.h"
#include "Minimax.h"
#include "FLWGGame.h"

/*Test 1: Creating and Freeing the Hash Map 5,000 times: 
a) 35.99, 40.51, 38.78*/
/*Test 2: Doing a 13 deep search on it 100,000 times
66.96, 64.52, 63.71
Test 3: Running a test on it 34.40 Megabytes*/

int main(){
    srand(time(0)); 
    
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	IntToWord_HashMap = malloc(sizeof(struct wordDataArray));  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
	
	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap); 
	FLWG(WordToInt_HashMap, IntToWord_HashMap);
	//int depth = 10;  
	//struct minimaxOutput* output = minimax(0, depth, depth, 1, IntToWord_HashMap); 
	
	//printf("\n\nFinal Choice: %d", output->id); 

	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap); 

	return 0;  
}



