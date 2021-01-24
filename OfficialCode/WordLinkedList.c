#include "WordLinkedList.h"
/*The defined Lists of The Singularly Linked Lists*/
/*Word Node Functions*/
/*Add to the back*/
/*@param header -- the front of the linked list*/
/*@param word -- the word to be added*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define true 1 
#define false 0  
typedef int bool;

int getSize(struct word *header){
	int size = 0; 
	while(header->next != NULL){
		header = header->next; 
		size++; 
	}
	return size; 
}
 
 void AddToFront_WordLL(char* word, struct word *header, int dataMalloc){
 	struct word *newNode = malloc(sizeof(struct word));
 	newNode->next = header->next; 
 	newNode->word = word; 
 	newNode->dataMalloc = dataMalloc; 
	header->next = newNode;
 	
 }

void AddToBack_WordLL(char* word, struct word *header, int dataMalloc){
	struct word *newNode = malloc(sizeof(struct word));
	newNode->next = NULL;
	newNode->word = word;
	newNode->dataMalloc = dataMalloc; 
	/*You want to get to the very last one, and set it to the new node*/  
	while(header->next != NULL){
		header = header->next; 
	}
	header->next = newNode; 
}
void Remove_WordLL(char* word, struct word *header){
	//prev always stays behind the header so that it can connect with the next node after header, and header can be freed
	struct word *prev; 
	bool isFound = false; 
	while(isFound == false && header->next != NULL){
		prev = header;  
		header = header->next;
		 
		//if the word is found
		if(strcmp(word, header->word) == 0){
			prev->next = header->next;  
			free(header); 
			isFound = true; 
		}
	}

	
	
	//if(isFound == false)
		//printf("***Word Not Found: %s Remove Word Linked List***\n", word); 
	//Loop until it finds the loop, saving a previous location
		//When it finds it, set the previous to the next one and free the current one
	
}
/*Remove first index*/
void RemoveFront_WordLL(struct word *header){
	//we need this such that it can be freed
	struct word *temp = header->next;
	header->next = header->next->next; 
	free(temp);
	 
	
	
}
void RemoveFrom_WordLL(char* word, struct word *header){
	bool found = false; 
	struct word* prev = header; 
	header = header->next; 
	while(header != NULL && found == false){
		if(strcmp(word, header->word) == 0){
			found = true; 
			
		}
		else{
			prev = header; 
			header = header->next;
		}
	}
	if(found == true){
		Free_WordLL(header); 
		prev->next = NULL; 
	}
	
	
}

int Search_WordLL(char* word, struct word* header){
	header = header->next; 
	while(header != NULL){
		if(strcmp(word, header->word) == 0){
			return 1; 
		}
		header = header->next; 
	}
	return 0; 
}
void RemoveBack_WordLL(struct word *header){
	struct word *temp = header; 
	while(header->next != NULL){
		temp = header; 
		header = header->next; 
	}
	free(header); 
	temp->next = NULL; 
	  
}

char* FindLast_WordLL(struct word* header){
	header = header->next; 
	while(header->next != NULL){
		header = header->next; 
	}
	return header->word; 
	
}

/*Print the word Linked List*/
void Print_WordLL(struct word *header, enum output o){
	char* link; 
	if(o == LINES){
		link = "\n"; 
		printf("\n\nList:\n");
	}
	else{
		link = (o == LINKED)?"->":", ";
	}
	header = header->next;
	while(header != NULL){
		printf("%s", header->word); 
		if(header->next != NULL){
			printf("%s",link); 
		}
		header = header->next; 
	}
	printf("\n"); 
	
}

/*Free Word Node LL*/
void Free_WordLL(struct word *header){
	
	while(header != NULL){
		struct word temp = *header; 
		if(header->dataMalloc == 1){
			free(header->word); 
		}
		free(header); 
		header = temp.next;
  
	}
	
}

/*This is intended to replace a linked list*/ 

struct word *Copy_WordToWordLL(struct word *copiedTo, struct word *copiedFrom){
	struct word *copiedToHeader = copiedTo; 
	copiedFrom = copiedFrom->next;  
	while(copiedFrom != NULL){
		AddToBack_WordLL(copiedFrom->word, copiedTo, 0);
		copiedFrom = copiedFrom->next; 
		
	}
	return copiedToHeader; 
}
