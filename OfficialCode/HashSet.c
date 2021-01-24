#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WordLinkedList.h"
#include "HashFunctions.h"

extern int numLetters; 

struct word ***AllocateHashSet(){
	/*The categories which the word is stored in, the first one is the first letter of the word, the second is the first vowel of the word*/
	struct word **(*HashMap) = malloc(26 * (numLetters + 2) *(sizeof(struct word))); 
	int parse1 = 0;
	int parse2 = 0;
	/*Sets up a column for all 26 letters of words (column 0: A, column 1: B, column 2: C etc.)*/
	for(parse1 = 0; parse1 < 26; parse1++){
		/* This frees up all of the space of the first category group */
			
		HashMap[parse1] = calloc(6, sizeof(struct word)); 
		if(HashMap[parse1] == NULL){
			printf("Code Stopped Working"); 
			exit(0);
		}
	
		for(parse2 = 0; parse2 < 6; parse2++){
		 
			 
			/* This allocates the linked list in the array */ 
				HashMap[parse1][parse2] = malloc(sizeof(struct word)); 
				
			/*If it a malloc fails, close the program down*/
				if(HashMap[parse1][parse2]== NULL){
					printf("Fail"); 
					exit(0); 
				}
		
			/* This makes it so the stuff pre-stored isn't junk */
			HashMap[parse1][parse2]->next = NULL;
			HashMap[parse1][parse2]->dataMalloc = 0; 
	
			
			
		}
	
	}

	return HashMap; 

}

/*Add to Hash Set */ 
void AddToHashSet(char* word, struct word **(*HashSet)){
	struct word *list = HashSetArray(word, (void*)HashSet); 
	AddToFront_WordLL(word, list, 0); 
	
}
int Search_HashSet(char* word, struct word **(*HashSet)){
	struct word *header = (struct word*)HashSetArray(word, ((void*)(HashSet))); 
	header = header->next; 
	while(header != NULL){
		if(strcmp(header->word, word) == 0){
			return 1; 
		}
		header = header->next; 
	}
	return 0; 
}
//Remove From Hash Set
int Remove_HashSet(char* word, struct word **(*HashSet)){
	struct word *list = (struct word*)HashSetArray(word, ((void*)(HashSet))); 
	Remove_WordLL(word, list); 
	
}


/*Free Hash Set */  
void Free_HashSet(struct word **(*HashSet)){
		 
	const int rowMax = 26; 
	const int columnMax = 6; 
	
	int arrayRow = 0;
	int arrayColumn = 0;  
	/*Traverse Outter Array*/
	for(arrayRow = 0; arrayRow < rowMax; arrayRow++){
		 
		/*Then Traverse the inner array*/
		for(arrayColumn = 0; arrayColumn < columnMax; arrayColumn++){
			
			/*Frees the 2D Linked List*/
			struct word *header = HashSet[arrayRow][arrayColumn];  	
			Free_WordLL(header); 
		}
		arrayColumn = 0; 
		free(HashSet[arrayRow]); 
		/*Then go to the next Row*/
	}
	/*Free category*/
    free(HashSet); 
}  
