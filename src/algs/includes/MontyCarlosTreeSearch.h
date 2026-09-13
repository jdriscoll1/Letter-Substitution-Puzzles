#ifndef seenMCTS
#define seenMCTS

/*
Name: MontyCarlosTreeSearch.h
Date: October 29, 2021
Author: Jordan Driscoll
Desc: Method Headers for the Monty Carlos Tree Search Function
*/

#include "../../structs/includes/WordSet.h"
#include "../../structs/includes/HashMap.h"

struct t{int w;int c;};

//One search builds a node for every move it considers -- around 600,000 of them
//on docs/3.txt -- and every one is dead the moment the move is chosen. So rather
//than asking malloc for each node and handing each one back, they are carved out
//of a handful of large blocks that are released together. Nothing is ever freed
//on its own, which is why there is no free for a single node
struct mctsBlock{
	struct mctsBlock* next;
	//how much of this block has been carved off already
	unsigned long used;
	unsigned long capacity;
	char* memory;
};

struct mctsPool{
	struct mctsBlock* head;
	//blocks grow as the search goes on, so a long one does not collect thousands
	unsigned long nextCapacity;
};

//Opens a pool for one search to build its tree in
struct mctsPool* create_mctsPool(void);

//Releases every node the pool handed out, in one go
void free_mctsPool(struct mctsPool* pool);

struct mctsStruct{
	//the player whose turn it is at this node. The search calls itself player 0,
	//so a node scores for player (parent->player) -- the one who played into it.
	//With two players this alternates 0, 1, 0, ...; with three it cycles 0, 1, 2
	int player; 
	//the word ID that this node is at 
	int wordID; 
	//the node's utc score
	double score; 
	//the number of times this particular node has been visited 
	int visits;
	int numChildren; 
	//the parent of this particular node 
	struct mctsStruct* parent; 
	
	int numWins; 
	
	struct mctsStruct** children; 
	
	
	
};
//Chooses a move for a two player game -- the whole search with numPlayers = 2
int montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

//The same search for any number of players. The caller is player 0 and it is its
//turn; the rest follow it in turn order. The count matters: the game is won by not
//being the player left without a move, so which player a play-out strands depends
//on how many of them sit between this move and that one
int montyCarlosTreeSearch_Multiplayer(int wordID, int numPlayers, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

struct mctsStruct* traverse(struct mctsStruct *node, int simulations, int numPlayers, struct mctsPool* pool, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

void visit_mctsStruct(int wordID, struct mctsStruct* node, int numPlayers, struct mctsPool* pool, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

//this defines the rollout policy for a node.
//@param playerToMove --> whose turn it is at id
//@return --> the player left without a move, or -1 if the depth ran out first
int rollout(int id, int depth, int playerToMove, int numPlayers, struct WordSet* wordSet, struct wordDataArray *IntToWord_HashMap); 

//this backpropogates allowing the results to be returned.
//@param stuckPlayer --> the player the play-out stranded, or -1 for no verdict
void backpropogate(struct mctsStruct* node, int stuckPlayer, struct WordSet* wordSet); 

void print_mctsStruct(struct mctsStruct* m);

double calculate_mctsScore(struct mctsStruct* m, int simulations);

//Nodes belong to the pool they were carved from -- see free_mctsPool
//monty carlos tree search 
//this takes the current word & outputs the best word 

//traverse -- this takes a node & traverses it, giving visited nodes scores & returning an unvisited node 

//

#endif
