/*
Name: Hypermax.h
Author: Jordan Driscoll
Date: 9.20.21
Description: Multiplayer FLWG with Node Culling, or Alpha-Reduction*/

#include "MaxN.h"

#include "../../structs/includes/WordSet.h"


int Hypermax(int wordID, int playerID, int numPlayers, int depth, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet);

/*Hypermax is "Max-N with alpha beta pruning"
@param wordID --> The word who is at the top of the tree, the root word
@param playerID --> The player whose perspective is being considered in the evaluation of moves
@param numPlayers --> the number of players in the game
@param depth --> the current depth, higher is shallow, lower is deeper
@param maxDepth --> Also the starting depth
@param alphas --> The variables that allow for node culling
	@note: The alpha beta scores are only the number representation of the scores, the raw score and wordID is unnecessary 
@return --> The variable that was deemed best
*/
struct maxnNodeScore* HypermaxAlg(int wordID, int playerID, int numPlayers, int depth, int maxDepth, int* alphas, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet);
