#ifndef seenPathGameComponents
#define seenPathGameComponents

#include "2DLinkedList.h"
#include "ArrayList.h"
#include "Hints.h"

struct  GameComponents{
	//This is where it starts
	char* start; 
	//This is the shortest path 
	char* goal; 
	//This is the minimum number of connections 
	int minConnections; 
	//This is the amount of hints you can use
	struct HintComponents *hc; 
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
	//Now we need an array list which is a string
	struct arrayList* aList; 
	

};

/*This instantiates the Game Components
@param allWords --> The array that stores all of the words
@param HashMap --> The HashMap that stores all word's connections
@param minConnections --> The minimum number of connections needed to connect
@return --> Returns all of the Initalized Game Components, including the Start & Goal Word*/
struct GameComponents *InitializeGameComponents(char** allWords, struct wordConnections **(*HashMap), int minConnections);

/*This resets the Game Components so if the user decides to redo the round, they can*/
void ResetGameComponents(struct GameComponents *gc);

/*This checks if a word is in the game
@param word --> This is the word that the user wants to see if it's in the Hash Map
@param Hash Map --> This will be checked
@return 1 -- it is in the dictionary 0 -- it is not in dictionary*/
int inDictionary(const char* word, struct wordConnections ***HashMap); 


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

//This copies a word_ll onto an array list
void CopyWordLLOntoArrayList(struct GameComponents *gc); 

#endif
