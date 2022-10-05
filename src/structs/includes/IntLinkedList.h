#ifndef IntLL
#define IntLL

#include "../includes/HashMap.h"

/*
Name: IntLinkedList
Author: Jordan Driscoll
Date: 4/12/21
Description: This is the linked list designated for integers
*/

struct intList {
	int data; 
	int size; 
	struct intList* next; 
	
};

/*This adds a word to the back of an integer linked list*/
void AddToBack_IntLL(int data, struct intList* header); 

/*This adds a word to the front of an integer linked list*/
void AddToFront_IntLL(int data, struct intList* header); 

/*Removes the first index of a ll*/
int RemoveFirst_IntLL(struct intList* header); 

void RemoveData_IntLL(int data, struct intList* header); 


void Print_IntLL(struct intList* header); 

void PrintStrings_IntLL(struct intList* header, struct wordDataArray *IntToWord_HashMap); 



void Free_IntLL(struct intList* header); 

/*This goes through an integer linked list and finds the data from the last node*/
int FindLast_IntLL(struct intList* header); 

//This takes a list and removes a particular piece of data from it 
void RemoveFrom_IntLL(int data, struct intList *header); 

//This takes an integer linked list, and pastes it to the back of a different linked list
struct intList *Copy_IntLLToIntLL(struct intList *copiedTo, struct intList *copiedFrom); 

//Takes an integer linked list and converts it to a string
char* toString_IntLL(struct intList *header, enum output o, struct wordDataArray* IntToWord_HashMap); 

#endif
