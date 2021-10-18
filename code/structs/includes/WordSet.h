/*
Title: WordHashSet.h
Author: Jordan Driscoll
Date: 10/13/21
Desc: A Hash Set for all words to determine if they are used
*/

#ifndef seenWordSet
#define seenWordSet


/*This is the storage for words that have and have not been used */
struct WordSet{
	//the array of all words, each word takes up a byte
	unsigned long* words; 
	//the total number of words
	int totalWords; 
};

/*Allocates the structure and creates all of the necessary longs*/
struct WordSet* init_WordSet(int totalWords);

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUsed_WordSet(int wordID, struct WordSet *wordSet);

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUnused_WordSet(int wordID, struct WordSet *wordSet);

/*Check if word is used*/
long unsigned int checkIfUsed_WordSet(int wordID, struct WordSet *wordSet);

/*Go through all the words and mark them unused*/
void reset_WordSet(struct WordSet* wordSet);

/*frees hash set of all words*/
void free_WordSet(struct WordSet* wordSet);

//outputs a long as binary
void long2binary(unsigned long hash);

#endif
