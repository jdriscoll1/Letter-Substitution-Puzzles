#ifndef seenDFS
#define seenDFS

#include "2DLinkedList.h"

/*This is essentially the lobby for the depth first search. It's the room before it goes into the hallway and things get going.
It just sets allocates some memory locations, sets some variables, and frees everything after the main search happened.
@param start --> the starting word
@param goal --> the goal to be searched for
@param HashMap --> The HashMap that stores all of the words
@return --> Returns a linked list that stores the connections*/ 
struct word* DepthFirstSearch_Lobby(char* start, char* goal, struct wordConnections **(*HashMap));

/*This goes through and finds a path downwards, rather than outwards*/ 
struct TreeStorageNode* DepthFirstSearch(char* start, char* goal, struct wordConnections **(*HashMap), struct word **(*HashSet), struct TreeStorageNode *treeStorage); 

#endif
