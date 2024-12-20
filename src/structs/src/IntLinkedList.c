#include <stdio.h>
#include <stdlib.h>

#include "../includes/IntLinkedList.h"
#include "../includes/ArrayList.h"

#include "../../flwp/includes/UserInput.h"

/*This adds a word to the back of an integer linked list*/
void AddToBack_IntLL(int data, struct intList* header){
	header->size++; 
	//Loop through all of the nodes
	while(header->next != NULL){
		header = header->next; 
	} 
	//Header->next = Null at this point
	header->next = malloc(sizeof(struct intList));
	header->next->data = data;
	header->next->size = 0;  
	header->next->next = NULL;  
}

/*This adds a word to the front of an integer linked list*/
void AddToFront_IntLL(int data, struct intList* header){
	header->size++; 
	//Create the new node
	struct intList* newNode = malloc(sizeof(struct intList));
	newNode->data = data;
	newNode->size = 0; 
	//Set the current header's next to the new node's next 
	newNode->next = header->next; 
	
	//Set the header's new next node to the recently created new node
	header->next = newNode;  
}

void RemoveData_IntLL(int data, struct intList* header){
	struct intList* prev = header;
	struct intList* curr = header->next;
	 
	while(curr != NULL){
		if(curr->data == data){
			prev->next = curr->next; 
			free(curr); 
			curr = prev->next; 
			header->size--; 
		}
		else{
			prev = curr; 
			curr = curr->next; 
		}
		
	}
} 

int RemoveFirst_IntLL(struct intList* header){
	//curr takes the place as the item that will be removed 
	struct intList* curr = header->next; 

	//if the item to be removed is null, there's nothing to be removed
	if(curr == NULL){
		return -1; 
	}
	int output = curr->data; 
	//The header's next will point to curr's next
	header->next = curr->next; 
	
	//Free the currently lost object 
	free(curr); 
	return output; 
}




void Print_IntLL(struct intList* header){
	header = header->next; 
	while(header != NULL){
		printf("%d ", header->data); 
		header = header->next; 
	}
	
}

void PrintStrings_IntLL(struct intList* header, struct wordDataArray *IntToWord_HashMap){
	header = header->next; 
	while(header != NULL){
		printf("%s ", Convert_IntToWord(header->data, IntToWord_HashMap)); 
		header = header->next; 
	}
	
}


void Free_IntLL(struct intList* header){
	while(header != NULL){
		struct intList temp = *header; 
		free(header); 
		header = temp.next;  
		
		
	}
	
} 
int FindLast_IntLL(struct intList* header){
	header = header->next; 
	while(header->next != NULL){
		header = header->next; 
	}
	return header->data; 
	
}

void RemoveFrom_IntLL(int data, struct intList *header){
	int found = 0; 
	struct intList* prev = header; 
	struct intList* headerPtr = header; 
	header = header->next; 
	while(header != NULL && found == 0){
		if(data == header->data){
			found = 1; 
			
		}
		else{
			prev = header; 
			header = header->next;
		}
	}
	if(found == 1){
		while(header != NULL){
			struct intList temp = *header; 
			free(header); 
			header = temp.next;
			headerPtr->size--;   
		}
		prev->next = NULL; 
	}
	
	
}



struct intList *Copy_IntLLToIntLL(struct intList *copiedTo, struct intList *copiedFrom){
	struct intList *copiedToHeader = copiedTo; 
	copiedFrom = copiedFrom->next;  
	while(copiedFrom != NULL){
		AddToBack_IntLL(copiedFrom->data, copiedTo);
		copiedFrom = copiedFrom->next; 
		
	}
	return copiedToHeader; 
}

char* toString_IntLL(struct intList *header, enum output o, struct wordDataArray* IntToWord_HashMap){
	//Determine how the words will be linked together
	char* link;

	if(o == LINES){
		link = "\n"; 

	}
	else{
		link = (o == LINKED)?"->":", ";

	}
	//Now I'm going to convert it to an array list, still gonna return the string though
	//I need an array list so that it can be infinitely large
	struct arrayList* aList = init_ArrayList(5, 10, STR); 
	 
	header = header->next; 

	while(header != NULL){
		addString_ArrayList((const char*)Convert_IntToWord(header->data, IntToWord_HashMap), IntToWord_HashMap->numLetters, aList);  
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


