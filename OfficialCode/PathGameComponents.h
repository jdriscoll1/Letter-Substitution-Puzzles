#ifndef seenPathGameComponents
#define seenPathGameComponents

#include "2DLinkedList.h"

struct  GameComponents{
	//This is the shortest path 
	char** shortestPath; 
	//This is the minimum number of connections 
	int minConnections; 
	//This is the user's current connections
	struct word* userConnections; 
	//This is all of the user's connections, including the old ones
	struct GenericLinkedListNode *storage; 
	//This is the top of all of the user's connections, including the old one 
	struct GenericLinkedListNode *storageHeader; 
	//This is the number of times the user has hit "undo"
	int undoCalls; 
	//This it the number of moves the user has made
	int numMoves;
	//This user's previous input 
	char* prevInput; 
};

/*
This instantiates the Game Components
@param allWords --> The array that stores all of the words
@param HashMap --> The HashMap that stores all word's connections
@param minConnections --> The minimum number of connections needed to connect
@return --> Returns all of the Initalized Game Components, including the Start & Goal Word*/
struct GameComponents *InitializeGameComponents(char** allWords, struct wordConnections **(*HashMap), int minConnections);

/*This removes a desired word
@param gc --> All of the initalized game components
@param input --> The word to be removed, with a - at input[0]
@param freeInput --> Do you want to free the word, and have it replaced? Or is it freed outside of the remove word structure?*/
void RemoveWord_Struct(struct GameComponents* gc, char* input, int freeInput);
void Undo_Struct(struct GameComponents* gc);
void Redo_Struct(struct GameComponents* gc);
void ResetUndo_Struct(struct GameComponents* gc); 

/*This checks if the user's input is valid, then adds it to the game
@param gc --> All of the games components
@param newWord --> The new word that the user entered
@param HashMap --> The Hash Map who is checked to see if the word is valid
@return --> Returns a 0 if it failed, and a 1 if it worked TO-DO: More specific fail messages (2 = Too Short?)*/
int AddWord_Struct(struct GameComponents* gc, const char* newWord, struct wordConnections **(*HashMap));

//This goes through the game components and frees them
void FreeGameComponents(struct GameComponents *gameComponents);

#endif
