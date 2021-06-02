#ifndef seenMinimax
#define seenMinimax

#include "HashMaps.h"
/*The purpose of this algorithm is to search depth first down a tree and find the most likely path to trap your opponent
@param wordID --> The ID of the word being checked
@param depth --> How far is it from the original word?
@param isMaximizingPlayer --> Is it the maximizing player (looking for the max move) or the minimzing player (looking for the min move)
@return --> Returns the evaluation of all of the connected nodes, or the current node if it is the leaf*/
//int minimax(int wordID, int depth, int isMaximizingPlayer, struct HashMapNode** HashMap); 
struct minimaxOutput{
	/*This is the score, whether it be negative one, positive one, or 0
	@value 0 --> There are still connections, and it is unable to determine any further to see if those are good or bad
	@value 1 --> There are no more connections, and the opponent will be stuck at this particular node 
	@value -1--> There are more connections, and the algorithm will be stuck at this particular node*/
	int score; 
	//This is the fraction form of the number of wins to losses ratio
	double winPercent;
	//How deep the node is
	int depth; 
	//The word ID that contains this percent chance, this will change each time it goes up the recursion 
	int id; 
};



struct minimaxOutput* minimax(int id, int depth, int maxDepth, int isMaximizingPlayer, struct minimaxOutput alpha, struct minimaxOutput beta, struct wordDataArray* IntToWord_HashMap); 

struct minimaxOutput* minimaxAlg(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct minimaxOutput alpha, struct minimaxOutput beta, struct wordDataArray* IntToWord_HashMap); 

void Print_MinimaxOutput(struct minimaxOutput *mo); 

/*Compares two minimax outputs
@param a --> The first node being compared
@param b --> The second node being compared
@param isMaximizingPlayer --> Determines if it is the maximizing player being looked at, important for depth node
@return --> Returns whichever is better, based on whether it is the maximizing player
@case 1 --> Good for the maximizer
@case 0 --> Good for the minimizer

*/
int compare_mo(struct minimaxOutput* a, struct minimaxOutput* b, int isMaximizingPlayer); 
//Copies the contents of b onto a 
int copy_mo(struct minimaxOutput* a, struct minimaxOutput* b); 
/*This compares between two outputs -- the current one already considered to be the best. Or the potential, perhaps better than the current
@param curr --> The one whose life span has been longer (absEval, usually)
@param potential --> the one whose life span has been shorter, recently found from child nodes
@return:
@case 1 --> Good for the maximizer
@case 0 --> Good for the minimizer*/
int compareOutput(struct minimaxOutput* curr, struct minimaxOutput* potential, int isMaximizingPlayer); 

/*Compares the depth of two nodes, if their primary & secondary score are equal
@return: 
@param primary == 1 --> Good For Maximizer (max depth), Bad For Minimizer (min depth)
@param primary == 0 --> Special Case: Both depths will have to be 0 to zero. So this does not matter. Just choose 1. 
@param primary ==-1 --> Bad For Maximizer (min depth), Good For Minimizer (max depth)
@case 1 --> If the maximizer knows he is going to win (keeps game short). If the minimizer knows he will lose (draws out game).
@case 0 --> If the maximizer knows he is going to lose (draws out game). If the minimizer knows he will win (keeps game short). */
int compareDepth(struct minimaxOutput* curr, struct minimaxOutput* potential, int primary, int isMaximizingPlayer); 

/*This does the alpha-beta pruning
@param alpha --> The score that determines if the maximizer can prune 
@param beta --> The score that determines if the minimizer can prune
@absEval --> The score whose being tested against
@param isMaximizingPlayer --> is it the maximizer or minimizer?
@return --> 1 or 0
@case 1 --> We pruned
@case 0 --> We did not prune*/
int AlphaBetaPruning(struct minimaxOutput *alpha, struct minimaxOutput* beta, struct minimaxOutput* absEval, int isMaximizingPlayer); 

/*Compares for the winPercentage*/
int compareWinPercent(double potential, double curr); 

void DFS(int id, struct wordDataArray* IntToWord_HashMap); 

/*Creates a score box
@param score --> Primary value {-1, 0, +1}
@param winPercent --> Secondary value [0, 1]
@param depth --> How deep is the node the smaller the number the deeper it is {0, 1, ... depth -1, depth}
@param id --> The word whose score is being measured*/
struct minimaxOutput* createOutput(int score, double winPercent, int depth, int id); 

#endif
