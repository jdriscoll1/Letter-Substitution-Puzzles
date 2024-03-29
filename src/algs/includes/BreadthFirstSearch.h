#ifndef seenBFS
#define seenBFS

#include "TreeStorageNode.h"

#include "../../structs/includes/ArrayList.h"
#include "../../structs/includes/IntLinkedList.h"
#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/WordSet.h"


/*PATHFINDING METHODS*/

//This structures that contains all of the components of the BFS
struct BFSComponents {			
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
struct intList* BreadthFirstSearch_Goal(int start, int goal, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet);

/*This is the Breadth First Search Who Searches Given a Distance
@param start --> This is where the search begins
@param minConnections --> This is the absolute minimum number of connections that must be betwen the start, and the found word
@param HashMap --> This is the HashMap that contains all the words and their connections
@param storageType --> This is the word that is stored
@return --> Returns an array which is the connections (Advantage, easy access of the furthest connection*/
int* BreadthFirstSearch_Distance(int start, int minConnections, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet); 


/*This is the breadth first search, the only difference is that it returns a single word as opposed to the whole array*/
int BreadthFirstSearch_Distance_Goal(int start, int minConnections, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet); 

/*This outputs the number of options that a Breadth First Search has from a certain distance away
@param start --> The wrod to be started on
@param minConnections --> How far away should the number of connections be
@param HashMap --> The ability to find the connections
@param storageType --> The type of storage, HashSet? TreeSet?
@return --> Returns the number of connections there are a certain distance away*/
int BreadthFirstSearch_DistanceOptions(int start, int minConnections, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet); 

struct intList* BFS(int start, int goal, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet); 


/*This initalizes all of the necessary components of the Breadth First Search Method
@param start --> This is the start word
@param storageType --> This determines whether it uses an AVL Tree or a HashSet
@return --> Returns a structure of initialized components*/
struct BFSComponents* init_BFSComponents(int start, struct WordSet *wordSet); 

/*This frees the necessary components of the Breadth First Search Components
@param bc --> These are the components
@param storageType --> Is it AVL Tree? HashMap? Etc?*/
void Free_BFSComponents(struct BFSComponents* bc, struct WordSet *wordSet); 

/*This finds all of the connections a word has and puts them on the back of a tree storage node
@param bc --> The components of a breadth first search
@param goal --> Which word is being searched for
@param storageType --> Tree Set? Hash Set? 
@param HashMap --> How we actually find what we are searching for*/
struct TreeStorageNode* AddToTreeStorage_Dist_BFS(struct BFSComponents *bc, int goal, struct wordDataArray* IntToWord_HashMap, struct WordSet* wordSet); 


/*This Copies all the words on a linked list to a tree storage node so that all it's previous connections can be tracked 
@param bc --> All of the breadth first search components
@param minConnections --> How far it goes till it returns NULL 
@param storageType --> Are we using a Hash Map? An AVL Tree? 
@param options --> All of the options that will be considered when finding the output 
@param HashMap --> This is what documents all of the words and their connections
@return --> Returns the current last node in the tree storage node linked list */
struct TreeStorageNode* AddToTreeStorage_BFS(struct BFSComponents *bc, int minConnections, struct arrayList *options, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet); 


#endif
