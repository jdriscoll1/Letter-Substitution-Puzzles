/*
Name: MaxN.c
Author: Jordan Driscoll
Date: 8/25/21 
Description: Creating the Max-N: Multiplayer Four Letter Word Game Without Alpha Beta Pruning
*/

#include <stdlib.h>
#include <stdio.h>

#include "../includes/MaxN.h"
#include "../../structs/includes/IntLinkedList.h"


#define bool int
#define true 1
#define false 0

//input  - word ID, player ID 
//output - a vector of scores 

//Beginning the function, it takes:
//the current node it is on - the word it is on (wordID)
//the current player whose turn it is (playerID)
struct maxnNodeScore* MaxN(int wordID, int playerID, int numPlayers, int depth, int maxDepth, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){

	
	/**********INITIALIZE IMPORTANT VARIABLES*************************/	

	// information about the current word
	struct wordData* wordInfo = IntToWord_HashMap->array[wordID];
	
	//Set that the current word has been found 
	//only if the depth is not zero
	if(depth != 0){
		markUsed_WordSet(wordID, wordSet);
	}
	//The current child that is an option for the algorithm
	struct intList* currChild = wordInfo->connectionHeader->next; 
	//This is going to store the beta scores
	//Before the method returns, it will replace the current beta scores
	float* betaScores = NULL;
	
	int p = 0;
	
	// if the depth is not 0, malloc it
	if(depth != 0){
		betaScores = malloc(sizeof(float) * numPlayers); 
		for(p = 0; p < numPlayers; p++){ betaScores[p] = 0; }
	}
	//Initializes the player scores
 
	
	
	//the number of possible child options
	int numChildren = 0; 
	
	//Initialize the min possible score
	struct maxnNodeScore* bestScore;
	
	// if the depth is zero, there is no need to allocate the best score
	if(depth != 0){
		bestScore = init_MaxNNodeScore(numPlayers); 
		
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*******************STARTS TO LOOP THROUGH CHILD NODES*******************/
	//it loops through each individual child of the current node
	while(currChild != NULL){
		//it makes sure that this particular word has not been used
		if(checkIfUsed_WordSet(currChild->data, wordSet) == 0){
			 //if it is a leaf node with ? children
			 //Return an unknown outcome
			 if(depth == 0){
			 	
			 	//Case I: The depth was found
			 	return unknownOutcome(wordID, numPlayers);
			 }
			 
			//The number of found children increases
	 		numChildren++; 
	 		//printf("Going Down A Level\n");
			//for each child it runs this same function setting the currScore 
			struct maxnNodeScore* childScore = MaxN(
				currChild->data, //child word id
				(playerID + 1) % numPlayers, //playerID
				numPlayers, //number of players
				depth - 1, //curr depth
				maxDepth, //maximum depth
				IntToWord_HashMap, //Hash Map for Information about child node
				wordSet //The words that have already been used
			); 

		
		
			//Update all of the beta values
			for(p = 0; p < numPlayers; p++){
				betaScores[p] += childScore->rawScores[p]->isWinPercent;
				
			}
			
			//It checks if it is greater than the maxScore
			if(childScore->scores[playerID] > bestScore->scores[playerID]){
				Free_MaxNNodeScore(bestScore, numPlayers); 
				bestScore = childScore; 
				//printf("Chose Child Score\n");
				
			}
			//If the best score is better than the child score, it will free the child score
			else{
				Free_MaxNNodeScore(childScore, numPlayers); 
				//printf("Chose Current Score\n");
			}
			
			
			//if it is, it sets the maxScore to the current score
			//it sets the maxProjected score to the current project score, or the ouputted score
		}
		currChild = currChild->next;
	}
	
	
	
	
	
	
	
	
	
	
	
	/*****IT HAS FINISHED LOOKING THROUGH CHILDREN*****/
	
	// after it finishes looping, if it has not seen any children, 
	// it will assign a very bad score for the current player, 
	// and a very high score to the rest of the players 
	if(numChildren == 0){
		
		if(depth != 0){
			//free the beta scores array
			free(betaScores);
			Free_MaxNNodeScore(bestScore, numPlayers);
		}

		
		//CASE II: The depth is minimum and it could not find any replacements
		//printf("This is a bottom move\n");
		//Print_MaxNNodeScore(assignScore(depth, wordID, playerID, numPlayers), numPlayers);
		if(depth != maxDepth){
			markUnused_WordSet(wordID, wordSet);
		}
		else{
			wordID = -1;
		}
		return assignScore(depth, wordID, playerID, numPlayers);
		//Bad Score: -1, 0%, currDepth
		//Good Score: +1, 100%, currDepth (a higher score is better if it's good, and worse if it is bad)
	}
	
	//The score that has been considered the best score
	//go through and replace the beta scores
	for(p = 0; p < numPlayers; p++){
		
		bestScore->rawScores[p]->isWinPercent = betaScores[p] / numChildren;
		
	}

	free(bestScore->scores);
	
	bestScore->scores = Raw2Int_Array(bestScore->rawScores, numPlayers);
	//project the score to zero-sum
	Int2ZeroSum_Array(bestScore->scores, numPlayers);
		
	//Finally update the word
	//if depth is the max depth:
		//the word will be the word you are on, regardless
	if(depth != maxDepth){
		bestScore->wordID = wordID;
		markUnused_WordSet(wordID, wordSet);
	}
	free(betaScores);
	
	//CASE III: The depth is not the minimum and it looked through all the children 

	//when it finishes looping through each child, it returns the max projected score 
	return bestScore;  
}

struct maxnNodeScore* unknownOutcome(int wordID, int numPlayers){
	struct maxnNodeScore* score = malloc(sizeof(struct maxnNodeScore));
	score->wordID = wordID;
	
	score->rawScores = malloc(numPlayers * sizeof(struct rawScore));
	score->scores = malloc(numPlayers * sizeof(int));
	
	int p;
	
	for(p = 0; p < numPlayers; p++){
		score->rawScores[p] = init_RawScore(0, .5, 0);
		score->scores[p] = Raw2Int(score->rawScores[p]);
	}
	Int2ZeroSum_Array(score->scores, numPlayers);
	return score;
}

struct maxnNodeScore* assignScore(int depth, int wordID, int playerID, int numPlayers){
	
	struct maxnNodeScore* score = malloc(sizeof(struct maxnNodeScore));
	score->wordID = wordID; 
	
	score->rawScores = malloc(numPlayers * sizeof(struct rawScore)); 
	score->scores = malloc(numPlayers * sizeof(int));
	
	int p;
	
	for(p = 0; p < numPlayers; p++){		
		//If it is not the current player's move - they've won
		if(p != playerID){
			score->rawScores[p] = init_RawScore(1, 1, depth);
			
		}	
		//If it is the current player's move - they lost
		else{
			score->rawScores[p] = init_RawScore(-1, 0, depth); 
		}
		score->scores[p] = Raw2Int(score->rawScores[p]);
		
	}
	Int2ZeroSum_Array(score->scores, numPlayers);
	return score; 
	
}


struct maxnNodeScore* init_MaxNNodeScore(int numPlayers){
	//The minimum score it could be
	struct maxnNodeScore* min = malloc(sizeof(struct maxnNodeScore)); 
	
	min->rawScores = NULL;
	
	min->scores = malloc(sizeof(int) * numPlayers); 
	
	int p; 
	for(p = 0; p < numPlayers; p++){
		//Set it to the absolute minimum, or -2,000,000,000
		min->scores[p] = -2000000000; 
	}
	
	min->wordID = -1;
	return min; 
	
	
}
//initial call, start node, player 


//Score Function: a * 10,000 + b * 1,000 + c

//Alpha will Probably be initialized at -100,000 since that score is impossible to ever be reached. 

//Question: When will the beta variable be placed. 

void Int2ZeroSum_Array(int* scores, int numPlayers){
	//Obtain the mean
	long double mean = 0; 
	int p; 
	for(p = 0; p < numPlayers; p++){ mean += scores[p]; }
	
	long double tempMean = mean;
	mean /= numPlayers;
	
	//Second, subtract all of the scores by the mean
	for(p = 0; p < numPlayers; p++){scores[p] -= mean;} 
	
}

//Converts a raw score array to integers
int* Raw2Int_Array(struct rawScore** rawScores, int numPlayers){
	int* intScores = malloc(sizeof(int) * numPlayers); 
	int playerIndex = 0; 
	for(playerIndex = 0; playerIndex < numPlayers; playerIndex++){
		//Goes through 
		intScores[playerIndex] = Raw2Int(rawScores[playerIndex]);
		
	} 
	return intScores; 
}



int Raw2Int(struct rawScore* score){
	//alpha * 1,000,000,000 + beta * 10,000,000 + gamma * 100
	//a = +1, b = 100% c = 10
	// 110,010 or +1 100 10

	int output = score->isWinningPosition * 1000000000 + score->isWinPercent * 100000000; 
	output += score->depth; 
	return output;
}

void Free_MaxNNodeScore(struct maxnNodeScore *node, int numPlayers){
	int p = 0; 

	if(node->rawScores != NULL){

		for(p = 0; p < numPlayers; p++){
			free(node->rawScores[p]);  
		} 
		free(node->rawScores); 	
	}
	free(node->scores); 
	free(node); 
	
	
	
}



void Print_MaxNNodeScore(struct maxnNodeScore* node, int numPlayers){
	int p; 
	printf("MaxN Score %d:\n", node->wordID);
	printf("Raw Scores: \n");
	for(p = 0; p < numPlayers; p++){
		if(node->rawScores == NULL){
			printf("%d: %d\n", p, node->scores[p]);
		}
		else{
			printf("%d: {%d, %f, %d}: %d\n", p, node->rawScores[p]->isWinningPosition, node->rawScores[p]->isWinPercent, node->rawScores[p]->depth, node->scores[p]);
		}
	}	
	
}

struct rawScore* init_RawScore(int isWinningPosition, float isWinPercent, int depth){
	struct rawScore* r = malloc(sizeof(struct rawScore));
	r->isWinningPosition = isWinningPosition; 
	r->isWinPercent = isWinPercent;
	r->depth = depth;
	
}

void Print_RawScore(struct rawScore* r){
	printf("{%d, %f, %d}", r->isWinningPosition, r->isWinPercent, r->depth);
	
}
