extern int numLetters;
/*C input calls are not very secure
Name --> Jordan Driscoll
Date --> 1.11.21
Description --> This class is able to be readable by Java such that I can call functions from Java
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "jni.h"

#include "HashMap.h"

#include "Arrays.h"

#include "BreadthFirstSearch.h"

#include "GenericLinkedListNode.h"

#include "HashSet.h"

#include "HashFunctions.h"

#include "HashMap.h"

#include "UserInput.h"

#include "GameFunctions.h"



JNIEXPORT void JNICALL
Java_SafeInput_print(JNIEnv * env, jobject obj){
	printf("Test"); 
}

JNIEXPORT void JNICALL
Java_SafeInput_game(JNIEnv * env, jobject obj){
	
	srand(time(0)); 
	EnterText(); 
	
	
	struct wordConnections **(*HashMap) = AllocateHashMap();
	char** wordStorage = FillHashMap(HashMap, 0);

	
	char** allWords = ExtrapolateAllWords();
	
	int i = 2;
	int numRounds = 5;  
	int quit = 0; 
	for(i = 0; i < numRounds && quit == 0; i++){
	
		quit = PathfinderGame(i + 2, allWords, wordStorage, HashMap, env, obj, 1);
		 
	}
	
	//used to free the array
	int totalWordCount[3] = {30, 590, 2235}; 
	Free_2DArray(totalWordCount[numLetters - 2], (void***)allWords, 0); 
	FreeWordStorage(wordStorage); 
	FreeHashMap(HashMap); 
	
}
/*Plan: 
1) Figure out what Methods I need to go through to get to the input method
a) game
b) Pathfinder Game
c) Get Input
2) Figure out which classes need the jni.h 
a) JavaWrapper
b) GameFunctions
3) Figure out what paramters the methods would need to add
a) JNIEnv* env, jobject obj, boolean isJava
4) Figure out how to do Method Pointers that have varying parameters
5) Input the jni.h headers
6) Insert the method paramaters
7) Get the void pointer to work
8) Get it into the discord bot*/ 

