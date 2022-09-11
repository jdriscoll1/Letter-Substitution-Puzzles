%module flwgpy
%{
		/*#include "./code/algs/includes/BreadthFirstSearch.h"
        
		#include "./code/algs/includes/MinimaxTests.h"
		#include "./code/algs/includes/TreeStorageNode.h"
		
		
		
		
		#include "./code/flwp/includes/Hints.h"
		#include "./code/flwp/includes/PathGameComponents.h"
		#include "./code/flwp/includes/PathfinderGame.h"
		#include "./code/flwp/includes/UserInput.h"
		
		#include "./code/structs/includes/ArrayList.h"
		#include "./code/structs/includes/GenericLinkedListNode.h"
		#include "./code/structs/includes/HashFunctions.h"
		#include "./code/structs/includes/IntLinkedList.h"
		#include "./code/structs/includes/TreeSet.h"
		#include "./code/structs/includes/WordLinkedList.h"
		*/
		
		/*Minimax Functions*/
		#include "./code/algs/includes/Minimax.h"
		
		/*Hash Map Functions*/
		#include "./code/structs/includes/HashMap.h"
		
		/*FLWP Functions*/
		#include "./code/flwp/includes/GameFunctions.h"
		
		
		/*Simple C Functions*/
		#include "./code/flwg/includes/PythonWrapper.h"
		/*FLWG Functions*/
		#include "./code/flwg/includes/FLWGGame.h"
		
		
		extern int numLetters; 
%}

/*External Integers*/
extern int numLetters;
/*HashMap.h*/

/*We don't need to call the strutures*/

/*Let's first try to get word to int and int to word conversions correct.. */


/*WORD AND INTEGER CONVERSIONS*/
int Convert_WordToInt(char* word, struct DummyHeadNode*** WordToInt_HashMap); 
char* Convert_IntToWord(int wordID, struct wordDataArray* IntToWord_HashMap); 



/*HASH MAP CREATION/DELEATION*/

/*Int to word Creation*/
struct wordDataArray* Allocate_IntToWordStruct(); 
/*Word to Int Creation*/
struct DummyHeadNode*** Allocate_WordToInt(); 
/*Filling the Hash Maps*/
void Initialize_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, char* path); 
/*Freeing both Hash Maps*/
void Free_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 

/*HASH SET FUNCTIONS*/
void setAlgFound(int wordID, struct wordDataArray* IntToWord_HashMap);
int getAlgFound(int wordID, struct wordDataArray* IntToWord_HashMap); 


/*GameFunctions.h*/
int ChooseStart(struct wordDataArray* IntToWord_HashMap); 

/*PythonWrapper.h*/
void InstantiateRandomizer(); 

/*FLWGGame.h*/
int botPly(int word, int depth, struct wordDataArray* IntToWord_HashMap, struct minimaxOutput* (*minimax_func)(int, int, int, int, struct minimaxOutput, struct minimaxOutput, struct wordDataArray*)); 



/*For the minimax*/
%callback("%s_cb"); 
struct minimaxOutput* minimax(int id, int depth, int maxDepth, int isMaximizingPlayer, struct minimaxOutput alpha, struct minimaxOutput beta, struct wordDataArray* IntToWord_HashMap); 
%nocallback; 

