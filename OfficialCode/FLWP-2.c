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




#include "HashMap2.h"

#include "Arrays.h"

#include "BreadthFirstSearch.h"

#include "GenericLinkedListNode.h"

#include "HashSet.h"

#include "HashFunctions.h"

#include "UserInput.h"

#include "GameFunctions.h"

#include "PathGameComponents.h"

#include "ArrayList.h"

#include "DepthFirstSearch.h"


#include "PathfinderGame.h"

void WordConnections(); 
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
void PathfinderGameMain(); 
int ConnectionOptimization(); 
void FLWP(); 


int main(){
	srand(time(0)); 
	struct DummyHeadNode*** H = Create_HashMap();
	char** allWords = ExtrapolateAllWords();
	Play_FLWP(H, allWords); 
	Free_HashMap(H); 
	Free_2DArray(2235, (void***)allWords, 0);  
	return 0;  
}




void FLWP(){
	srand(time(0)); 
	struct DummyHeadNode*** H = Create_HashMap();
	char** allWords = ExtrapolateAllWords();
	char* s = ChooseStart(allWords, H, 0); 
	int i = 0; 
	
	struct word* l=  BreadthFirstSearch_Dest_WordLL("pies", "vine", H); 
	Print_WordLL(l, LINKED);  

	Free_HashMap(H); 
	Free_2DArray(2235, (void***)allWords, 0); 


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



