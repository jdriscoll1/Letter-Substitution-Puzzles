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


struct mctsStruct* updateMax(struct mctsStruct* currMax, struct mctsStruct* currChild, int simulations); 

struct mctsStruct* init_mctsStruct(int isMaximizer, struct mctsStruct* parent, int wordID); 

int getOutput(struct mctsStruct* root); 


//monty carlos tree search
//this takes the current word & outputs the best word 
int montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	/************ TIMING THE PROGRAM *******************/ 
	/*The time at which the program begins*/
	//time_t initTime = time(0);
	/*How many seconds hte program is expected to last*/
	//time_t deltaTime = 1; 
	/*At what time should the program end*/
	//time_t endTime = initTime + deltaTime;
	
	
	
	
	
	/*Initialize the root word node*/
	struct mctsStruct* root = init_mctsStruct(1, NULL, wordID);
	
	/*Explore the root node & obtain its children*/
	visit_mctsStruct(wordID, root,  wordSet, IntToWord_HashMap);
	
	/* The Current Simulation*/
	int s = 0; 
	/*The # of times it should run */
	int numRuns = 50000;
	
	//How deep it should branch after it finds an unexplored node
	int depth = 10000; 
	

	/*Run the simulation*/
	while(s < numRuns) {
		//printf("Run: %d\n", s);
		/*1) Find an unexplored node starting at the root*/
		struct mctsStruct* unexploredNode = traverse(root, s, wordSet, IntToWord_HashMap);
		//printf("Unexplored Node: %d\n", unexploredNode->wordID);
		/*2) Go down a whole bunch of nodes until there is a word that has no connections, or it reaches max depth*/
		int simulationResult = rollout(unexploredNode->wordID, depth, 1, wordSet, IntToWord_HashMap); 
		//printf("Simulation Result: %d", simulationResult);
		
		/*3) Send the result up starting at the unexplored node*/
		backpropogate(unexploredNode, simulationResult, wordSet);
		
		/*Move to the next simulation*/
		s++;
		
		

	}
	
	int bestChoice = getOutput(root);
	free_mctsStruct(root);
	return bestChoice;  
	
	

	 
	
	
	
	
	
	


}



struct mctsStruct* updateMax(struct mctsStruct* currMax, struct mctsStruct* currChild, int simulations){
	
	//if there is not yet a max, set the max
	struct mctsStruct* max = (currMax == NULL) ? currChild : currMax;  
	
	//If the max is not equal to the current child 
	if(max != currChild){
	
		//Get the score of the maximum node 
		double maxScore = calculate_mctsScore(max, simulations); 
		//Get the score of the current child 
		double currScore = calculate_mctsScore(currChild, simulations);
		  
		//If the current child's score is better than the max score. Update the max 
		if(currScore > maxScore){	
			max = currChild;

		}
		
	}
	return max; 
}



/*
WHAT DOES TRAVERSE DO?
Traverse starts at the root node & glances at all of the 1-deep children node 
There'll be two cases: 
	1) It finds an unexplored node & has to return it. This node will be rolled out 
	2) It finds that all of the nodes have been explored & has to choose the best one based on some equation 
	
Return: An unexplored node 
What if? All of the nodes are deadends? What if it reaches a point where just all of the nodes are dead ends? */
struct mctsStruct* traverse(struct mctsStruct *node, int simulations, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	
	
	//This is probably the maximum output node 
	struct mctsStruct *max = NULL;
	
	//Has it found a child that has not been explored yet? 
	int unexploredChildFound = 0; 
	
	//TODO: FINISH COMMENT
	//The output node is the node that has not been explored or that has no children left 
	struct mctsStruct *outputNode = NULL; 
	
	//Now the parent node is going to be the node whose children are going to be explored
	struct mctsStruct *parent = node; 
	
	
	
	//Until it finds a child that has not been explored, it will continue going deeper and deeper through children
	while(!unexploredChildFound){
		
		
		
		//The parent has been visited again
		parent->visits++;
				
		
		//If the current node has no children, just return it 
		if(parent->numChildren == 0){
			return parent; 
			 
		}
		
		
		//Loops through all of the children of a node
		//i represents the id of a child node relative to the parent node
		int i; 
		for(i = 0; i < parent->numChildren && unexploredChildFound == 0; i++){
			
			//The current child being considered
			struct mctsStruct* currChild =  parent->children[i]; 

			//printf("Curr Child: %s\n", Convert_IntToWord(currChild->wordID, IntToWord_HashMap));
			
			int childIsExplored = currChild->visits > 0; 
			
			
			
			//if the current child has not been explored yet 
			if(!childIsExplored){
				//printf("Child Not Explored\n");
				//Visits the current child, and fills it out 
				visit_mctsStruct(currChild->wordID, currChild, wordSet, IntToWord_HashMap);
				
				//sets the output node to the unexplored node (the current child)
				outputNode = currChild;
				
				//TODO: Can I just return current child? 
				//this node is not fully explored
				unexploredChildFound = 1; 
				
				markUsed_WordSet(currChild->wordID, wordSet); 
	
			}	
			
			
			
			
			//if the child is found
			else{
				
				max = updateMax(max, currChild, simulations);
			}	
		}	
		
		//After it's gone through & all the children have been explored. It chooses to explore the "best" node to explore
		parent = max; 
		if(!unexploredChildFound){
			markUsed_WordSet(parent->wordID, wordSet);

		}
		//It also sets the current max to null because it doesn't exist anymore
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
void backpropogate(struct mctsStruct* node, int isWin, struct WordSet* wordSet){
	
	//until it is has reached the root it will back propogate
	while(node->parent != NULL){
		
		node->numWins += (isWin == 1) ?  1 : 0; 
		markUnused_WordSet(node->wordID, wordSet); 
		
		node = node->parent;
		isWin = (isWin == 1) ? -1 : 1; 
	}

	
	
}
	//once it reaches the root, it'll return 
	//otherwise, it'll take this node stats <tbd> & updates them based on the inputted node & the results
	//it will recursively call this function to the nodes parent  

//bestChild -- this chooses node with the higest number of visits

//This takes a look at a node & fills out its current data based on where it lies in the tree 
void visit_mctsStruct(int wordID, struct mctsStruct* node, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	
	//The Current Child Is NOT going to change - this only adds its children & sets there values 
	
	
	//The number of children the current node has 
	int numChildren = 0;
	
	 //The 
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
		//Allocate space for the children
		node->children = calloc(numChildren, sizeof(struct mctsStruct*));
		
		//The current child being looked at's ID 
		int i = 0;
		
		//The Options of the children to be searched through 
		options = IntToWord_HashMap->array[wordID]->connectionHeader; 
		
		for(i = 0; i < numChildren; i++){
			do{
				options = options->next;
			}while(options->next != NULL && checkIfUsed_WordSet(options->data, wordSet) != 0);
			
			node->children[i] = init_mctsStruct(isMaximizer, node, options->data); 
			
		}
	}
	node->numChildren = numChildren;

	node->visits = 1; 
	 	
	
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
	//printf("ID: %d, Maximizing: %d\n", id, isMaximizing);
	if(id == -1){
		return (isMaximizing == 1) ? 1: -1;
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
	double c = 5;//sqrt(2);	
	
	//n_i is the number of times the action has previously been selected
	double n = (double)m->visits; 
	
	//x_i = the value of the node 
	double x = m->numWins / n; 
	//t is 
	
	
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

int getOutput(struct mctsStruct* root){
	//Loop through the nodes to determine which has the greatest number of wins 
	
	//Find the # of children the root has
	int n = root->numChildren; 
	
	//The best node to choose based on its id
	int maxNode = -1;
	//The best score aquirable 
	int maxScore = -50000000;  
	
	//Loop through the children & take the one that has the maximum # of wins
	int i;  
	for(i = 0; i < n; i++){
		struct mctsStruct* currChild = root->children[i]; 
		//printf("%d) ID: %d, Visits: %d, Score: %d\n", i, currChild->wordID, currChild->visits, currChild->numWins);
		if(currChild->numWins > maxScore){
			maxScore = currChild->numWins; 
			maxNode = currChild->wordID; 
		}
	}
	
	//Output the ID of the max 
	return maxNode; 	
}
	

struct mctsStruct* init_mctsStruct(int isMaximizer, struct mctsStruct* parent, int wordID){
	/*Initialize the root word node*/
	struct mctsStruct* newNode = malloc(sizeof(struct mctsStruct)); 
	newNode->isMaximizer = isMaximizer; 
	newNode->numChildren = 0;
	newNode->numWins = 0;  
	newNode->parent = parent;
	newNode->score = 0; 
	newNode->visits = 0; 
	newNode->wordID = wordID; 
	newNode->children = NULL;
	return newNode; 
}	




