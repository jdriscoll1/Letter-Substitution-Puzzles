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
	//The word ID that contains this percent chance, this will change each time it goes up the recursion 
	int id; 
};


struct minimaxOutput* minimax2(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap);
struct minimaxOutput* minimax(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 

struct minimaxOutput* minimaxAlg(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap); 

void DFS(int id, struct wordDataArray* IntToWord_HashMap); 

//This creates a minimax output
struct minimaxOutput* createOutput(int score, int winPercent, int id); 

/*This compares two minimax outputs
@param curr --> The one that is the current best
@param potential --> The new one that may be the best
@return 1: potential > curr
		0: curr >= potential*/
int compareOutput(struct minimaxOutput* curr, struct minimaxOutput* potential); 
