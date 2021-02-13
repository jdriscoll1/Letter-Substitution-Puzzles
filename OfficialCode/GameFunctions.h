#ifndef seenGame
#define seenGame

#include "2DLinkedList.h"
#include "GenericLinkedListNode.h"
#include "PathGameComponents.h"
enum Difficulty{
	EASY, MEDIUM, HARD
}Difficulty;
/*Start Game*/
enum Difficulty StartGame(); 



/*This finds the pathway that starts from the start word and goes all the way to the end word
@param allWords --> Every single word that can be chosen
@param HashMap --> A Hashmap that helps determine what words can be chosen
@return --> Returns the word connections*/
char** ChoosePathway(char** allWords, struct wordConnections **(*HashMap)); 

/*This takes the difficulty of the game and chooses the minimum number of connections the goal word should be from the start word
@param difficulty --> This is the games difficulty
@return --> Returns the minimum number of connections the goal should be from the start*/ 
int GetMinConnections(enum Difficulty difficulty);

/*This chooses the word that the game starts with, and continues trying until it finds a word that has at least one connection
@param allWordsArray --> The array that stores every single word in it
@param HashMap --> The Hash Map that has all of the word's connections
@param TEST_TEMP --> TEMPORARY VARIABLE THAT IS ONLY USED FOR TEMPORARY PURPOSES
@return --> Returns a word that will be considered the start word*/ 
char* ChooseStartWord(char** allWordsArray, struct wordConnections **(*HashMap), int TEST_TEMP); 


/*Method that determines when the game will be stopped
@param input --> the current word who may be the end
@param goal --> The goal
@return --> 1) The goal has been met. 0) The goal has not been met 2) User quit*/ 
int goalCheck(char* input, char* goal); 


/*Returns a string that is the difficulty*/ 
char* enumToString(enum Difficulty difficulty); 

/*This finds a path from teh start word to the goal based on the difficulty
@param allWords --> This stores all of the words 
@param HashMap --> The Hash Map used to find connections
@param minConnections --> The number of connections it takes*/ 
char** ChoosePath(char** allWords, struct wordConnections*** HashMap, int minConnections); 

/*This is the output after the game ends
@param gameEndCondition --> The condition after the game ended:
@cont: 0)Error 1)They Won 2)They Gave Up */ 
void AfterGameOutput(int gameEndCondition); 


/*When someone undoes and redoes, if they add or remove this method will be called
@param storageHeader --> This stores the header of storage, which will not be accessible (without looping through prev) without it
@param storage --> This is the current location of the storage
@param userConnections --> This is the current list of words
@param undoCalls -->The number of times that the user has made undo calls +*/ 
void ResetUndo(struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage, struct word* userConnections, int* undoCalls); 


/*This is the command that runs when the user types help
@param goal --> This is the goal word so that when the user types help it turns out alright*/
void Help(char* goal);

void hint1(struct GameComponents *gc); 
void hint2(struct GameComponents *gc, struct wordConnections **(*HashMap)); 

/*This is the printed out at the beginning of the game*/ 
void EnterText(); 

int trueGame(int minConnections, char** allWords, char** wordStorage, struct wordConnections **(*HashMap)); 

/*This Handles the Hints*/
void hints(struct GameComponents *gc, int hintID, int minConnections, char* currWord, char* goalWord); 
  
#endif
  
