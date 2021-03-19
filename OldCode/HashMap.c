#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


#include "HashSet.h"
#include "HashMap.h"
#include "2DLinkedList.h"
#include "HashFunctions.h"
#include "Arrays.h"

extern int numLetters; 

struct wordConnections ***AllocateHashMap(){
	/*The categories which the word is stored in, the first one is the first letter of the word, the second is the first vowel of the word*/
	struct wordConnections **(*HashMap) = malloc(26 * (numLetters + 2) *(sizeof(struct wordConnections))); 
	int parse1 = 0;
	int parse2 = 0;
	/*Sets up a column for all 26 letters of words (column 0: A, column 1: B, column 2: C etc.)*/
	for(parse1 = 0; parse1 < 26; parse1++){
		/* This frees up all of the space of the first category group (a e i o u y)*/
			
		HashMap[parse1] = calloc(6, sizeof(struct wordConnections)); 
		if(HashMap[parse1] == NULL){
			printf("Code Stopped Working"); 
			exit(0);
		}
	
		for(parse2 = 0; parse2 < 6; parse2++){
		 
			 
			/* This allocates the linked list in the array */ 
				HashMap[parse1][parse2] = malloc(sizeof(struct wordConnections)); 
		
				
			/*If it a malloc fails, close the program down*/
				if(HashMap[parse1][parse2]== NULL){
					printf("Fail"); 
					exit(0); 
				}
		
			/* This makes it so the stuff pre-stored isn't junk */
			HashMap[parse1][parse2]->nextRow = NULL;
			HashMap[parse1][parse2]->nextColumn = NULL;
	
			
			
		}
	
	}

	return HashMap; 

}


char** FillHashMap(struct wordConnections **(*HashMap), int isRandom){
	/* The Amount of words in each file, File 1, 2, 3 */
	int totalWordQuantity[3] = {133, 7421, 19852}; 
	/*What we store each wrod in*/ 
	char* word; 
	char* rowWord = malloc(sizeof(char) * (numLetters + 1));
	
	/*Four Letter Word Document*/
	FILE *flwd = OpenFile(); 
	/* is it the first word of the line */
	int wordIndex = 0; 
	 

	/* So, this is a temporary word, who will store the row, so all of its category folks will be contained within it */
	/*This stores all of the words*/
	char** wordStorage = (char**)calloc(totalWordQuantity[numLetters - 2], sizeof(char*));
	int i; 
	for(i = 0; i < totalWordQuantity[numLetters - 2]; i++){
		wordStorage[i] = malloc(sizeof(char) * numLetters + 1);  
	}
	/*the length of the line*/
	//int lineLength[3] = {134, 106, 126}; 
	char line[200]; 
	int max = 0; 
	/*This goes through every single line in the file, assuming there are <= 200*/
	while(fgets(line, 200, flwd) != NULL){
		if(strlen(line) > (unsigned int)max){
			max = strlen(line); 
		}
		i = 0; 
		/*This is a linked list that is used to randomize the 4th dimension of the hash map*/
		struct word *randomizerList;
		if(isRandom == 1){
			randomizerList = malloc(sizeof(struct word)); 
			randomizerList->next = NULL; 
		} 
		/*First, it inputs all of the words that would normally be added to the Column*/  
		//if the line has more than one word, it will skip all 
		int end = (strchr(line, ' ') == NULL)? 1:0; 
		int lastWord = 0;  
		word = strtok(line, " "); 
		/*This goes through the entirety of each line*/ 
		while(end == 0) {
			/*Set the last character of the word to \0, making it an official string*/ 
			word[numLetters] = '\0'; 
			
			strcpy(wordStorage[wordIndex], word);
 
			if(i == 0){
				
				strcpy(rowWord, word); 
				AddRow_2DLL(wordStorage[wordIndex], HashMap[FirstHashFunction(word[0])/*First Letter*/][SecondHashFunction(word)]/*First Vowel*/); 
				wordIndex++;
			}
			else{	
				/*If we do not want to randomize the 2nd dimension of the linked list (4th of the HashMap). We just insert it normally*/ 
				if(isRandom == 0){
					AddColumn_2DLL(wordStorage[wordIndex], HashMap[FirstHashFunction(rowWord[0])/*First Letter*/][SecondHashFunction(rowWord)]/*First Vowel*/); 
					wordIndex++;
				}
				/*If we do want to randomize the 2nd dim of the linked list, we add it to the word linked list*/
				else{
					AddToBack_WordLL(word, randomizerList, 0); 
				}
			}
			
			
			
			
			/*Tokenizes the next word off of the string*/ 
			word = strtok(NULL, " "); 
			
			i++;
			/*if it is the last word in a line, it will end this particular loop*/ 
			end = (lastWord == 1)?1:0; 
			
			/*If it knows the it is the last */  
			if(end == 0){
				/*This searches a word to see if it has a \0, if it does, it will say that it is the last word*/ 
				lastWord = (strchr(word, '\n') == NULL)?0:1; 
		 
			}
			
			
		}
		/*If it got to the end, and there is only one measely word on the line, that is, a word has 0 connections (i.e. void)*/ 
		if(i == 0){ 
	
			word[numLetters] = '\0'; 
			
			strcpy(wordStorage[wordIndex], word); 
			
			AddRow_2DLL(wordStorage[wordIndex], HashMap[FirstHashFunction(word[0])/*First Letter*/][SecondHashFunction(word)]/*First Vowel*/);  
			wordIndex++;
			if(isRandom == 1){
				free(randomizerList); 
			}
		}
		else if(isRandom == 1){
			//We need teh size of the randomizer list
			int size = getSize(randomizerList); 
			/*So, now we have a linked list with all of the words for the second column*/ 
 
			/*Second, it converts the linked list to an array*/ 
			char** randomizerArray = ConvertWordLLTo2DArray(randomizerList); 
			/*Thirdly, it randomizes the order of the array*/
			char** randomizedArray = (char**)Randomize_2DArray(size, numLetters + 1, (void***)randomizerArray, STRING);
			 
			Free_2DArray(size, (void***)randomizerArray, 0); 
			Free_WordLL(randomizerList); 
		
			/*Alright, now I will need to add all of the values from the array to the appropriate dimension in the hash map, 
			that is the second dimension of the linked list in the hashmap*/ 
			int index; 
			for(index = 0; index < size; index++){
				strcpy(wordStorage[wordIndex], randomizedArray[index]);
				AddColumn_2DLL(wordStorage[wordIndex], HashMap[FirstHashFunction(rowWord[0])/*First Letter*/][SecondHashFunction(rowWord)]/*First Vowel*/);  
				wordIndex++; 
			}
			Free_2DArray(size, (void***)randomizedArray, 0); 
			
			/*Fourthly, it puts the array into the 4th dimension of the HashMap*/
			/*Finally, it frees the array & linked list*/   
		}
		 
	}

	fclose(flwd);
	free(rowWord); 
	return wordStorage; 
}

FILE *OpenFile(){
	char* wordDocuments[3] = {"WordDocuments/Two_Letter_Connections.txt", "WordDocuments/Three_Letter_Connections.txt", "WordDocuments/Four_Letter_Connections.txt"};
	/*Four Letter Word Document*/
	FILE *flwd = fopen(wordDocuments[numLetters - 2], "r"); 
	if(flwd == NULL){
		printf("Could not open file: %s\n", wordDocuments[numLetters - 2]);
		
	}
	return flwd;
}


struct wordConnections* SearchHashMap(struct wordConnections ***HashMap, const char* word){
	//First open the 2D Linked List
	struct wordConnections *list = HashMap[FirstHashFunction(word[0])][SecondHashFunction(word)];
	//pass the header
	list = list->nextRow; 
	while(list != NULL && strcmp(list->word, word) != 0){
		list = list->nextRow; 
		
	} 
	return list; 
	
	
}


/*Print out Hash Map*/
void PrintHashMap(struct wordConnections ***HashMap){
	const int rowMax = 26; 
	const int colMax = 6; 
	int row = 0; 
	int col = 0;
	printf("\nHash Map:\n"); 
	/*Go through each piece of the array*/
	for(row = 0; row < rowMax; row++){
		for(col = 0; col < colMax; col++){
			printf("\n[%d][%d]:\n", row, col); 
			Print_2DLL(HashMap[row][col]); 
		}
	}
}

/* This function will accept a word as an input, and output all of the connecting values */
struct word *hashMapOutput(char* input, struct wordConnections **(*HashMap)){
	/* Create the linked list into which I put the words */
	struct word *output = malloc(sizeof(struct word)); 
	output->next = NULL; 
	output->dataMalloc = 0; 
	 
	
	struct wordConnections *connections2D = HashMap[FirstHashFunction(input[0])][SecondHashFunction(input)]; 
	connections2D = connections2D->nextRow; 
	/* Loop through the array until I find the correct word */ 
	while(strcmp(connections2D->word, input) != 0){
		connections2D = connections2D->nextRow; 
	}
	//If it can't find it, it crashes
	if(connections2D == NULL){
		printf("Error: Can't find word [hashMapOutput]");
		exit(0);  
	}
	

	while(connections2D->nextColumn != NULL){
		connections2D = connections2D->nextColumn; 
		AddToBack_WordLL(connections2D->word, output, 0);  		
	
	}

	/* Return the Linked List */
	return output; 
	
	
	
	
	
}

struct word *hashMapOutput_Restrictions(char* input, struct word*** HashSet, int cap, struct wordConnections **(*HashMap)){
	int foundWords = 0; 
	/* Create the linked list into which I put the words */
	struct word *output = malloc(sizeof(struct word)); 
	output->next = NULL; 
	output->dataMalloc = 0; 
	 
	
	struct wordConnections *connections2D = HashMap[FirstHashFunction(input[0])][SecondHashFunction(input)]; 
	connections2D = connections2D->nextRow; 
	/* Loop through the array until I find the correct word */ 
	while(strcmp(connections2D->word, input) != 0){
		connections2D = connections2D->nextRow; 
	}
	//If it can't find it, it crashes
	if(connections2D == NULL){
		printf("Error: Can't find word [hashMapOutput]");
		exit(0);  
	}
	

	while(connections2D->nextColumn != NULL){
		
		connections2D = connections2D->nextColumn; 
		//Search the HashSet
		if(Search_HashSet(connections2D->word, HashSet) == 0){

			//If it's in the Hash Set it should add it to the back
			AddToBack_WordLL(connections2D->word, output, 0);  	
			//Then it should add it to the Hash Set
			AddToHashSet(connections2D->word, HashSet, 0); 
			//Then the words found should go up should go up
			foundWords++;  
			//If the cap has been met, it should return the output	
			if(foundWords == cap){
				return output; 
			}
		}
	}

	/* Return the Linked List */
	return output; 
	
	
	
	
	
}


/* This frees all of the words stored in the links list's words storage space */
void FreeHashMap(struct wordConnections ***HashMap){
	 
	const int rowMax = 26; 
	const int columnMax = 6; 
	
	int arrayRow = 0;
	int arrayColumn = 0;  
	/*Traverse Outter Array*/
	for(arrayRow = 0; arrayRow < rowMax; arrayRow++){
		 
		/*Then Traverse the inner array*/
		for(arrayColumn = 0; arrayColumn < columnMax; arrayColumn++){
			
			/*Frees the 2D Linked List*/
			struct wordConnections *header = HashMap[arrayRow][arrayColumn];  	
			Free_2DLL(header); 
		}
		arrayColumn = 0; 
		free(HashMap[arrayRow]); 
		/*Then go to the next Row*/
	}
	/*Free category*/
    free(HashMap); 


}

void FreeWordStorage(char** wordStorage){ 
	/* The Amount of words in each file, File 1, 2, 3 */
	int totalWordQuantity[3] = {133, 7421, 19852}; 
	int i; 
	for(i = 0; i < totalWordQuantity[numLetters - 2]; i++){
		free(wordStorage[i]); 
	}
	 
	free(wordStorage);
}





