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

/*Using Breadth First Search, it generates a word node linked list that states all of the words, front to back that align to create the quickes link between the words through singular letter substitution
@param start --> The word it begins with
@param goal --> The word it ends with 
@param HashMap --> This stores all of the words, and the words they connect to 
@see HashMap.h for information about the hash map
@param enum storageType --> this determines whether a found word is stored, if it is done via a tree set or hash set
@return --> Returns a word linked list that has all of the words, starting from the start, and ends at the goal */
void* BreadthFirstSearch(char* start, char* goal, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType); 



/*Takes a word, outputs all the connecting words that haven't already been found
@param wordInput --> the word that's used to find the connecting word, the anchor
@param header --> This is a 2D Linked list that stores all of the words which shares the first letter and first vowel.
cont) Each word contains a list of words that connect to it through single letter substitution
@param FoundStorage --> The storage that contains the words that have already been checked by the program. Tree Sets have been proven to be slower than Hash Sets, however, you can input either one
@param storageType --> either a TREE_SET, HASH_SET, or NEITHER_SET 
@param readOnly --> do we want to add to the foundStorage, or do we want to edit it? Generally, breadth first search == edit, depth first search == probably read only
@return word a linked list of words that connect to the anchor word, wordInput*/
struct word *linkOutput(char* wordInput, struct wordConnections *header, void* FoundStorage, enum FoundWordStorage storageType, int readOnly);

/*For Multiple Pathfinds, it will be necessary to remove the words that have already been found*/ 
struct word *RemoveFoundWord(); 

/*The purpose of this is to add a list of words that connect the start to the goal to a Generic Linked List Node
@param Connections --> the generic linked list that will be accepting the new list
@param timesFound --> helps detect where the list is stored in the Generic Linked List
@param End --> This is the list that stores the connections to the word*/ 
struct GenericLinkedListNode *AddConnection(struct GenericLinkedListNode *Connections, int timesFound, struct TreeStorageNode *End); 

/*This is essentially the lobby for the depth first search. It's the room before it goes into the hallway and things get going.
It just sets allocates some memory locations, sets some variables, and frees everything after the main search happened.
@param start --> the starting word
@param goal --> the goal to be searched for
@param HashMap --> The HashMap that stores all of the words
@return --> Returns a linked list that stores the connections*/ 
struct word* DepthFirstSearch_Lobby(char* start, char* goal, struct wordConnections **(*HashMap));

/*This goes through and finds a path downwards, rather than outwards*/ 
struct TreeStorageNode* DepthFirstSearch(char* start, char* goal, struct wordConnections **(*HashMap), struct word **(*HashSet), struct TreeStorageNode *treeStorage); 

/*The goal of this is to take a start word, and give it a number of connections, and a random word that happens to connect from that far
@Example --> if I say 2, it'll say pies->ties->tins 8) pies->ties->tins->bins->bans->bags->rags->rage->mage
@param minConnections --> the minimum number of connections
@param start --> The word that is started with 
@param HashMap --> The Hash Map that stores all of the words & there connections
@return --> Returns a 2D array, the array[numLetters + 1] = start word. array[numLetters] = end word*/
char** FindByDepth_Lobby(int minConnections, char** allWords, struct wordConnections **(*HashMap)); 

int FindByDepth(int index, int maxIndex, char* curWord, char** connections, struct DummyHeadNode *header, struct wordConnections **(*HashMap)); 


 
 
/*Similar to Pathfind, this time it just tries to find every single path
@params start, goal, HashMap --> Same as in Pathfind
@param max --> the maximum number of times that the word can be found. (Otherwise, it could run indefinitely)
@return --> a 2 dimensional linked list that stores all of the paths*/
struct GenericLinkedListNode *Multiple_Pathfinds(char* start, char* goal, int maxTimesFound, struct wordConnections **(*HashMap)); 

struct GenericLinkedListNode *Multiple_Pathfinds2(char* start, char* goal, int maxTimesFound, struct wordConnections **(*HashMap)); 
struct word* MultiplePathFinds_Final(char* start, char* goal, struct wordConnections **(*HashMap)); 

#endif
