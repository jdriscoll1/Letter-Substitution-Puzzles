/*The arrays .c all the methods for the arrays*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Arrays.h"
#include "TreeSet.h"

extern int numLetters; 


struct array* init_Array(int length, enum arrayType type){
	struct array* a = malloc(sizeof(struct array)); 
	a->length = length; 
	a->arr = Allocate_Array(length); 
	a->type = type; 
	
}

struct array2D* init_Array2D(int dim1Size, int dim2Size, enum arrayType type){
	struct array2D* a = malloc(sizeof(struct array2D)); 
	a->dim1Length = dim1Size; 
	a->dim2Length = dim2Size; 
	a->arr = Allocate_2DArray(dim1Size, dim2Size); 
	a->type = type; 
	
	
	
}

void free_Array(struct array* a){
	free(a->arr); 
	free(a); 
}

void free_2DArray(struct array2D* a){
	Free_2DArray(a->dim1Length, a->arr, 0);
	free(a);  
}


void** Allocate_Array(int size){
	/*ALlocates the array*/ 
	void** array = malloc(sizeof(void*) * size);  

	return array; 
	
} 

void Print_Array(int size, void** array, enum arrayType type){
	int i;  
	
	
	
	for(i = 0; i < size - 1; i++){
		if(type == INT){
			printf("%i, ", (int)(*(((int*)(array)) + i))); 
		}
		else if(type == STRING){
			printf("%s, ", (char*)array); 
		}
	
	}
	if(type == INT){
 		printf("%i.", (int)(((int*)array)[size - 1])); 
	}
	else if(type == STRING){
		printf("%s.", (char*)((char**)(array)[size-1]));
	}
	
	
}
int* FillRange_Array(int min, int max, int randomized){
	int* newArray = (int*)Allocate_Array(1+ (max - min));
	int i = 0; 
	for(i = 0; i <= max - min; i++){
		newArray[i] = min + i; 
	}
	if(randomized == 1){
		int* randArray = (int*)Randomize_Array((max - min) + 1, (void**)newArray, INT); 
		free(newArray); 
		return randArray; 
	}
	return newArray; 
	
	
}

void* Randomize_Array(int size, void** array, enum arrayType type){ 
	if(type == INTEGER){
		int* randValues = (int*)Allocate_Array(size); 
	
		/*A new array that will take all of the elements of the old array*/ 
		int* newArray = (int*)Allocate_Array(size); 
		/*The index of the array, gone through one by one*/ 
		int index = 0; 
		(*(((int*)randValues) + index)) = rand() % size; 
		/*Tree set to store the used words
		@param --> the first number that will be added, the first number that is chosen on the run*/
		struct DummyHeadNode *header = Allocate_TreeSet(&(*(((int*)randValues) + index))); 
		/*have the integer go through values, making sure that each one is not added to the tree set*/
		for(index = 0; index < size; index++){  
 
			(*((int*)newArray + (*((int*)randValues + index)))) = *(((int*)(array)) + index);
		 
			/*Checks to make sure that the index hasn't already been added'*/ 
			do{
			
				*((int*)randValues + index + 1) = rand() % size;   
			}
			/*If it's 1, we want to try again'*/ 
			while(Search_TreeSet(&*(randValues + index + 1), header->start, type) != NULL && index < size - 1);  
			AddNode_TreeSet(&*(randValues + index + 1), (void*)header, header->start, DUMMY, type);
			
		}
		return newArray;	
	
	}
	 
	/*go through each value fo the array, and place them at values of the old array*/
	/*Return the new array, free the old one*/   
	   
}

void*** Randomize_2DArray(int dim1Size, int dim2Size, void*** array, enum arrayType type){
	int* randValues = (int*)Allocate_Array(dim1Size); 
	void*** newArray = Allocate_2DArray(dim1Size, dim2Size);
	int index = 0;  
	(*(((int*)randValues) + index)) = rand() % dim1Size; 

	/*Tree set to store the used words

	@param --> the first number that will be added, the first number that is chosen on the run*/

	struct DummyHeadNode *header = Allocate_TreeSet(&(*(((int*)randValues) + index))); 

	for(index = 0; index < dim1Size; index++){
		
		strcpy((*((char**)newArray + (*((int*)randValues + index)))), *(((char**)(array)) + index)); 
		
		do{
		
			*((int*)randValues + index + 1) = rand() % dim1Size;  
		
		}while(Search_TreeSet(&*(randValues + index + 1), header->start, type) != NULL && index < dim1Size - 1); 
		
		AddNode_TreeSet(&*(randValues + index + 1), (void*)header, header->start, DUMMY, type);  
	} 
	Free_TreeSet(header->start, INTEGER); 
	free(header); 
	free(randValues); 
	return newArray; 
}

void Copy_2DArray(int dim1Size, void*** array1, void*** array2, enum arrayType type){
	int i; 
	for(i = 0; i < dim1Size; i++){
		if(type == STRING){
			strcpy((char*)*((char**)array1 + i), (char*)*((char**)array2 + i)); 
		}
	}
	
}

void*** Allocate_2DArray(int dim1Size, int dim2Size){
	int i; 
	//The Size of the 2D Array
	void*** TwoDimArray = calloc(dim1Size, sizeof(void**)); 
	for(i = 0; i < dim1Size; i++){
		*(TwoDimArray + i) = malloc(sizeof(void*) * dim2Size); 
	}
	return TwoDimArray; 
}


char** ConvertWordLLTo2DArray(struct word *header){
	/*Creates the string array that'll store the words of the linked list, and bonus, will be returned, which has to be 2D
	@param getSize(header) --> gets the size of the linked list
	@param numLetters + 1 --> enough numbers for the word, and the \0 */ 
	char** stringArray = (char**)Allocate_2DArray(getSize(header), numLetters + 1);  
	//the string array's index
	int i = 0; 
	//until it's on the last word
	while(header->next != NULL){
		//go to the next location
		header = header->next;
		//input the word into the array 
		strcpy(stringArray[i],  header->word);
		i++;  
		
	}
	return stringArray; 
}


void Print_2DArray(int dim1Size, void*** array, enum arrayType type){
	int i;   
	for(i = 0; i < dim1Size - 1; i++){
		if(type == STRING){
			printf("%s, ", (char*)(*(((char**)(array)) + i))); 
		}
	}
	printf("%s.", (char*)(((char**)(array))[dim1Size - 1])); 
}
void Free_2DArray(int dim1Size, void*** array, int skipFirst){
	int i; 
	for(i = 0; i < dim1Size; i++){
		if( !(skipFirst == 1 && i == 0) ){
			free(*(array + i)); 
		}
	}
	free(array); 
} 
char** ExtrapolateAllWords(){
	/*First, Open all of the files*/ 
	char* wordDocuments[3] = {"WordDocuments/Two_Letters.txt", "WordDocuments/Three_Letters.txt", "WordDocuments/Four_Letters.txt"};
	/*Four Letter Word Document*/
	FILE *flwd = fopen(wordDocuments[numLetters - 2], "r"); 
	if(flwd == NULL){
		printf("Cold Dog.");	
	}
	int i = 0; 
	/*Then go through it*/
	int totalWordCount[3] = {30, 590, 2235};
	char** allWords = calloc(totalWordCount[numLetters - 2], sizeof(char*)) ; 
	char word[numLetters + 2]; 
	while(fgets(word, numLetters + 3, flwd) != NULL && i < totalWordCount[numLetters - 2]){
		word[numLetters] = '\0'; 
		*(allWords +  i) = malloc(sizeof(char) * (numLetters + 1)); 
		strcpy(allWords[i], word);  
		i++;  
	} 
	fclose(flwd); 
	return allWords; 	
}

int BinarySearch_Array(int size, void* goal, void* array, enum arrayType type){
	int index = size / 2;
	int max = size; 
	int min = 0; 
	int comparison = compare_Array(array, index, goal, type); 
	while(comparison != 0){
		
		if(comparison == 1){
			min = index; 
			index = (max + index) / 2; 
		
		}
		else{
			max = index;  
			index = (min + index) / 2;
			 
			
			
		}

		comparison = compare_Array(array, index, goal, type); 
		
	} 
	
	return index; 
}
/*Compares two "objects"*/
int compare_Array(void* array, int index, void* c2, enum arrayType type){
	if(type == INT){
		int c1 = ((int*)(array))[index]; 
		if(c1 > *(int*)c2){
			return 2; 
		}
		else if(c1 < *(int*)c2){
			return 1; 
		}
		else{
			return 0; 
		}
	}
	
	
	else if(type == STRING){
		char* c1 = ((char**)(array))[index];
		if(strcmp(c1, (char*)c2) > 0){
			return 2; 
		}
		else if(strcmp(c1, (char*)c2) < 0){
			return 1; 
		}
		
		else{
			return 0; 
		}
		
	}
	else{
		char c1 = ((char*)(array))[index]; 
		if((char)c1 > *(char*)c2){
			return 2; 
		}
		else if((char)c1 < *(char*)c2){
			return 1; 
		}
		else{
			return 0; 
		}
		
	}
	

	
}


