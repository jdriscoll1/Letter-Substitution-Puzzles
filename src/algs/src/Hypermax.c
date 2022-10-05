/*
Name: Hypermax.h
Author: Jordan Driscoll
Date: 9.20.21
Description: Multiplayer FLWG with Node Culling, or Alpha-Reduction*/

#include <stdio.h>
#include <stdlib.h>

#include "../includes/Hypermax.h"

#include "../../structs/includes/IntLinkedList.h"

int Hypermax(int wordID, int playerID, int numPlayers, int depth, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	int* alphas = malloc(sizeof(int) * numPlayers);
	int i = 0; 
	//Initialize alphas to -infinity equivilent
	for(i = 0; i < numPlayers; i++){
		//-2,000,000,000
		alphas[i] = -2000000000;
	}
	struct maxnNodeScore* bestScore = HypermaxAlg(wordID, playerID, numPlayers, depth, depth, alphas, IntToWord_HashMap, wordSet);
	free(alphas);
	int outputWord = bestScore->wordID;
	Free_MaxNNodeScore(bestScore, numPlayers);
	return outputWord;
	
}

struct maxnNodeScore* HypermaxAlg(int wordID, int playerID, int numPlayers, int depth, int maxDepth, int* alphas, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	
	
	/**********INITIALIZE IMPORTANT VARIABLES************/	

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
	//It is necessary to reallocate alphas because it is not possible to 
	//pass arrays by value
	int* currAlphas = malloc(sizeof(int) * numPlayers);
	for(p = 0; p < numPlayers; p++){currAlphas[p] = alphas[p];}
	
	//Unnecessary to malloc it if depth is not too low
	//the number of possible child options
	int numChildren = 0; 
	
	//Initialize the min possible score
	struct maxnNodeScore* bestScore = NULL;
	
	
	
	
	
	
	
	
	
	
	/*******************STARTS TO LOOP THROUGH CHILD NODES*******************/
	//it loops through each individual child of the current node
	while(currChild != NULL){
		//it makes sure that this particular word has not been used
		if(checkIfUsed_WordSet(currChild->data, wordSet) == 0){
			if(depth == 0){
				
				free(currAlphas);
				//Case I: The depth was found
			 	return unknownOutcome(wordID, numPlayers);
			}
			//gets the child node
			struct maxnNodeScore* childScore = HypermaxAlg(currChild->data, (playerID + 1) % numPlayers, numPlayers, depth - 1, maxDepth, currAlphas, IntToWord_HashMap, wordSet);
			for(p = 0; p < numPlayers; p++){
				betaScores[p] += childScore->rawScores[p]->isWinPercent; 
			}
			
			//if leaf node, this is the best score (by default)
			if(numChildren == 0){
				if(bestScore != NULL){
					free(bestScore);
				}
				bestScore = childScore;
			}
			
			
			else{
				
				if(currAlphas[playerID] < childScore->scores[playerID]){
					currAlphas[playerID] = childScore->scores[playerID];
					Free_MaxNNodeScore(bestScore, numPlayers);
					bestScore = childScore;
				}
				else{
					Free_MaxNNodeScore(childScore, numPlayers);
				}
				
				//obtain the sum
				
			}
			long sum = 0;

			//Get the sum of alphas
			for(p = 0; p < numPlayers; p++){
				
				if(currAlphas[p] == -2000000000){
					sum = -1;
					break;
				}
				sum += currAlphas[p];
				
			}
			//it is either the best it can possibly be, or better
			if(sum >= 0){
				break;
			}
			numChildren++;
			//if it is, it sets the maxScore to the current score
			//it sets the maxProjected score to the current project score, or the ouputted score
		}
		currChild = currChild->next;
	}
	
	
	
	
	
	
	
	
	
	
	
	/*****IT HAS FINISHED LOOKING THROUGH CHILDREN*****/
	free(currAlphas);
	
	
	// after it finishes looping, if it has not seen any children, 
	// it will assign a very bad score for the current player, 
	// and a very high score to the rest of the players 
	if(numChildren == 0){
		//free the beta scores array
		
		if(depth != 0){
			free(betaScores);
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
