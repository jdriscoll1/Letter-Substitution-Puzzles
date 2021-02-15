#ifndef seenPath
#define seenPath

#include "TreeStorageNode.h"
#include "TreeSet.h"
#include "ArrayList.h"

//#include "2DLinkedList.h"
/*PATHFINDING METHODS*/


//The enum that determines whether, when a word is found, if it is stored via a tree set, a hash set, or if it only wants the list of words, and does not want the word to be removed from a set 
enum FoundWordStorage{
	HASH_SET, TREE_SET, NEITHER_SET
};

//This structures that contains all of the components of the BFS
struct BFSComponents {	
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
struct word* BreadthFirstSearch_Dest_WordLL(char* start, char* goal, struct wordConnections **(*HashMap), enum FoundWordStorage storageType);

/*This is the Breadth First Search Who Searches Given a Distance
@param start --> This is where the search begins
@param minConnections --> This is the absolute minimum number of connections that must be betwen the start, and the found word
@param HashMap --> This is the HashMap that contains all the words and their connections
@param storageType --> This is the word that is stored
@return --> Returns an array which is the connections (Advantage, easy access of the furthest connection*/
char** BreadthFirstSearch_Distance(char* start, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType); 

/*This outputs the number of options that a Breadth First Search has from a certain distance away
@param start --> The wrod to be started on
@param minConnections --> How far away should the number of connections be
@param HashMap --> The ability to find the connections
@param storageType --> The type of storage, HashSet? TreeSet?
@return --> Returns the number of connections there are a certain distance away*/
int BreadthFirstSearch_DistanceOptions(char* start, int minConnections, struct wordConnections **(*HashMap), enum FoundWordStorage storageType); 



/*This initalizes all of the necessary components of the Breadth First Search Method
@param start --> This is the start word
@param storageType --> This determines whether it uses an AVL Tree or a HashSet
@return --> Returns a structure of initialized components*/
struct BFSComponents* init_BFSComponents(char* start, enum FoundWordStorage storageType); 

/*This frees the necessary components of the Breadth First Search Components
@param bc --> These are the components
@param storageType --> Is it AVL Tree? HashMap? Etc?*/
void Free_BFSComponents(struct BFSComponents* bc, enum FoundWordStorage storageType); 

/*This finds all of the connections a word has and puts them on the back of a tree storage node
@param bc --> The components of a breadth first search
@param goal --> Which word is being searched for
@param storageType --> Tree Set? Hash Set? 
@param HashMap --> How we actually find what we are searching for*/
struct TreeStorageNode* AddToTreeStorage_Dist_BFS(struct BFSComponents *bc, char* goal, enum FoundWordStorage storageType,  struct wordConnections **(*HashMap)); 


/*This Copies all the words on a linked list to a tree storage node so that all it's previous connections can be tracked 
@param bc --> All of the breadth first search components
@param minConnections --> How far it goes till it returns NULL 
@param storageType --> Are we using a Hash Map? An AVL Tree? 
@param options --> All of the options that will be considered when finding the output 
@param HashMap --> This is what documents all of the words and their connections
@return --> Returns the current last node in the tree storage node linked list */
struct TreeStorageNode* AddToTreeStorage_BFS(struct BFSComponents *bc, int minConnections, enum FoundWordStorage storageType, struct arrayList *options, struct wordConnections **(*HashMap)); 


#endif
