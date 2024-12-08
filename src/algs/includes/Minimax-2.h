#ifndef seenMinimax
#define seenMinimax

#include "../../structs/includes/HashMap.h"
#include "../../FLWG-API.h"
#include "../../structs/includes/WordSet.h"

/*The purpose of this algorithm is to search depth first down a tree and find the most likely path to trap your opponent
@param wordID --> The ID of the word being checked
@param depth --> How far is it from the original word?
@param isMaximizingPlayer --> Is it the maximizing player (looking for the max move) or the minimzing player (looking for the min move)
@return --> Returns the evaluation of all of the connected nodes, or the current node if it is the leaf*/
struct score{
	// the id of the word whose score is being considered
	int wordId; 
	// score ranging from 0 to 1 
	double score; 
	// the percentage of possible games that lead to a win 
	double winPercentage; 
	//How deep the node is
	int depth; 
};

// These are considerations the algorithm ought make when choosing a word's score
struct score_parameters{
	// The remaining depth in the game 
	int remainingDepth; 
	// How deep did the node start out
	int startDepth; 
	// The perspective of the bot 
	int isMaximizingPlayer; 
	// The words the algorithm is trying to get to 
	struct WordSet* goalWords; 	
	// The words the algorithm ought avoid 
	struct WordSet* avoidWords; 	
	// The score function to be used (FLWC or FLWG)
	struct score (*scoreFunction)(int,struct DataStructures*, struct score_parameters); 
	// The number of goal words that have been found -- always initialized to 0
	int goalWordsFound; 
}; 

struct score flwg_score(int id, struct DataStructures* data, struct score_parameters parameters); 
struct score flwc_score(int id, struct DataStructures* data, struct score_parameters parameters); 
int choose_random_word(int id, struct DataStructures* data); 

// Minimax 
struct score minimax2(int id, int remainingDepth, int isMaximizingPlayer, struct score_parameters parameters, struct score alpha, struct score beta, struct DataStructures* data); 

// output a score
void printScore(struct score s);

/*Compares two scores and output the word id of the better score*/
int compareScore(struct score a, struct score b, int isMaximizingPlayer); 

// Initializes a score object
struct score createScore(int wordId, double score, double winPercentage, int depth);

// Function for alpha beta pruning optimizations
int AlphaBetaPrune(struct score *a, struct score* b, struct score x, int isMaximizingPlayer); 



#endif
