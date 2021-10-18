/*
Title: TranspositionTable.c
Author: Jordan Driscoll
Date: 10/17/21
Desc: The functions for the Transposition Tables
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/TranspositionTable.h"
#include "../includes/WordSet.h"

unsigned long update_GameStateHash(unsigned long hash, int id){
	unsigned long temp = 1;
	int x = (sizeof(unsigned long) * 8) - id; 
	int y = sizeof(unsigned long) * 8; 
	int z = x % y; 
	temp <<= z - 1; 

	return hash ^= temp;  
	
}
void print_TranspositionTable(struct TranspositionTable* tt){
	if(tt == NULL){
		printf("NULL\n");
	}
	else{
		int i = 0; 
		for(i = 0; i < 10; i++){
			printf("%d: ", i);
			if(tt->treeArray[i] != NULL){
				Print_TreeSet(tt->treeArray[i]->start, SCORE);
			}
			else{
				printf("NULL\n");
			}
		}
		
		
	}
	printf("\n\n");
}

void addScore_TranspositionTable(int currWord, unsigned long hash, void* score, struct wordDataArray* IntToWord_HashMap){
	
	/************
	1) Create the New Transposition Table 
		--> The Transposition Table is the Data Structure that Takes & Retrieves Saved Scores 
		--> If it is Null, it is necessary to allocate it (lazy instantiation)
	2) Create the Storage Node that contains the hash by which the score will be retrieved
		--> This storage node contains the hash and the score to be stored
	3) It then adds this to the tree
	**********/
	
	/*STEP #1*/
	//First, get to the right element of the IntToWord Array
	struct TranspositionTable* tt = IntToWord_HashMap->array[currWord]->transpositionTable; 
	
	
	//Check if it is null, if it is null, allocate space for it 
	if(tt == NULL){
		//allocate the transposition table
		tt = malloc(sizeof(struct TranspositionTable));
		//allocate the array 
		tt->treeArray = malloc(sizeof(struct DummyHeadNode) * 10);
		int i = 0;
		for(i = 0; i < 10; i++){
			tt->treeArray[i] = NULL;
		}
	} 
	
	/*STEP #2*/
	struct savedScore* scoreStorage = init_savedScore(hash, score); 
	int index = hash % 10;

	//Then go to the array, to get there, mod the long by 10 and go to that index 
	//if it is null, then it is necessary to allocate the tree set 
	if(tt->treeArray[index] == NULL){
		tt->treeArray[index] = Allocate_TreeSet(scoreStorage);		
	}
	else{
		//Then, look at the AVL Tree, and insert this into it 
		AddNode_TreeSet(scoreStorage, tt->treeArray[index], tt->treeArray[index]->start, DUMMY, SCORE);
	
	}

	IntToWord_HashMap->array[currWord]->transpositionTable = tt;
	
	
}

struct savedScore* init_savedScore(unsigned long hash, void* savedScore){
	struct savedScore* savedScoreStorage = malloc(sizeof(struct savedScore));
	savedScoreStorage->hash = hash;
	//store it in memory just in case the pointer gets freed
	memcpy(&(savedScoreStorage->savedScore), &savedScore, sizeof(void*)); 
	return savedScoreStorage; 
}

void* getScore_TranspositionTable(unsigned long hash, int wordID, struct wordDataArray* IntToWord_HashMap){
	
	struct TranspositionTable *tt = IntToWord_HashMap->array[wordID]->transpositionTable;
	if(tt == NULL){
		return NULL;
	}
	struct TreeSetNode* node = Search_TreeSet((void*)(&hash), tt->treeArray[hash % 10]->start, SCORE_HASH);
	if(node == NULL){
		return NULL;
	}
	return (struct minimaxOutput*)((struct savedScore*)(node->data))->savedScore;
	
}




