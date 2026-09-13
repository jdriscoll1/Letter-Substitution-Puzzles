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

struct mctsStruct* traverse(struct mctsStruct *node, int simulations, int numPlayers, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

void visit_mctsStruct(int wordID, struct mctsStruct* node, int numPlayers, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

//this defines the rollout policy for a node.
//@param playerToMove --> whose turn it is at id
//@return --> the player left without a move, or -1 if the depth ran out first
int rollout(int id, int depth, int playerToMove, int numPlayers, struct WordSet* wordSet, struct wordDataArray *IntToWord_HashMap);

//this backpropogates allowing the results to be returned.
//@param stuckPlayer --> the player the play-out stranded, or -1 for no verdict
void backpropogate(struct mctsStruct* node, int stuckPlayer, struct WordSet* wordSet);

void print_mctsStruct(struct mctsStruct* m);

double calculate_mctsScore(struct mctsStruct* m, int simulations);

//frees a mcts struct and all of its children nodes
void free_mctsStruct(struct mctsStruct *root); 
//monty carlos tree search 
//this takes the current word & outputs the best word 

//traverse -- this takes a node & traverses it, giving visited nodes scores & returning an unvisited node 

//

#endif
