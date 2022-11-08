%module flwg_php
%{

/*The data structures which allow the game to work*/
#include "../../src/structs/includes/HashMap.h"

/*Allows the word set to be created & manipulated*/
#include "../../src/structs/includes/WordSet.h"

/*The algorithms which allow the game to be played*/
#include "../../src/flwg/includes/FLWGGame.h"

/*Specific functions for the four letter word game*/
#include "../../src/flwp/includes/GameFunctions.h"

#include "php_wrapper.h"
%}

/*********************WRAPPED FUNCTION*********************/
void ChooseFirstWord(struct wordDataArray* IntToWord_HashMap);

/*********HASH MAP FUNCTIONS**************/ 
/*Creates the Int To Word Structure*/
struct wordDataArray* Allocate_IntToWordStruct(); 

/*Creates the Word 2 Int Structure*/
struct DummyHeadNode*** Allocate_WordToInt();

/*Takes the two hash maps & initializes them*/
void Initialize_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, char* path);

/*Takes the two hash maps & frees them*/
void Free_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap);

/*Converts a word to an integer*/
int Convert_WordToInt(char* word, struct DummyHeadNode*** WordToInt_HashMap);

/*Converts an integer to a word*/
char* Convert_IntToWord(int wordID, struct wordDataArray* IntToWord_HashMap);




/************WORD SET FUNCTIONS************/
/*Create the word set*/
struct WordSet* init_WordSet(int totalWords);

/*Mark all words to unused in the word set*/
void reset_WordSet(struct WordSet* wordSet);

/*Check if word is used*/
long unsigned int checkIfUsed_WordSet(int wordID, struct WordSet *wordSet);

/*Marks a word as used*/
void markUsed_WordSet(int wordID, struct WordSet *wordSet);

/*Free the word set*/
void free_WordSet(struct WordSet* wordSet);


/********FLWG ALGORITHMS*****************/

/*This allows the algorithm to state a word*/
int botPly(int word, int depth, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet); 



