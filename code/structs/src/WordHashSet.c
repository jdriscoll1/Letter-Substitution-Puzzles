/*Title: WordHashSet.c
Author: Jordan Driscoll
Date: October 13th, 2021
Desc: A Hash Set for all words to determine if they are used*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../includes/WordHashSet.h"

#define NUM_BYTES 8

/*Allocates the structure and creates all of the necessary longs*/
struct WordHashSet* init_WordHashSet(int totalWords){
	//the total number of word blocks
	int arrLength = ceil(totalWords / (sizeof(unsigned long) * NUM_BYTES));
	
	//the hash set that contains whether all words have been, or have not been used 
	struct WordHashSet *whs = malloc(sizeof(struct WordHashSet));
	whs->wordSet = calloc(arrLength, sizeof(unsigned long)); 
	int i; 
	for(i = 0; i < arrLength; i++){
		//setting all the words to be unused
		whs->wordSet[i] = 0; 
	}
	whs->totalWords = totalWords; 
	return whs; 
	
}

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUsed_WordHashSet(int wordID, int isUsed, struct WordHashSet *whs);

/*Check if word is used*/
void checkIfUsed_WordHashSet(int wordID, struct WordHashSet *whs);

/*Go through all the words and mark them unused*/
void reset_WordHashSet(int wordID, struct WordHashSet* whs);

/*frees hash set of all words*/
void free_WordHashSet(struct WordHashSet* whs){
	free(whs->wordSet);
	free(whs);
}
