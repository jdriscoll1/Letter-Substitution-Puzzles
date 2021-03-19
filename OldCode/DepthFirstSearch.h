#ifndef seenDFS
#define seenDFS

#include "2DLinkedList.h"
#include "BreadthFirstSearch.h"
#include "WordLinkedList.h"

/*This is essentially the lobby for the depth first search. It's the room before it goes into the hallway and things get going.
It just sets allocates some memory locations, sets some variables, and frees everything after the main search happened.
@param start --> the starting word
@param goal --> the goal to be searched for
@param HashMap --> The HashMap that stores all of the words
@return --> Returns a linked list that stores the connections*/ 
struct word* DFS_Lobby(char* start, char* goal, struct wordConnections **(*HashMap));

/*This goes through and finds a path downwards, rather than outwards*/ 
struct TreeStorageNode* DFS(char* start, char* goal, struct wordConnections **(*HashMap), struct word **(*HashSet), struct TreeStorageNode *treeStorage); 

struct word *linkOutput_DFS(char* wordInput, struct wordConnections *header, void* FoundStorage, enum FoundWordStorage storageType, int readOnly); 
#endif
