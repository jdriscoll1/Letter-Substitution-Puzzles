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

struct score_parameters{
	int remainingDepth; 
	int isMaximizingPlayer; 
	int goalId; 
}; 

struct score flwg_score(int id, struct DataStructures* data, struct score_parameters parameters); 
struct score flwc_score(int id, struct DataStructures* data, struct score_parameters parameters); 
int choose_random_word(int id, struct DataStructures* data); 

// Minimax 
struct score minimax2(int id, int goalId, int remainingDepth, int startDepth, int isMaximizingPlayer, struct score alpha, struct score beta, struct DataStructures* data, struct score (*scoreFunction)(int,struct DataStructures*, struct score_parameters)); 

// output a score
void printScore(struct score s);

/*Compares two scores and output the word id of the better score*/
int compareScore(struct score a, struct score b, int isMaximizingPlayer); 


struct score createScore(int wordId, double score, double winPercentage, int depth);
/*This does the alpha-beta pruning
@param alpha --> The score that determines if the maximizer can prune 
@param beta --> The score that determines if the minimizer can prune
@absEval --> The score whose being tested against
@param isMaximizingPlayer --> is it the maximizer or minimizer?
@return --> 1 or 0
@case 1 --> We pruned
@case 0 --> We did not prune*/
int AlphaBetaPrune(struct score *a, struct score* b, struct score x, int isMaximizingPlayer); 



#endif
