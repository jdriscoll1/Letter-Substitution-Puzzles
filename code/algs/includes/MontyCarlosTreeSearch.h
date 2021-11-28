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
	//is this node in favor of the algorithm
	//or opposing the algorithm
	int isMaximizer; 
	//the word ID that this node is at 
	int wordID; 
	//the node's utc score
	double score; 
	//the number of times this particular node has been visited 
	int visits;
	int numChildren; 
	//the parent of this particular node 
	struct mctsStruct* parent; 
	
	struct mctsStruct** children; 
	
	
	
};
struct t* montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

struct mctsStruct* traverse(struct mctsStruct *node, int simulations, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

void visit_mctsStruct(int wordID, struct mctsStruct* node, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

//this defines the rollout policy for a node
int rollout(int id, int depth, int isMaximizing, struct WordSet* wordSet, struct wordDataArray *IntToWord_HashMap); 

//this backpropogates allowing the results to be returned 
void backpropogate(struct mctsStruct* node, int isWin); 

void print_mctsStruct(struct mctsStruct* m);

double calculate_mctsScore(struct mctsStruct* m, int simulations);

//frees a mcts struct and all of its children nodes
void free_mctsStruct(struct mctsStruct *root); 
//monty carlos tree search 
//this takes the current word & outputs the best word 

//traverse -- this takes a node & traverses it, giving visited nodes scores & returning an unvisited node 

//

#endif
