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
#include <math.h>

#include "../includes/MontyCarlosTreeSearch.h"
#include "../includes/MinimaxTests.h"

#include "../../structs/includes/IntLinkedList.h"


//monty carlos tree search
//this takes the current word & outputs the best word 
struct t* montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	/*The time at which the program begins*/
	time_t initTime = time(0);
	/*How many seconds hte program is expected to last*/
	time_t deltaTime = 1; 
	/*At what time should the program end*/
	time_t endTime = initTime + deltaTime;
	
	
	/*Initialize the root word node*/
	struct mctsStruct* root = malloc(sizeof(struct mctsStruct)); 
	root->isMaximizer = 1; 
	root->numChildren = 0; 
	root->parent = NULL;
	root->score = 0; 
	root->visits = 0; 
	root->wordID = wordID; 
	root->children = NULL;
	
	/*Explore the root node & obtain its children*/
	visit_mctsStruct(wordID, root,  wordSet, IntToWord_HashMap);
	int s = 0; 

	/*Run precisely 10,000 times*/
	while(s < 50000){
		
		/*Traverse the root & find an unexplored node*/
		struct mctsStruct* m = traverse(root, s, wordSet, IntToWord_HashMap);
		
		/*Simulate the result of that node*/
		int simulationResult = rollout(m->wordID, 800, 1, wordSet, IntToWord_HashMap); 
		
		/*Back propogate those results*/
		backpropogate(m, simulationResult);
		
		/*Move to the next simulation*/
		s++;

	}
	
	/*The node that gets outputted. Defaults to the first child*/
	struct mctsStruct *output = root->children[0];
	
	/*The index that goes through each child*/
	int i = 0; 
	/*TEMPORARY OUTPUT FOR TESTING*/
	int o = 0; 
	
	/*Looks at Each Child of the root to determine he with the best score*/
	for(i = 0; i < root->numChildren; i++){
		/*Looks at another child*/
		struct mctsStruct* c = root->children[i]; 
		
		/*If the node to be outputted has a score that is less than the new node being examined*/
		if(output->score < c->score){
			/*Change the output*/
			output = c;
			/*CHANGE THE TEMPORARY OUTPUT*/
				o = i; 
		}
	}
	/*If there were no options in the first place, let the user know that*/
	if(output == NULL){
		//return -1;  
	}
	
	/*Free the mcts structure data*/
	//free_mctsStruct(root);
	
	

	#ifndef STILL_TESTING
	struct t* owtput = malloc(sizeof(struct t));
	owtput->c = root->children[o]->score; 
	owtput->w = o; 
	return owtput; 
	#endif
	//Returns the best child
	//return output->wordID; 

}

//traverse -- this takes a node & traverses it, giving visited nodes scores & returning unvisited nodes
struct mctsStruct* traverse(struct mctsStruct *node, int simulations, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	struct mctsStruct *max = NULL;
	//is a node fully explored
	int isFullyExplored = 1; 
	struct mctsStruct *outputNode = NULL; 
	
	//while the current node, that will constantly be changing is fully expanded 
	while(isFullyExplored){
		int i;
		//Loop through all of the children of a node
		//it must break if it sees a child that is not fully explored
		for(i = 0; i < node->numChildren && isFullyExplored == 1; i++){
			//if the child it explores is unfound, break out of the while loop 
			if(node->children[i]->visits == 0){
				//this node is not fully explored
				isFullyExplored = 0; 
			
				visit_mctsStruct(node->children[i]->wordID, node->children[i], wordSet, IntToWord_HashMap);
				
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
					if(calculate_mctsScore(max, simulations) < calculate_mctsScore(node->children[i], simulations)){
						
						max = node->children[i];
					

					}
				}
				
			}	
		}
		//After it has explored all of the children
		//if it has not found unexplored child, then it should set itself to the max child, and set max to null
		//otherwise there's no need. However, this should not be all that complex
		//Turn node into child node
		
		node->visits++;
		if(node->numChildren == 0){
			outputNode = node; 
			isFullyExplored = 0;
			//If it has reached a leaf, it returns this node
			
			 
		}
		
		node = max; 
		max = NULL;
		
		
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
void backpropogate(struct mctsStruct* node, int isWin){
	//until it is has reached the root it will back propogate
	while(node->parent != NULL){
		node->score += isWin; 
		node = node->parent; 
	}
	
	
}
	//once it reaches the root, it'll return 
	//otherwise, it'll take this node stats <tbd> & updates them based on the inputted node & the results
	//it will recursively call this function to the nodes parent  

//bestChild -- this chooses node with the higest number of visits


void visit_mctsStruct(int wordID, struct mctsStruct* node, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){

	int numChildren = 0; 
	int isMaximizer = (node->isMaximizer == 1) ? 0 : 1; 
	struct intList* options = IntToWord_HashMap->array[wordID]->connectionHeader; 
	//find the number of available children 
	while(options->next != NULL){
		
		if(checkIfUsed_WordSet(options->next->data, wordSet) == 0){
			numChildren++; 
		}
		
		options = options->next;
		
	}
	//if the parent's children have been found
	if(node->children == NULL){
		//allocate space for the children
		node->children = calloc(numChildren, sizeof(struct mctsStruct*));
		int i = 0;
		options = IntToWord_HashMap->array[wordID]->connectionHeader; 
		for(i = 0; i < numChildren; i++){
			do{
				options = options->next;
			}while(options->next != NULL && checkIfUsed_WordSet(options->data, wordSet) != 0);
			
			
			node->children[i] = malloc(sizeof(struct mctsStruct));
			node->children[i]->children = NULL;
			node->children[i]->isMaximizer = isMaximizer; 
			node->children[i]->numChildren = -1;
			node->children[i]->parent = node; 
			node->children[i]->score = 0;
			node->children[i]->visits = 0;
			node->children[i]->wordID = options->data;
			
		}
	}
	node->numChildren = numChildren;

	node->visits= 1; 
	 	
	
}

//This is the simulation stage
//It takes a node and explores it using some policy
//This policy could be a lot of things, however,
//to keep it simple, it will be randomly assigned
int rollout(int id, int depth, int isMaximizing, struct WordSet* wordSet, struct wordDataArray *IntToWord_HashMap){
	//While it is not a leaf
	 
	if(depth == 0){
		return 0;
		
	}

	id = chooseRandom(id, IntToWord_HashMap, wordSet);
	if(id == -1){
		return (isMaximizing == 1) ? -1: 1;
	}

	
		
	markUsed_WordSet(id, wordSet);
	//Once it reaches the original node, that will mean it tried every option, and did not have a choice
	int isWin = rollout(id, depth - 1, (isMaximizing == 1) ? 0 : 1, wordSet, IntToWord_HashMap); 
	markUnused_WordSet(id, wordSet);
	return isWin; 
}

double calculate_mctsScore(struct mctsStruct* m, int simulations){
	/*S_i = x_i + c sqrt(ln(t) / n_i)*/
	//c -> Constant that grows with the desire to explore.
	double c = sqrt(2);	
	//x_i = the value of the node 
	double x = m->score / (double)m->visits; 
	//t is 
	//n_i is the number of times the action has previously been selected
	double n = m->visits; 
	
	double t = (double)simulations; 
	
	//log = ln in C 
	return x + c * (log(t) / n);
	



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
	printf("}\n\n");
	
	
}

void free_mctsStruct(struct mctsStruct *root){
	/*It has to first free all of it's children recursively, then free itself*/
	int c = 0; 
	for(c = 0; c < root->numChildren; c++){
		free_mctsStruct(root->children[c]); 
		
	}
	//frees the array
	free(root->children);
	free(root);
	
	
	
}



