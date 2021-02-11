#ifndef seenPath
#define seenPath

#include "TreeStorageNode.h"
#include "TreeSet.h"
//#include "2DLinkedList.h"
/*PATHFINDING METHODS*/


//The enum that determines whether, when a word is found, if it is stored via a tree set, a hash set, or if it only wants the list of words, and does not want the word to be removed from a set 
enum FoundWordStorage{
	HASH_SET, TREE_SET, NEITHER_SET
};

//This structures that contains all of the components of the BFS
struct BFSComponents{	
	//This is the AVL Tree Set
	struct DummyHeadNode *TreeHead;
	
	//This is the Hash Set 
	struct word **(*HashSet);
	
	/*Creates the Tree Storage Node Header -- This makes it so it can track through the connections */
	struct TreeStorageNode *ReverseTreeHeader; 
	//This object keeps track of where the previous connection is. Example, pies pins pier --> pies (Keeps this at O(1) time
	struct TreeStorageNode *prevConnection; 

	/*The last word of the queue*/
	struct TreeStorageNode *End; 

};



/*This is the Breadth First Search Who Searches Given A Goal Word
@param start --> The word at which the search begins
@param goal --> The word at which the search ends
@param HashMap --> The HashMap that contains all the words and their connections
@param storageType --> Are you using the Tree Set or the Hash Set to store found words
@return --> Returns the connections as a linked list*/
struct word* BreadthFirstSearch_Dest(char* start, char* goal, struct wordConnections **(*HashMap), enum FoundWordStorage storageType);

/*This is the Breadth First Search Who Searches Given a Distance
@param start --> This is where the search begins
@param minConnections --> This is the absolute minimum number of connections that must be betwen the start, and the found word
@param HashMap --> This is the HashMap that contains all the words and their connections
@param storageType --> This is the word that is stored
@return --> Returns an array which is the connections (Advantage, easy access of the furthest connection*/
char** BreadthFirstSearch_Distance(char* start, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType); 

/*This initalizes all of the necessary components of the Breadth First Search Method
@param start --> This is the start word
@param storageType --> This determines whether it uses an AVL Tree or a HashSet
@return --> Returns a structure of initialized components*/
struct BFSComponents* init_BFSComponents(char* start, enum FoundWordStorage storageType); 

/*This frees the necessary components of the Breadth First Search Components
@param bc --> These are the components
@param storageType --> Is it AVL Tree? HashMap? Etc?*/
void Free_BFSComponents(struct BFSComponents* bc, enum FoundWordStorage storageType); 


/*Takes a word, outputs all the connecting words that haven't already been found
@param wordInput --> the word that's used to find the connecting word, the anchor
@param header --> This is a 2D Linked list that stores all of the words which shares the first letter and first vowel.
cont) Each word contains a list of words that connect to it through single letter substitution
@param FoundStorage --> The storage that contains the words that have already been checked by the program. Tree Sets have been proven to be slower than Hash Sets, however, you can input either one
@param storageType --> either a TREE_SET, HASH_SET, or NEITHER_SET 
@param readOnly --> do we want to add to the foundStorage, or do we want to edit it? Generally, breadth first search == edit, depth first search == probably read only
@return word a linked list of words that connect to the anchor word, wordInput*/
struct word *linkOutput(char* wordInput, struct wordConnections *header, void* FoundStorage, enum FoundWordStorage storageType, int readOnly);




#endif
