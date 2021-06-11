/*
Title: FLWGGame.h
Descirption: Contains the necessary structures and methods for the Four Letter Word Game. */
#ifndef seenFLWG
#define seenFLWG

#include "../../algs/includes/Minimax.h"

#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/TreeSet.h"




int FLWG(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 

/*Takes user input*/
int Input_FLWG(int prevWord, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap);

/*This is the method that allows the bot to take a turn
@param word --> The current word
@param depth --> how deep will the bot search before saying non-leaf nodes are just 0
@param IntToWord_HashMap --> This allows the bot to know which nodes connect to which
@return --> Returns the word that the bot found
@case --> -1 = the bot has lost*/
int botPly(int word, int depth, struct wordDataArray* IntToWord_HashMap, struct minimaxOutput* (*minimax_func)(int, int, int, int, struct minimaxOutput, struct minimaxOutput, struct wordDataArray*) ); 

int weakBotPly(int word, struct wordDataArray* IntToWord_HashMap); 

/*This is the method that allows the user to take a turn
@param word --> This is the word that the bot will use to find its next victim
@param WordToInt_HashMap --> This is the hash map that converts a word to an integer
@param IntToWord_HashMap --> This is the hash map that takes an int and converts it to an integer
@return --> Returns a chosen word
@case --> -1 = the user gave up :(*/
int userPly(int word, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 
 
void FLWG_Test(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 



#endif
