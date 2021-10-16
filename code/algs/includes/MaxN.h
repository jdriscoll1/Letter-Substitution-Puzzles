/*
Name: MaxN.h
Author: Jordan Driscoll
Date: 8/27/2021
Description: Multiplayer Algorithm Headers for the FLWG
*/

#ifndef seenMaxN
#define seenMaxN

#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/WordSet.h"

struct rawScore{
	//Is this a winning or a losing move?:
		//+1 = winning position
		//-1 = losing position
		//0 = unknown position -- reached max depth
	int isWinningPosition; 
	
	//the likelihood this is a good move 
		//The higher the percentage the more likeky it is to win 
	float isWinPercent; 
	
	//The depth: 
		//If it is a winning move, you want a higher depth
		//If it is a losing move, you want a lower depth
	int depth; 
};

//The score that a node will contain
struct maxnNodeScore{
	
	//Each player's score
	int* scores; 
	
	//a list which each index represents the likelihood the player
	//whose id is that index, will win 
	struct rawScore** rawScores; 
	
	//the word ID associated with this score
	int wordID; 
	
	
};

struct maxnNodeScore* init_MaxNNodeScore(int numPlayers); 

struct maxnNodeScore* MaxN(int wordID, int playerID, int numPlayers, int depth, int maxDepth, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet);

/*Convert an array to Zero-Sum*/
void Int2ZeroSum_Array(int* scores, int numPlayers); 

/*Score a node given the variables*/
struct rawScore* init_RawScore(int isWinningPosition, float isWinPercent, int depth);

void Free_MaxNNodeScore(struct maxnNodeScore *node, int numPlayers); 

/*Compare two outputs, score arrays*/
//a -- the one currently being compared to see if it is worthwhile
//b -- the previous score that has either already one, or was created when the algorithm started at this level
struct maxnNodeScore* compareScores(struct maxnNodeScore* a, struct maxnNodeScore* b, int numPlayers); 

/*Takes a list of scores and converts it to a list of integers*/
int* Raw2Int_Array(struct rawScore** rawScores, int numPlayers);

//Takes a score and converts it to an integer
int Raw2Int(struct rawScore* score);

/*When the maximum depth has been reached and it is unknown if this is a good move*/
struct maxnNodeScore* unknownOutcome(int wordID, int numPlayers); 

//Assigns a losing score to whoever is on this word
//Assigns a winning score to everyone else
struct maxnNodeScore* assignScore(int depth, int wordID, int playerID, int numPlayers); 

void Print_MaxNNodeScore(struct maxnNodeScore* node, int numPlayers); 

void Print_RawScore(struct rawScore* r); 

#endif

