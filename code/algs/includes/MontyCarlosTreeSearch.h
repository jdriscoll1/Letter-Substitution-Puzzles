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
int montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, int isMaximizer, struct wordDataArray* IntToWord_HashMap);

struct mctsStruct* traverse(struct mctsStruct *node, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

void visit_mctsStruct(int wordID, struct mctsStruct* parent, int isParentMaximizer, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap);

void print_mctsStruct(struct mctsStruct* m);
//monty carlos tree search 
//this takes the current word & outputs the best word 

//traverse -- this takes a node & traverses it, giving visited nodes scores & returning an unvisited node 

//

#endif
