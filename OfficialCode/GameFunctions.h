#ifndef seenGame
#define seenGame

#include "2DLinkedList.h"
#include "GenericLinkedListNode.h"
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

/*The Actual Pathfinder Game --> The game where you start with one word, and have to find another word
For Example:
Pies --> Tins
Pies -> Ties -> Tins
@param allWords --> All of the words
@param wordStorage --> This stores all words in the order
@param HashMap --> This is the hash map that stores all words and their connections
@return --> Returns 1 if user quits, otherwise returns 0*/
int PathfinderGame(int challenge, char** allWords, char** wordStorage, struct wordConnections **(*HashMap));

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

/*This undoes the previous move, it can be redone
@param undoCalls --> This is the number of times the user has typed undo
@param numMoves --> This is the number of moves
@param storage --> This stores all of the moves the user has made
@param prevInput --> This is the previous valid word*/ 
void Undo(int* undoCalls, int* numMoves, struct GenericLinkedListNode **storage, char** prevInput); 

/*This is the redo function when the user wants to redo a move
@param undoCalls --> The number of times a user has chosen to undo a move
@param numMoves --> The number of moves a user has made
@param storage --> This is all the moves a user has made
@parma prevInput --> This is the users previous input*/
void Redo(int* undoCalls, int* numMoves, struct GenericLinkedListNode **storage, char** prevInput); 

/*When someone undoes and redoes, if they add or remove this method will be called
@param storageHeader --> This stores the header of storage, which will not be accessible (without looping through prev) without it
@param storage --> This is the current location of the storage
@param userConnections --> This is the current list of words
@param undoCalls -->The number of times that the user has made undo calls +*/ 
void ResetUndo(struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage, struct word* userConnections, int* undoCalls); 

/*When the user wants to remove a word, and all the words behind it
@param undoCalls --> The number of times the user has previously undone a move
@param numMoves --> The number of moves in total the user has redone
@param cmd --> The full line the user has chosen
@param userConnections --> The current list the user is working with
@param storage --> All of the moves the uesr has done starting from the current undo
@param storageHeader --> All of the moves the user has done starting from the header*/
void RemoveWord(int* undoCalls, int* numMoves, char** cmd, char **prevInput, struct word *userConnections, struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage); 

/*When someone wants to add a word to the list
@param undoCalls --> The number of time the users typed undo
@param numMoves --> The number of moves the user has taken
@param prevInput --> The previous valid word
@param cmd --> The current word
@param userConnections --> This is the current list of words
@param storage --> This is the current storage, all the saved moves the user has done,  location
@param storageHeader --> This is the storage header
@param HashMap --> This is the HashMap that stores all the words and their single letter connections*/ 
void AddWord(int* undoCalls, int* numMoves, char** prevInput, char** cmd, struct word* userConnections, struct GenericLinkedListNode **storage, struct GenericLinkedListNode *storageHeader, struct wordConnections **(*HashMap));


/*The user puts in input
@param undoCalls --> The number of times the user has undone a move
@param numMoves --> The number of moves the user has taken
@param prevInput --> The user's previous input
@param minConnections --> The minimum number of connections a user is allowed
@param shortestConnection --> This is (one of) the shortest Connections a user can take
@param userConnections --> This is the list of connections the user has taken
@param HashMap --> This stores all the words and all their single letter connections
@param storage --> This is the storage of all of the moves the user has previously taken
@param storageHeader --> This is the header of the storage of all of the moves the user has previously taken
@return --> goal check return value*/ 
int GameInput(int undoCalls, int numMoves, char* prevInput, int minConnections, char** shortestConnection, struct word* userConnections, struct wordConnections **(*HashMap), struct GenericLinkedListNode *storage, struct GenericLinkedListNode *storageHeader);

/*This is the command that runs when the user types help
@param goal --> This is the goal word so that when the user types help it turns out alright*/
void Help(char* goal);

/*This is the printed out at the beginning of the game*/ 
void EnterText(); 

  
#endif
  
