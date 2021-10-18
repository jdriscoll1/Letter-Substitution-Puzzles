/*
Title: TranspositionTable.h
Author: Jordan Driscoll
Date: 10/17/21
Description: Transposition Tables save scores of game states as they are come across, and remember them for future use
*/

#ifndef seenTranspositionTable
#define seenTranspositionTable

#include "../../algs/includes/Minimax.h"
#include "TreeSet.h"
#include "HashMap.h"

struct TranspositionTable{
	struct DummyHeadNode** treeArray; 
};

struct savedScore{
	unsigned long hash; 
	void* savedScore; 
};

/*this updates a hash to make it accurate to the new game state 
@param hash --> the hashed game state to be updated
@param wordID --> the word ID that has been mentioned
@return --> Returns the updated hash*/
unsigned long update_GameStateHash(unsigned long hash, int id);


/*
This adds a score to the transposition table 
@param currWord --> The current word that is in the array 
@param hash --> The hash that is being added to the table
@param score --> The score that is being saved into the table 
@param IntToWord_HashMap --> The array that stores all of the transposition tables*/
void addScore_TranspositionTable(int currWord, unsigned long hash, void* score, struct wordDataArray* IntToWord_HashMap);

/*Given a hash, output the associated score
@param hash --> The hash used to retrieve the score
@param wordID --> The word ID identifying which Transposition Table was used
@param IntToWord_HashMap --> The data structure which stores all transpostion tables
@return --> Returns a void pointer which can be converted to a score, for example minimaxOutput*/
void* getScore_TranspositionTable(unsigned long hash, int wordID, struct wordDataArray* IntToWord_HashMap);

/*Initializes the saved score
@param hash --> How it is accessed and stored by the Tree Set
@param savedScore --> The score that is saved*/
struct savedScore* init_savedScore(unsigned long hash, void* savedScore);
#endif


