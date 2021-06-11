/*The defined Lists of The Singularly Linked Lists*/
/*Word Node Functions*/
/*Add to the back*/
/*@param header -- the front of the linked list*/
/*@param word -- the word to be added*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../includes/WordLinkedList.h"

#include "../../flwp/includes/UserInput.h"

#include "../../structs/includes/ArrayList.h"


#define true 1 
#define false 0  
typedef int bool;

extern int numLetters;

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
	header->size++; 
 	
 }

void AddToBack_WordLL(char* word, struct word *header, int dataMalloc){
	struct word *newNode = malloc(sizeof(struct word));
	newNode->next = NULL;
	newNode->word = word;
	newNode->dataMalloc = dataMalloc;
	header->size++;  
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
	header->size++;  
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
	if(header->size == 0){
		printf("Cannot Remove From Front"); 
	}
	else{
	
		//we need this such that it can be freed
		header->size--; 
	
		struct word *temp = header->next;
		header->next = header->next->next; 
		free(temp);
	}
	 
	
	
}
void RemoveFrom_WordLL(char* word, struct word *header){
	bool found = false; 
	struct word* prev = header; 
	header->size--; 
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
	header->size--; 
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

char* toString_WordLL(struct word *header, enum output o){
	//Determine how the words will be linked together
	char* link;
	int linkLen;  
	if(o == LINES){
		link = "\n"; 
		linkLen = 1; 
	}
	else{
		link = (o == LINKED)?"->":", ";
		linkLen = 2; 
	}
	//Now I'm going to convert it to an array list, still gonna return the string though
	//I need an array list so that it can be infinitely large
	struct arrayList* aList = init_ArrayList(5, 10, STR); 
	 
	header = header->next; 
	int start = 0; 
	while(header != NULL){
		addString_ArrayList((const char*)header->word, numLetters, aList);  
		if(header->next != NULL){
			addString_ArrayList((const char*)link, 2, aList);  
			
			
		}
		header = header->next; 
	}
	if(aList->currPrecision == 0){
		printf("Linked List Empty [toString_WordLL]"); 
		exit(0); 
	}
	
	char* outputStr = malloc(aList->currPrecision + 1);
	safeStrcpy(&outputStr, (const char*)aList->list, aList->currPrecision, aList->currPrecision + 1); 
	free_ArrayList(aList); 	

	return outputStr; 
	
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
