#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "2DLinkedList.h"
#include "HashFunctions.h"

/*The Functions that define the 2 Dimensional Linked List*/

/*Adds a row to the linked list. (Rightwards)*/
void AddRow_2DLL(char* input, struct wordConnections *header){
	
	/* I need to loop through here, beginning at the header, and find the next null area, then I'll fill it with "word" */
	/* Then, I'll set the next Row and it's columns previous & next to null */
		

	while(header->nextRow != NULL){
		/* This moves the header to the next row */
		header = header->nextRow; 
		
		
	}
	
	/* Sets some space in memory to the next row */
	header->nextRow = malloc(sizeof(struct wordConnections)); 
	 
	/* Sets the next row to NULL, for 2 reasons: a. It won't be filled in with any random jazz, b. We'll be able to find it in the future */
	header->nextRow->nextRow = NULL; 
	/* Then it sets the word in the current header to the input */
	header->nextRow->word = input;

	/* After that, it sets space aside for the next Column */
	//header->nextRow->nextColumn = malloc(sizeof(struct wordConnections)); 
	header->nextRow->nextColumn = NULL; 
	
	
	
	
} 

/* Add Column <Downwards> */
void AddColumn_2DLL(char* input, struct wordConnections *header){
	/* So, what I need to do, is I need to Go through all of the rows, then, when the next row will be null, then. I need to flip through the columns, setting the current one to the previous column */
	
	/* First, scroll through the rows, and gets to the last one */
	while(header->nextRow != NULL){
		/* Goes into the next Row's Location */
		header = header->nextRow; 		
	}
	/* Then, scroll through the columns, setting this current location to the next location */
	while(header->nextColumn != NULL){
		/* Goes into the next Column's Location */
		header = header->nextColumn; 
		
	}
	
	/* This allocates a place in space for the next Column */
	header->nextColumn = malloc(sizeof(struct wordConnections));
	/* Then, it sets the value of the next Column to NULL */
	header->nextColumn->nextColumn = NULL; 
	/* The word in this node is set to the input */
	header->nextColumn->word = input;


}
/*This removes a word from a column in the two dimensional linked list*/ 
void RemoveFromColumn_2DLL(char* rowWord, char* columnWord, struct wordConnections *header){
	/*Moves off of the header*/
	header = header->nextRow; 
	/*Has to loop through the two dimensional linked list*/
	while(header != NULL && strcmp(header->word, rowWord) != 0){
		
		header = header->nextRow; 
	}
	if(header == NULL){
		printf("Could Not Find Row Word\n");
		exit(0);  
	}
	
	/*Then, must loop through the column and remove the word*/
	struct wordConnections *prev = header;
	int isFound = 0; 
	while(isFound == 0 && header != NULL){
		prev = header; 
		header = header->nextColumn;
		
		/*If it's found the word, it has no choice, but to remove it'*/ 
		if(strcmp(header->word, columnWord) == 0){
			prev->nextColumn = header->nextColumn;
			struct wordConnections temp = *header; 	
			free(header);
			header = temp.nextColumn;
			isFound = 1;    
			
			
		
		} 
		
		
	}  
}


void Print_2DLL(struct wordConnections *header){
	/*Start off by ripping through the dummy header*/
	header = header->nextRow; 
	/*While we are still in a row*/
	while(header != NULL){
		/*If i is 0, then it is printing the row, otherwise, it is printing the column*/ 
		int i = 0; 
		/*temp space, such that when we get to the final column, it can hop back to the row*/
		struct wordConnections tempRow = *header; 
		/*While loop that goes through the columns*/
		while(header != NULL){
			if(i == 0){printf("%s: ", header->word);}
			else{printf("%s ", header->word);}
			header = header->nextColumn;
			i++; 
		}
		header = tempRow.nextRow; 
		printf("\n");  
	}
	
	
}

void Free_2DLL(struct wordConnections *header){
	int first = 0; 
	while(header != NULL){							
		struct wordConnections temp = *header; 
		struct wordConnections tempColumn;  
		if(first != 0){
		
			while(header != NULL){				
				tempColumn = *header;
				free(header);
				header = tempColumn.nextColumn; 	
			
			}
		}
		else{
			free(header); 
		}
		
		header = temp.nextRow; 				
		first = 1; 					
	}  	
}

