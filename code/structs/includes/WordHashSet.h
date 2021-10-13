/*
Title: WordHashSet.h
Author: Jordan Driscoll
Date: 10/13/21
Desc: A Hash Set for all words to determine if they are used
*/


/*This is the storage for words that have and have not been used */
struct WordHashSet{
	//the array of all words, each word takes up a byte
	unsigned long* wordSet; 
	//the total number of words
	int totalWords; 
};

/*Allocates the structure and creates all of the necessary longs*/
struct WordHashSet* init_WordHashSet(int totalWords);

/*Function that marks a word as used or unused
@param wordID --> The word that is being marked used or unused
@param isUsed --> 1 - the word was just seen 0 - the word is being marked as unseen*/
void markUsed_WordHashSet(int wordID, int isUsed, struct WordHashSet *whs);

/*Check if word is used*/
void checkIfUsed_WordHashSet(int wordID, struct WordHashSet *whs);

/*Go through all the words and mark them unused*/
void reset_WordHashSet(int wordID, struct WordHashSet* whs);

/*frees hash set of all words*/
void free_WordHashSet(struct WordHashSet* whs);
