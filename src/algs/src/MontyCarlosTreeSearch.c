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



struct mctsStruct* init_mctsStruct(int player, struct mctsStruct* parent, int wordID);

int getOutput(struct mctsStruct* root);

/*The UCT score given ln(t) already worked out. ln(t) is the same for every node in
a pass, so traverse computes it once instead of calling log() per child compared*/
static double uctScore(struct mctsStruct* m, double logSimulations);


//monty carlos tree search
//this takes the current word & outputs the best word
int montyCarlosTreeSearch(int wordID, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	return montyCarlosTreeSearch_Multiplayer(wordID, 2, wordSet, IntToWord_HashMap);
}

//The same search, told how many players are at the table. Everything it decides
//comes down to who is left without a move at the end of a play-out, and that is a
//question of counting seats: with two players the stranded player is the one an
//even number of plies away, with three it is every third one. Running the two
//player search in a three player game therefore credits most of its play-outs to
//the wrong player, and it ends up playing worse than choosing at random
int montyCarlosTreeSearch_Multiplayer(int wordID, int numPlayers, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	/************ TIMING THE PROGRAM *******************/ 
	/*The time at which the program begins*/
	//time_t initTime = time(0);
	/*How many seconds hte program is expected to last*/
	//time_t deltaTime = 1; 
	/*At what time should the program end*/
	//time_t endTime = initTime + deltaTime;
	
	
	
	
	
	/*Initialize the root word node. The search is player 0 and it is its turn*/
	struct mctsStruct* root = init_mctsStruct(0, NULL, wordID);

	/*Explore the root node & obtain its children*/
	visit_mctsStruct(wordID, root, numPlayers, wordSet, IntToWord_HashMap);
	
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
		struct mctsStruct* unexploredNode = traverse(root, s, numPlayers, wordSet, IntToWord_HashMap);
		//printf("Unexplored Node: %d\n", unexploredNode->wordID);
		/*2) Go down a whole bunch of nodes until there is a word that has no connections, or it reaches max depth*/
		int stuckPlayer = rollout(unexploredNode->wordID, depth, unexploredNode->player, numPlayers, wordSet, IntToWord_HashMap);
		//printf("Stranded Player: %d", stuckPlayer);
		
		/*3) Send the result up starting at the unexplored node*/
		backpropogate(unexploredNode, stuckPlayer, wordSet);
		
		/*Move to the next simulation*/
		s++;
		
		

	}
	
	int bestChoice = getOutput(root);
	free_mctsStruct(root);
	return bestChoice;  
	
	

	 
	
	
	
	
	
	


}





/*
WHAT DOES TRAVERSE DO?
Traverse starts at the root node & glances at all of the 1-deep children node 
There'll be two cases: 
	1) It finds an unexplored node & has to return it. This node will be rolled out 
	2) It finds that all of the nodes have been explored & has to choose the best one based on some equation 
	
Return: An unexplored node 
What if? All of the nodes are deadends? What if it reaches a point where just all of the nodes are dead ends? */
struct mctsStruct* traverse(struct mctsStruct *node, int simulations, int numPlayers, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	
	
	//This is probably the maximum output node, and the score it earned
	struct mctsStruct *max = NULL;
	double maxScore = 0.0;

	//Has it found a child that has not been explored yet? 
	int unexploredChildFound = 0; 
	
	//TODO: FINISH COMMENT
	//The output node is the node that has not been explored or that has no children left 
	struct mctsStruct *outputNode = NULL; 
	
	//Now the parent node is going to be the node whose children are going to be explored
	struct mctsStruct *parent = node;

	//ln(t) is fixed for this whole pass, so it is worked out once here rather than
	//once per child comparison -- this loop is the hottest part of the search
	double logSimulations = (simulations < 1) ? 0.0 : log((double)simulations);

	
	
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
				visit_mctsStruct(currChild->wordID, currChild, numPlayers, wordSet, IntToWord_HashMap);
				
				//sets the output node to the unexplored node (the current child)
				outputNode = currChild;
				
				//TODO: Can I just return current child? 
				//this node is not fully explored
				unexploredChildFound = 1; 
				
				markUsed_WordSet(currChild->wordID, wordSet); 
	
			}	
			
			
			
			
			//if the child is found
			else{
				//The best score is carried along instead of being recomputed for
				//the incumbent on every comparison, halving the square roots
				double currScore = uctScore(currChild, logSimulations);
				if(max == NULL || currScore > maxScore){
					max = currChild;
					maxScore = currScore;
				}
			}
		}	
		
		//After it's gone through & all the children have been explored. It chooses to explore the "best" node to explore
		parent = max; 
		if(!unexploredChildFound){
			markUsed_WordSet(parent->wordID, wordSet);

		}
		//It also sets the current max to null because it doesn't exist anymore
		max = NULL;
		maxScore = 0.0;
		
		
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
void backpropogate(struct mctsStruct* node, int stuckPlayer, struct WordSet* wordSet){

	//until it is has reached the root it will back propogate
	while(node->parent != NULL){

		//A node is a move by whoever was on turn at its parent, and the only way
		//to lose is to be the player left without a move. So every player the
		//play-out did not strand scores, which with three or more players means
		//one play-out can be a win for two nodes on the same path
		if(stuckPlayer != -1 && stuckPlayer != node->parent->player){
			node->numWins++;
		}
		markUnused_WordSet(node->wordID, wordSet);

		node = node->parent;
	}

	
	
}
	//once it reaches the root, it'll return 
	//otherwise, it'll take this node stats <tbd> & updates them based on the inputted node & the results
	//it will recursively call this function to the nodes parent  

//bestChild -- this chooses node with the higest number of visits

//This takes a look at a node & fills out its current data based on where it lies in the tree 
void visit_mctsStruct(int wordID, struct mctsStruct* node, int numPlayers, struct WordSet* wordSet, struct wordDataArray* IntToWord_HashMap){
	
	//The Current Child Is NOT going to change - this only adds its children & sets there values 
	
	
	//The number of children the current node has 
	int numChildren = 0;
	
	 //A child is one ply further on, so the turn passes to the next player
	int childPlayer = (node->player + 1) % numPlayers;
	
	
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

		//One pass down the adjacencies, taking each word that is still available
		for(options = IntToWord_HashMap->array[wordID]->connectionHeader->next; options != NULL; options = options->next){
			if(checkIfUsed_WordSet(options->data, wordSet) == 0){
				node->children[i++] = init_mctsStruct(childPlayer, node, options->data);
			}
		}
	}
	node->numChildren = numChildren;

	node->visits = 1; 
	 	
	
}

//This is the simulation stage
//It takes a node and explores it using some policy
//This policy could be a lot of things, however,
//to keep it simple, it will be randomly assigned
int rollout(int id, int depth, int playerToMove, int numPlayers, struct WordSet* wordSet, struct wordDataArray *IntToWord_HashMap){
	//While it is not a leaf

	//No verdict: the play-out ran out of depth before anyone was stranded
	if(depth == 0){
		return -1;

	}

	id = chooseRandom(id, IntToWord_HashMap, wordSet);
	//printf("ID: %d, To Move: %d\n", id, playerToMove);
	//Whoever is on turn has nowhere left to go, so they are the one who loses
	if(id == -1){
		return playerToMove;
	}
	
	

	
		
	markUsed_WordSet(id, wordSet);

	//Once it reaches the original node, that will mean it tried every option, and did not have a choice
	int stuckPlayer = rollout(id, depth - 1, (playerToMove + 1) % numPlayers, numPlayers, wordSet, IntToWord_HashMap);
	markUnused_WordSet(id, wordSet);

	return stuckPlayer;
}

static double uctScore(struct mctsStruct* m, double logSimulations){
	/*S_i = x_i + c sqrt(ln(t) / n_i)*/
	//c -> Constant that grows with the desire to explore.
	double c = 5;//sqrt(2);	
	
	//n_i is the number of times the action has previously been selected
	double n = (double)m->visits; 
	
	//x_i = the value of the node
	double x = m->numWins / n;

	//ln(t) arrives already worked out: it is the same for every node being compared
	return x + c * sqrt(logSimulations / n);
	



}

double calculate_mctsScore(struct mctsStruct* m, int simulations){
	//log = ln in C. Under one simulation there is nothing for the exploration term
	//to weigh against and ln(0) is -inf, so the term is dropped rather than letting
	//a NaN into the comparison
	double t = (double)simulations;
	return uctScore(m, (t < 1.0) ? 0.0 : log(t));
}

void print_mctsStruct(struct mctsStruct* m){
	printf("<%d> {\n", m->wordID);
	printf("\tplayer: %d\n", m->player);
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
	

struct mctsStruct* init_mctsStruct(int player, struct mctsStruct* parent, int wordID){
	/*Initialize the root word node*/
	struct mctsStruct* newNode = malloc(sizeof(struct mctsStruct));
	newNode->player = player;
	newNode->numChildren = 0;
	newNode->numWins = 0;  
	newNode->parent = parent;
	newNode->score = 0; 
	newNode->visits = 0; 
	newNode->wordID = wordID; 
	newNode->children = NULL;
	return newNode; 
}	




