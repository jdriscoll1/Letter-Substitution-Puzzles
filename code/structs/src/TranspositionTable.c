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
#include "../../algs/includes/MaxN.h"

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
		for(i = 0; i < tt->arrLength; i++){
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

void addScore_TranspositionTable(int currWord, unsigned long hash, void* score, struct wordDataArray* IntToWord_HashMap, enum dataType scoreType){
	
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
		tt->arrLength = 10;
		tt->treeArray = malloc(sizeof(struct DummyHeadNode) * tt->arrLength);
		int i = 0;
		for(i = 0; i < tt->arrLength; i++){
			tt->treeArray[i] = NULL;
		}
		
	} 
	
	/*STEP #2*/
	struct savedScore* scoreStorage = init_savedScore(hash, score, scoreType); 
	int index = hash % tt->arrLength;

	//Then go to the array, to get there, mod the long by 10 and go to that index 
	//if it is null, then it is necessary to allocate the tree set 
	if(tt->treeArray[index] == NULL){
		tt->treeArray[index] = Allocate_TreeSet(scoreStorage);		
	}
	else{
		
		//Then, look at the AVL Tree, and insert this into it 
		if(AddNode_TreeSet(scoreStorage, tt->treeArray[index], tt->treeArray[index]->start, DUMMY, SCORE) == NULL){
			free_SavedScore(scoreStorage, scoreType);
		}
		
	}

	IntToWord_HashMap->array[currWord]->transpositionTable = tt;
	
	
}


struct savedScore* init_savedScore(unsigned long hash, void* savedScore, enum dataType scoreType){
	struct savedScore* savedScoreStorage = malloc(sizeof(struct savedScore));
	savedScoreStorage->hash = hash;
	//store it in memory just in case the pointer gets freed
	switch(scoreType){
		case(MINIMAX_SCORE):
			savedScoreStorage->savedScore = malloc(sizeof(struct minimaxOutput));
			copy_mo((struct minimaxOutput*)savedScoreStorage->savedScore, (struct minimaxOutput*)savedScore); 
			break;
	
		case(MAXN_SCORE):
			printf("Add MaxN_Score");
			break;
		
	}
	
	return savedScoreStorage; 
}

void* getScore_TranspositionTable(unsigned long hash, int wordID, struct wordDataArray* IntToWord_HashMap){
	
	struct TranspositionTable *tt = IntToWord_HashMap->array[wordID]->transpositionTable;
	if(tt == NULL){
		return NULL;
	}
	if(tt->treeArray == NULL){
		return NULL;
	}
	if(tt->treeArray[hash % tt->arrLength] == NULL){
		return NULL;
	}

	struct TreeSetNode* node = Search_TreeSet((void*)(&hash), tt->treeArray[hash % tt->arrLength]->start, SCORE_HASH);
	if(node == NULL){
		return NULL;
	}
	return (struct minimaxOutput*)((struct savedScore*)(node->data))->savedScore;
	
}

void free_TranspositionTable(struct TranspositionTable* tt, enum dataType scoreType){
	int i;
	if(tt != NULL){
	
		if(tt->treeArray != NULL){
		
			for(i = 0; i < tt->arrLength; i++){
				if(tt->treeArray[i] != NULL){
					Free_TreeSet(tt->treeArray[i]->start, scoreType);
					//Frees the dummy header node
					free(tt->treeArray[i]);
				}
			}
		
			free(tt->treeArray);
		}
		free(tt);
	}
}
void reset_TranspositionTable(struct wordDataArray* IntToWord_HashMap, enum dataType scoreType){
	int i;
	for(i = 0; i < IntToWord_HashMap->numWords; i++){
	
		free_TranspositionTable(IntToWord_HashMap->array[i]->transpositionTable, scoreType);
		IntToWord_HashMap->array[i]->transpositionTable = NULL;
	}
}

void free_SavedScore(struct savedScore* score, enum dataType scoreType){
	if(scoreType == MAXN_SCORE){
		//Free_MaxNNodeScore((struct maxnNodeScore*)score->savedScore, 5); 
	}
	if(scoreType == MINIMAX_SCORE){
		free(score->savedScore);
	}
	free(score);
}



