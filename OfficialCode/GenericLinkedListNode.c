#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TreeSet.h"
#include "WordLinkedList.h"
#include "GenericLinkedListNode.h"
//Add one
void Add_GenericLinkedListNode(struct GenericLinkedListNode *header, const enum listType type){
	while(header->next != NULL){
		header = header->next; 
		
	}
	header->next = malloc(sizeof(struct GenericLinkedListNode)); 
	header->next->prev = header; 
	header->next->listType = type; 
	header->next->next = NULL;

	
	
}

void Print_GenericLinkedList(struct GenericLinkedListNode *header){
	int i = 0; 
	header = header->next; 
	while(header != NULL){

		Print_WordLL(header->listHeader, LINKED); 
		i++; 
		
		header = header->next; 
	}
	printf("\n"); 
}


void CopyInto_GenericLinkedListNode(void* copy, struct GenericLinkedListNode *header, int intoFront, const enum listType type){
	//if I want to copy it into the front of the Generic linked list node, it only occurs once 
	if(intoFront == 1){
		header = header->next; 
		
	}
	//if I want it to copy it into the back, it has to loop all the way to the back 
	else{
		while(header->next == NULL){
			header = header->next; 
		}
	}
	//if it's a word linked list that I'mma copy in 
	if(type == WORD_LL){
		//then I have to malloc some info fro the list header 
		header->listHeader = malloc(sizeof(struct word)); 
		//then I set the next to null to not cause any friction 
		((struct word*)(header->listHeader))->next = NULL; 
		((struct word*)(header->listHeader))->dataMalloc = 0;
		//Then I move copy because I do not need a new header 
		copy = ((struct word*)(copy))->next; 
		//While the copy isn't null, add it's word to the back of the generic's sublist
		while(copy != NULL){
			AddToBack_WordLL(strdup(((struct word*)(copy))->word), ((struct word*)(header->listHeader)), 1);
			copy = ((struct word*)(copy))->next;  
		}
	}
	
	
	
}

void AddToFront_GenericLinkedListNode(struct GenericLinkedListNode *header, const enum listType type){
		
		/*So, right now we are at the front, so we need to take the next one, and save it somehow
		Then, we need to malloc new information for the new one 
		Then, just keep adding into it*/
		struct GenericLinkedListNode* newNode = malloc(sizeof(struct GenericLinkedListNode)); 
		newNode->next = header->next; 
		newNode->prev = header; 
		newNode->listHeader = NULL;
		newNode->listType = type; 
		if(header->next != NULL){
			header->next->prev = newNode; 
		}
		header->next = newNode; 
	
}

struct GenericLinkedListNode *ReturnIndex_GenericLinkedListNode(const int index, struct GenericLinkedListNode *header){
	int i;
	if(index < 0){
		printf("Index can't be less than 0 [Return Index]"); 
		exit(0);
	} 
	header = header->next; 
	for(i = 0; i < index && header != NULL; i++){
		header = header->next; 

	}
	if(header == NULL){
		printf("Out of Bounds [Return Index]");
		exit(0);  
	}
	return header; 
}
//this only frees a certain number of items in a generic linked list
void FreeQuantity_GenericLinkedList(int freeAmount, struct GenericLinkedListNode *header){
	struct GenericLinkedListNode *headerStorage = header; 
	header = header->next; 
	for(; freeAmount > 0; freeAmount--){
		if(header != NULL){
			Free_WordLL(((struct word*)(header->listHeader))); 
			struct GenericLinkedListNode temp = *header; 
			free(header); 
			header = temp.next; 
		}
	}
	//This makes it so that the whole thing is not lost
	headerStorage->next = header; 
	
	
	
} 


//Free it
void Free_GenericLinkedList(struct GenericLinkedListNode *header){
	struct GenericLinkedListNode temp = *header; 
	free(header);  	
	header = temp.next; 
	while(header != NULL){
		 
		//If it's a tree set
		if(header->listType == TREE_SET_LL){
			Free_TreeSet(((struct DummyHeadNode*)(header)->listHeader)->start);  
			free(header->listHeader); 
		}
		
		else if(header->listType == WORD_LL ){
			Free_WordLL(((struct word*)(header->listHeader))); 
		}
		/*Temp variable for freeing the actual generic linked list node*/
		struct GenericLinkedListNode temp = *header; 
		free(header);  	
		header = temp.next; 
	}
	
	
}


