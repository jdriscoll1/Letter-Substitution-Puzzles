/*Title: WordHashSet.c
Author: Jordan Driscoll
Date: October 13th, 2021
Desc: A Hash Set for all words to determine if they are used*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../includes/WordSet.h"

#define NUM_BYTES 8

/*Allocates the structure and creates all of the necessary longs*/
struct WordSet* init_WordSet(int totalWords){
	//the total number of word blocks
	int arrLength = ceil((double)totalWords / (double)(sizeof(unsigned long) * NUM_BYTES));

	//the hash set that contains whether all words have been, or have not been used 
	struct WordSet *wordSet = malloc(sizeof(struct WordSet));
	wordSet->words = calloc(arrLength, sizeof(unsigned long)); 
	int i; 
	for(i = 0; i < arrLength; i++){
		//setting all the words to be unused
		wordSet->words[i] = 0; 
	}
	wordSet->totalWords = totalWords; 
	return wordSet; 
	
}

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUsed_WordSet(int wordID, struct WordSet *wordSet){
	int numBytes = sizeof(unsigned long) * NUM_BYTES;
	//First get the block that the word is in 
	int block = (int)(wordID / numBytes);
	int byte = wordID % numBytes;
	//Then, from that block set the particular byte to either 0 or 1, or set it to isUsed
	//Places the 1 all the way on the right
	unsigned long temp = 1; 
	//Shifts that 1 to 64 - bytes. If it was 0, it would be all hte way on the left, so this has to get pushed to the left
	
	//it has to be -1 because otherwise it will not be 0-index, it will be 1-index
	temp <<= (numBytes - byte - 1); 

	//Then it does a bitwise OR on the two operators, and the word that was used should be set to TRUE 
	wordSet->words[block] |= temp; 
	

}

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUnused_WordSet(int wordID, struct WordSet *wordSet){
	int numBytes = sizeof(unsigned long) * NUM_BYTES;
	//First get the block that the word is in 
	int block = (int)(wordID / numBytes);
	//the index at which the byte is at
	int byte = wordID % numBytes;
	//Then, from that block set the particular byte to either 0 or 1, or set it to isUsed
	//Places the 1 all the way on the right
	unsigned long temp = 1; 
	//Shifts that 1 to 64 - bytes. If it was 0, it would be all hte way on the left, so this has to get pushed to the left
	
	//it has to be -1 because otherwise it will not be 0-index, it will be 1-index
	temp <<= (numBytes - byte - 1); 

	//Then it does a bitwise OR on the two operators, and the word that was used should be set to TRUE 
	wordSet->words[block] &= ~temp; 
	
	
}

/*Outputs if a word has been used*/
long unsigned int checkIfUsed_WordSet(int wordID, struct WordSet *wordSet){
	int numBytes = sizeof(unsigned long) * NUM_BYTES;
	//First get the block that the word is in 
	int block = (int)(wordID / numBytes);
	//the index at which the byte is at
	int byte = wordID % numBytes;
	//Then, from that block set the particular byte to either 0 or 1, or set it to isUsed
	//Places the 1 all the way on the right
	unsigned long temp = 1; 
	//Shifts that 1 to 64 - bytes. If it was 0, it would be all hte way on the left, so this has to get pushed to the left
	
	//it has to be -1 because otherwise it will not be 0-index, it will be 1-index
	temp <<= (numBytes - byte - 1);

	//This returns whether or not the and results in true. 
	//If the index at which the byte was moved is True: True
	//If the index at which the byte was moved is False: False
	return wordSet->words[block] & temp; 
	
}

int ceil_div(int x, int y) {
  return (x + y - 1) / y;
}

/*Go through all the words and mark them unused*/
void reset_WordSet(struct WordSet* wordSet){
	for(unsigned int i = 0; i < (unsigned int)ceil_div(wordSet->totalWords, (sizeof(unsigned long) * NUM_BYTES)); i++){
		wordSet->words[i] = 0;
	}
		
}
void print_WordSet(struct WordSet* wordSet){
	printf("WordSet:\n");
	unsigned int i; 
	for(i = 0; i < wordSet->totalWords / (sizeof(unsigned long) * NUM_BYTES); i++){
		printf("%ld\n", wordSet->words[i]);
	}
	
}
/*frees hash set of all words*/
void free_WordSet(struct WordSet* wordSet){
	free(wordSet->words);
	free(wordSet);
}




void long2binary(unsigned long hash){
        unsigned long f = floor(pow(2, sizeof(unsigned long) * 8) / 2);
        unsigned int i = 0;
        for(i = 0; i < sizeof(unsigned long) * 8; i++){
                printf("%i", hash & f ? 1 : 0);
                f >>= 1;

        }
        printf("\n");




}
