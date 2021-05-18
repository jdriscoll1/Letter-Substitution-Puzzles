#ifndef seenMinimaxTests
#define seenMinimaxTests

#include "Minimax.h"
#include "HashMaps.h"

//Minimax but without when options aren't 0%
struct minimaxOutput* minimax_ZeroOptions(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 
struct minimaxOutput* minimaxAlg_ZeroOptions(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap); 
int compareOutput_ZeroOptions(struct minimaxOutput* curr, struct minimaxOutput* potential, int isMaximizingPlayer); 

//Minimax Without The Beta Variable
struct minimaxOutput* minimax_NoBeta(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 
struct minimaxOutput* minimaxAlg_NoBeta(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap); 
int compareOutput_NoBeta(struct minimaxOutput* curr, struct minimaxOutput* potential, int isMaximizingPlayer); 

//Minimax But Only 1 Method
struct minimaxOutput* minimax_Unmethodized(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 

#endif
