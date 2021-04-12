#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FLWGAlgorithm.h"
#include "HashMap2.h"

typedef int bool; 
#define true 1
#define false 0
/*This is C Document that holds the code for the FLWG
Date: Mar 31, 2021
Author: Jordan Driscoll*/

//Private Method: This takes a word, and adds all of it's connections to the tree storage node, until it finds a matching word
struct TreeStorageNode* AddToTreeStorage_Connections(char* currWord, struct BFSComponents* bc, struct word*** HashSet, struct DummyHeadNode*** HashMap); 

int Search_TreeStorageNode(char* word, struct TreeStorageNode *currNode); 



void FLWG(){
	//So, when playing the four letter word game, a couple of things will have to happen: 
		//1) It will create the Hash Set 
		//2) It'll randomly choose a word
		//3) It will add the word to the hash set
		//4) The User Will will have a turn
		//5) The Robot Will Have a turn
		//6) This will continue until there is no where to go 
		//How to determine no where to go? --> Either the user will quit, or the bot will state that he's been defeated 
	
	
	
}


//Right now, it should only return a word that has no connecting words 
//It should take as input the Hash Set, Hash Map and the previous word
struct TreeStorageNode* FLWGAlgorithm(char* prevWord, struct word ***HashSet, struct DummyHeadNode*** HashMap){
	//These are the breadth first search components
	struct BFSComponents* bc = init_BFSComponents(prevWord);
	
	//The goal is to find a node that has 0 connections
	bool goalFound = false; 
	
	char* currWord = prevWord; 
	
	bc->prevConnection = bc->prevConnection->next; 
 	int numSucc = 0; 
	//Then, we want to go out, until we find a word that has 0 connections, or have no words left 
	while(bc->prevConnection != NULL){
 
		currWord = bc->prevConnection->word;
	
		//We will add the words to the Tree Storage List
		
		bc->End = AddToTreeStorage_Connections(currWord, bc, HashSet, HashMap); 
		if(bc->End->noConnections == 1){
			Remove_HashMap(bc->prevConnection->word, HashMap); 
			
			numSucc++;
			Print_HashMap(HashMap); 	
	
		}
		 

		
		bc->prevConnection = bc->prevConnection->next; 
			
		

	}
	
	return NULL; 
	
}

//This is the function that will go out and add words to the breadth first search, until it can find a word that has 0 connections
struct TreeStorageNode* AddToTreeStorage_Connections(char* currWord, struct BFSComponents* bc, struct word*** HashSet, struct DummyHeadNode*** HashMap){
	const char* start = currWord; 
	//First, we'll obtain all of the words that can be connected 
	struct word* connections = getList((const char*)currWord, HashMap); 
	//We'll also need an integer that tracks the numebr of connections
	int numConnections = 0; 
	connections = connections->next; 
	//We'll loop through the words and check if it has been added in 2 places
	 while(connections != NULL){
		currWord = connections->word; 
		
		//Place 1: The Overall Hash Set
		if(Search_HashSet(currWord, HashSet) == 0){
			//Place 2: The Smaller Hash Set
			if(Search_TreeStorageNode(currWord, bc->prevConnection) == 0){
			
				//it will be added to the Tree Set Node
				bc->End = Add_TreeStorageNode(currWord, bc->prevConnection, bc->End, 0); 
				//If it is in neither hash set, it will add it to the local hash set
		
				//The number of connections this node has will go up 
				numConnections++; 
			}
			
		}
		
		connections = connections->next; 
		
		
		//If it has been added in either of those places, it will give it a skip
		//Otherwise, it will add it in the smaller hash set
		//If it is added in, the number of connections will go up 
		//At the end, it will return itself	 	
	 	
	 	
	 }

	
	//If we find a word that has no	connections, that is the numConnections is 0 by the end, we'll return NULL
	if(numConnections == 0){
		  
		 
		bc->prevConnection->noConnections = 1; 
		/*I need to return the previous connection, because it is at the location where the start word currently is,
		 and it is necessary to send that location. If we were to send the End, it would not work for 2 reasons
		1) The End is whereever the last added node happened to be, and it would not be here*/
		return bc->prevConnection; 
		 
	}
 
	return bc->End; 
	
}
//This function takes a word, and goes through the previous links in that particular branch of the tree storage to see if it has already been used
//It is good if the program wants to go through every possible path without going backwards
int Search_TreeStorageNode(char* word, struct TreeStorageNode *currNode){
	//if the node is null, is cannot be found
	if(currNode == NULL){
		return 0; 
	}

	//if the word matches the node's word, the word has been found
	if(strcmp(word, currNode->word) == 0){
		return 1; 
	}
	//If the word is not found, and the node is not null, that means that there is another node that may store the goal word
	return Search_TreeStorageNode(word, currNode->prev); 
}
