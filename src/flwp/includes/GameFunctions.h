#ifndef seenGame
#define seenGame

#include "PathGameComponents.h"
#include "PathfinderGame.h"

#include "../../api/includes/FLWG-API.h"

#include "../../structs/includes/GenericLinkedListNode.h"
#include "../../structs/includes/HashMap.h"

enum Difficulty{
	EASY, MEDIUM, HARD
};

/*This takes the difficulty of the game and chooses the minimum number of connections the goal word should be from the start word
@param difficulty --> This is the games difficulty
@return --> Returns the minimum number of connections the goal should be from the start*/ 
int GetMinConnections(enum Difficulty difficulty);

/*This is the output after the game ends
@param gameEndCondition --> The condition after the game ended:
@cont: 0)Error 1)They Won 2)They Gave Up */ 
void AfterGameOutput(int gameEndCondition, struct GameComponents* gc); 

/*This chooses the word that the game starts with, and continues trying until it finds a word that has at least one connection
@param allWordsArray --> The array that stores every single word in it
@return --> Returns a word that will be considered the start word*/ 
int ChooseStart(struct wordDataArray* IntToWord_HashMap, int numAdjacencies);
int ChooseStart_Range(struct wordDataArray* IntToWord_HashMap, int minAdjacencies, int maxAdjacencies);

int getWordWithNumberOfConnections(int minConnections, int maxConnections, struct DataStructures* data);

/*Method that determines when the game will be stopped
@param input --> the current word who may be the end
@param isValid --> Is it valid?
@return --> 1) The goal has been met. 0) The goal has not been met 2) User quit*/ 
int goalCheck(char* input, int goal, int isValid, int isCommand, struct DataStructures* data); 


/*Returns a string that is the difficulty*/ 
char* enumToString(enum Difficulty difficulty); 

/*When someone undoes and redoes, if they add or remove this method will be called
@param storageHeader --> This stores the header of storage, which will not be accessible (without looping through prev) without it
@param storage --> This is the current location of the storage
@param userConnections --> This is the current list of words
@param undoCalls -->The number of times that the user has made undo calls +*/ 
void ResetUndo(struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage, struct intList* userConnections, int* undoCalls); 


/*This is the command that runs when the user types help
@param goal --> This is the goal word so that when the user types help it turns out alright*/
void Help(char* goal);

/*this gets the game score*/
int getScore(struct GameComponents* gc); 

/*This takes 2 numbers and calculates the score*/
int calcScore(int n1, int n2); 

/*This is the printed out at the beginning of the game*/ 
void EnterText(); 

/*This is the game being played
@param minConnections --> The minimum number of connections in the round
@param allWords --> These are all of the words that can be chosen from
@param HashMap --> This is the HashMap of all words
@param gc --> These are all of the game components for this individual round
@param pc --> These are the variables stored throughout the entire game 
@return --> It returns the score
@case -1 = the user quit*/
int round_FLWP(struct GameComponents* gc, struct PathfinderGame *pc, struct DataStructures* data); 

#endif
  
