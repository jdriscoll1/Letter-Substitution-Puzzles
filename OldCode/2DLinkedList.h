#ifndef seen2D
#define seen2D

#include "WordLinkedList.h"

/*
Purpose: All 2 Dimensional Linked List Functions
Uses: What the Hash Map uses in its 3rd and 4th dimension 


*/
/* The 2 Dimensional Linked List's Node */
struct wordConnections{
	/*The word that will be saved in each location*/
	char* word; 
	/*next word from the 2D List */
	struct wordConnections *nextRow;
	/* Next word of the 2D List */
	struct wordConnections *nextColumn;
	
};

/*Add Row To Hash Map -- The first dimension
always adds to the bottom
@param input --> The word to be added to a row
@param header --> The front of the linked list*/
void AddRow_2DLL(char* input, struct wordConnections *header); 

/*Add Column To Hash Map -- The Second Dimension
always adds to the very last row, and the very last column
@param input --> The word to be added to a row
@param header --> the front of the linked list*/
void AddColumn_2DLL(char* input, struct wordConnections *header);

  
 
/*Removes a word from a column in a 2 dimensional linked list
@param rowWord --> The row from which the word will be removed
@param columnWord --> That word that will be removed, it is in the column
@param header --> The Header of the two dimensional linked list*/ 
void RemoveFromColumn_2DLL(char* rowWord, char* columnWord, struct wordConnections *header); 

/*Prints out a 2D Linked List
@param --> the front of the linked list */
void Print_2DLL(struct wordConnections *header);

/*Frees the 2D Linked List that's locked inside of the array
@param header --> the front of the linked list 
*/
void Free_2DLL(struct wordConnections *header); 

#endif 
