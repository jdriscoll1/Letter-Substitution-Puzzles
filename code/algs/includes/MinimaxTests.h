#ifndef seenMinimaxTests
#define seenMinimaxTests

#include "Minimax.h"
#include "../../structs/includes/HashMap.h"


//When it reaches depth of 0, it counts the number of connections
struct minimaxOutput* minimax_CountAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 
struct minimaxOutput* minimaxAlg_CountAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap); 


//When it reaches 0, if there is still a connection it will go to the 50 50 option
struct minimaxOutput* minimax_FiftyFifty(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 
struct minimaxOutput* minimaxAlg_FiftyFifty(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap); 


//Minimax But It Stops when it reaches zero
struct minimaxOutput* minimax_QuitAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct wordDataArray* IntToWord_HashMap); 
struct minimaxOutput* minimaxAlg_QuitAtZero(int id, int depth, int maxDepth, int isMaximizingPlayer, struct intList* currConnection, struct wordDataArray* IntToWord_HashMap); 

//The minimax algorithm that chooses the first word it can find
int chooseFirst(int id, struct wordDataArray* IntToWord_HashMap); 
//The minimax algorithm that chooses a random word  
int chooseRandom(int id, struct wordDataArray* IntToWord_HashMap);

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
