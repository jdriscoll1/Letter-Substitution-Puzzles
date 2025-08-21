/*Title: WordHashSet.c
Author: Jordan Driscoll
Date: October 13th, 2021
Desc: A Hash Set for all words to determine if they are used*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "../includes/WordSet.h"

struct WordSet* init_WordSet(int totalWords) {
    int bitsPerBlock = sizeof(unsigned long) * CHAR_BIT;
    int arrLength = (totalWords + bitsPerBlock - 1) / bitsPerBlock;  // ceil division

    struct WordSet *wordSet = calloc(1, sizeof(struct WordSet)); // zero entire struct
    if (!wordSet) return NULL;

    wordSet->words = calloc(arrLength, sizeof(unsigned long));  // zero array
    if (!wordSet->words) {
        free(wordSet);
        return NULL;
    }

    wordSet->totalWords = totalWords;
    wordSet->arrLength = arrLength;

    return wordSet;
}

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUsed_WordSet(int wordID, struct WordSet *wordSet) {
    int bitsPerBlock = sizeof(unsigned long) * CHAR_BIT;

    if (wordID < 0 || wordID >= wordSet->totalWords) {
        fprintf(stderr, "markUsed_WordSet: wordID %d out of bounds (total=%d)\n",
                wordID, wordSet->totalWords);
        return;
    }

    int block = wordID / bitsPerBlock;
    int bit   = wordID % bitsPerBlock;

    if (block >= wordSet->arrLength) return; // extra safety

    unsigned long mask = 1UL << bit;
    wordSet->words[block] |= mask; // mark as used
}


void markUnused_WordSet(int wordID, struct WordSet *wordSet) {
    int bitsPerBlock = sizeof(unsigned long) * CHAR_BIT;

    if (wordID < 0 || wordID >= wordSet->totalWords) {
        fprintf(stderr, "markUnused_WordSet: wordID %d out of bounds (total=%d)\n",
                wordID, wordSet->totalWords);
        return;
    }

    int block = wordID / bitsPerBlock;
    int bit   = wordID % bitsPerBlock;

    if (block >= wordSet->arrLength) return; // extra safety

    unsigned long mask = 1UL << bit;
    wordSet->words[block] &= ~mask; // mark as unused
}

unsigned long checkIfUsed_WordSet(int wordID, struct WordSet *wordSet) {
    int bitsPerBlock = sizeof(unsigned long) * CHAR_BIT;

    if (wordID < 0 || wordID >= wordSet->totalWords) {
        fprintf(stderr, "checkIfUsed_WordSet: wordID %d out of bounds (total=%d)\n",
                wordID, wordSet->totalWords);
        return 0;
    }

    int block = wordID / bitsPerBlock;
    int bit   = wordID % bitsPerBlock;

    if (block >= wordSet->arrLength) return 0; // extra safety

    unsigned long mask = 1UL << bit;
    return (wordSet->words[block] & mask) != 0;
}

int ceil_div(int x, int y) {
  return (x + y - 1) / y;
}

/*Go through all the words and mark them unused*/
void reset_WordSet(struct WordSet* wordSet) {
    if (!wordSet || !wordSet->words) return;

    for (int i = 0; i < wordSet->arrLength; i++) {
        wordSet->words[i] = 0; // mark all bits as unused
    }
}


void print_WordSet(struct WordSet* wordSet){
	printf("WordSet:\n");
	unsigned int i; 
	for(i = 0; i < wordSet->totalWords / (sizeof(unsigned long) * CHAR_BIT); i++){
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
