/*
Name: MontryCarlosTreeSearch.c
Date: 10/29/21
Author: Jordan Driscoll
Description: Applies MCTS to the FLWG

*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../includes/MontyCarlosTreeSearch.h"
#include "../../structs/includes/IntLinkedList.h"


//monty carlos tree search
//this takes the current word & outputs the best word 
int montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, int isMaximizer, struct wordDataArray* IntToWord_HashMap){
	time_t initTime = time(0);
	time_t deltaTime = 2; 
	time_t endTime = initTime + 2;
	
	struct mctsStruct* root = malloc(sizeof(struct mctsStruct)); 
	root->isMaximizer = 1; 
	root->numChildren = 0; 
	root->parent = NULL;
	root->score = 0; 
	root->visits = 0; 
	root->wordID = wordID; 
	root->children = NULL;
	
	visit_mctsStruct(wordID, root, 1, wordSet, IntToWord_HashMap); 
	print_mctsStruct(root);
	
	//struct intList* wordOptions = IntToWord_HashMap->array[wordID]->connectionHeader->next;
	struct mctsStruct* bestChild = NULL;     
	//while there is still computational power left 
	while(time(0) < endTime){
		printf("Happy Time: %d", time(0));
		//calls the traversal function on the root and sets it to a variable called leaf
	
		//finds unvisited child  
		struct mctsStruct* unvisitedChild = traverse(root, wordSet, IntToWord_HashMap);
		 
		//take the unvisited child & simulates it, calling the rollout function 
		/*********** struct mctsStruct* simulationResult = rollout(unvisitedChild); ***********/
		 
		//takes the results found from the simulation and backpropogates it
		/****** backpropogate(unvisitedChild, simulationResult); ************/
		
	}
	//Returns the best child
	return -1; 
}

//traverse -- this takes a node & traverses it, giving visited nodes scores & returning unvisited nodes
struct mctsStruct* traverse(struct mctsStruct *node, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	struct mctsStruct *max = NULL;
	//is a node fully explored
	int isFullyExplored = 1; 
	struct mctsStruct *outputNode = NULL; 
	
	//while the current node, that will constantly be changing is fully expanded 
	while(isFullyExplored){
	
		int i; 
		//Loop through all of the children of a node
		for(i = 0; i < node->numChildren && isFullyExplored == 1; i++){
			//if the child it explores is unfound, break out of the while loop 
			if(node->children[i]->visits == 0){
				//this node is not fully explored
				isFullyExplored = 0; 
			
				visit_mctsStruct(node->wordID, node, node->isMaximizer, wordSet, IntToWord_HashMap);
				//sets the output node to the unexplored node
				outputNode = node->children[i];
			}	
			
			//if the child is found
			else{
				//if there is not yet a max, set the max
				if(max == NULL){
					max = node->children[i];
				}
				
				//If there is a max set, check to see if this is greater
				else{
					if(max->score < node->children[i]->score){
						max = node->children[i];
					}
				}
			}	
		}
		//After it has explored all of the children
		//if it has not found unexplored child, then it should set itself to the max child, and set max to null
		//otherwise there's no need. However, this should not be all that complex
		node = max; 
		max = NULL;	
		//if it has reached a leaf node, then it returns that 
		if(node != NULL && node->numChildren == 0){
			outputNode = node; 
			isFullyExplored = 1; 
		}
		
	}
	//more precisely, check through the nodes and if one of them happens to be NULL, then return it 
	//however, it is necessary to keep track of the node with the highest score, and if it reaches null, then
	//return that node 
	
	//set the node to the node w/ the best utc
	//if it has reached a node that is NOT fully expanded, it'll immediately return it & then simulate it  
	//return either an unvistited node, or the current node, assuming it's a leaf 	
	return outputNode;
}	

//rollout -- so this goes through and essentially simulates a node returning however likely it is to win
//this takes a node that has yet to be explored, and simulates the likelihood of winning based on some policy 
	//while the node is not a leaf
		//set the node to a child node based on some policy 
	//return the score of this node

//backpropogate -- This takes results & sends it upwards
	//once it reaches the root, it'll return 
	//otherwise, it'll take this node stats <tbd> & updates them based on the inputted node & the results
	//it will recursively call this function to the nodes parent  

//bestChild -- this chooses node with the higest number of visits


void visit_mctsStruct(int wordID, struct mctsStruct* parent, int isParentMaximizer, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	int numChildren = 0; 
	int isMaximizer = (isParentMaximizer == 1) ? 0 : 1; 
	struct intList* options = IntToWord_HashMap->array[wordID]->connectionHeader; 
	//find the number of available children 
	while(options->next != NULL){
		numChildren++; 
		options = options->next;
		
		
	}
	//if the parent's children have been found
	if(parent->children == NULL){
		//allocate space for the children
		parent->children = calloc(numChildren, sizeof(struct mctsStruct*));
		int i = 0;
		struct intList* options = IntToWord_HashMap->array[wordID]->connectionHeader->next; 
		for(i = 0; i < numChildren; i++){
			parent->children[i] = malloc(sizeof(struct mctsStruct));
			parent->children[i]->children = NULL;
			parent->children[i]->isMaximizer = isMaximizer; 
			parent->children[i]->numChildren = -1;
			parent->children[i]->parent = parent;  
			parent->children[i]->score = -1; 
			parent->children[i]->visits = 0; 
			parent->children[i]->wordID = options->data;
			printf("%d\n\n", options->data); 
			options = options->next; 
		}
	}
	parent->numChildren = numChildren;
	parent->visits++; 
	 	
	
}

void print_mctsStruct(struct mctsStruct* m){
	printf("<%d> {\n", m->wordID);
	printf("\tisMaximizer: %d\n", m->isMaximizer);
	printf("\tvisits: %d\n", m->visits);
	printf("\tnumChildren: %d\n", m->numChildren);
	if(m->numChildren > 0){
		int c; 
		printf("\tchildren: ");
		for(c = 0; c < m->numChildren; c++){
			printf("%d ", m->children[c]->wordID); 
			
		}
		printf("\n");
		
	}
	printf("\tscore: %f\n", m->score);
	if(m->parent != NULL){
		printf("\tparent: %d\n", m->parent->wordID); 	
	}
	else{
		printf("\tparent: NULL\n");
	}
printf("}");
	
	
}




