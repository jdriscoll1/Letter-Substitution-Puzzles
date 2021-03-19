#ifndef HashMap2
#define HashMap2
#include "TreeSet.h"

/*
Date: 3/8/21
Purpose: This is to create a faster, more efficent Hash Map
This Hash Map's structure will be a 2D Array, This will connect to a Dummy Head Node which will connect to AVL Trees each of which contating a linked list 

*/

//To Do List: 

//Create a fill Hash Map Method
//This both allocates and fills the new Hash Map
struct DummyHeadNode*** Create_HashMap();
//Create a Get Dummy Head Node Method

//This allocates the hash map
struct DummyHeadNode*** Allocate_HashMap(); 

/*function for adding a row to the hash map, creating the tree set
@param currWord --> The word to start off the row 
@param header --> This is the header that will be added to
@note This tree set being created or added to stores all data with the same first letter and same first vowel*/
struct TreeSetNode* addRow(char* currWord, struct DummyHeadNode* header);

//function for adding a column
void addCol(char* currWord, struct word* header);

struct word* getList(const char* word, struct DummyHeadNode*** HashMap);

void Print_HashMap(struct DummyHeadNode** (*HashMap)); 

//Frees the Hash Map
void Free_HashMap(struct DummyHeadNode*** HashMap); 

//Restrictions for getting the list
struct word *getList_Restrictions(char* input, struct word*** HashSet, int cap, struct DummyHeadNode **(*HashMap));

FILE *OpenFile(); 

#endif
