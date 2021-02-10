#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "UserInput.h"
#include "GameFunctions.h"
#include "HashMap.h"
//#include "2DLinkedList.h"

#define MAX_SIZE 256

extern int numLetters;
//This takes the input, and returns a pointer -- this is with a known size
char* Take_Input(int size){
	printf("\nGivith me thine fine input: "); 
	//In order to remember the jazz that happened in this function, outside of this function, we must use malloc
	char* input = malloc(sizeof(char) * size);   
	//We want to refrain from using scanf, because it is insecure. 
	if(fgets(input, size, stdin) != NULL)
	{
		/*The temporary variable that stores the important information, while fgets takes care of the \n*/ 
		char* temp = malloc(sizeof(char) * size); 
		/*Copies the important information into the temp*/ 
		strcpy(temp, input); 
		/*Takes care of the \n*/ 
		fgets(input, size, stdin);
		/*Makes the last character on the string the end of the string*/  
		input[size] == '\0';
		/*Copies the information from the temp back onto the input*/ 
		strcpy(input, temp);
		/*The temp value is now, no longer an issue*/ 
		free(temp);    
		 
  		return input; 
	}
	/*If the user is dumb, they get to try again*/ 
	Take_Input(size);
	 
}

char* Take_Input_NoSize(){
	int maxSize = 256; 
	char* input = malloc(MAX_SIZE);
	fgets(input, maxSize, stdin);
	return input;    
	
}

/*This asks the user what they'd like the difficulty to be*/ 
enum Difficulty ChooseDifficulty(){
	printf("What would you like the difficulty to be? \n0) Easy\n1) Medium \n2) Hard\n");
	char* difficulty = Take_Input(2); 
	if(strcmp(difficulty, "0") == 0){
		free(difficulty); 
		return EASY; 
	}
	else if(strcmp(difficulty, "1") == 0){
		free(difficulty); 
		return MEDIUM; 
	}
	else if(strcmp(difficulty, "2") == 0){
		free(difficulty); 
		return HARD; 
	}
	free(difficulty); 
	return ChooseDifficulty(); 
	 
	
}

char* Interpret_Input(struct word* userConnections, char* prevWord, char* input){
	//How to read input 
	//First, we need a scanner that'll scan through the input 
	//We need to tokenize the string, to do so, we need a delim
	
	//This is the first piece of input, the command 
	
	char* lower = toLowerCase(input); 
	char* cmd; 
	char* output = malloc(MAX_SIZE); 

	if(strchr(lower, ' ')){
		cmd = strtok(lower, " ");
	}
	else{
		cmd = strtok(lower, "\n"); 
	}
	
	//Determining what the command is: 
	if(strcmp(cmd, "add") == 0){
		/*Get the second word*/
		cmd = strtok(NULL, "\n"); 
		strcpy(output, cmd); 
	
	}
	else if(strcmp(cmd, "remove") == 0){
		cmd = strtok(NULL, "\n"); 
		strcpy(output, cmd); 
		strcat(output, "!"); 
		 
	}
	else if(strcmp(cmd, "undo") == 0){
		strcpy(output, "u");  
	}
	else if(strcmp(cmd, "redo") == 0){
		strcpy(output, "r"); 
	}
	else if(strcmp(cmd, "finish") == 0){
		strcpy(output, "q"); 
	}
	else if(strcmp(cmd, "help") == 0){
		strcpy(output, "f"); 
		 
	}
	else{
		printf("Unrecognized Command. Your Options are:\nAdd <Word>\nRemove <Word>\nUndo\nRedo\nFinish\n");
		strcpy(output, "f"); 
	}
	free(lower);
	return output; 

	   
}

int Check_Input(char* prevWord, const char* currWord, struct wordConnections ***HashMap){
	//Has to make sure that word is numLetters letters
	//Has to make sure that word has numLetters - 1 letters in commond
	//First, find prev word 

	
	int i = 0;
	int equalLetters = 0;  
	for(i = 0; i < strlen(currWord)+1; i++){
		if(*(prevWord + i) == '\0' && *(currWord + i) != '\0'){
			printf("Word is too long\n"); 
			return 3; 
			return 0; 
		}
		else if(*(prevWord + i) != '\0' && *(currWord + i) == '\0'){
			printf("Word is too short\n");
			return 2; 
			return 0; 
		}
		else if(equalLetters < i - 1){
			printf("Not enough letters in common\n");
			return 4;
		  
		}
		if(*(prevWord + i) == *(currWord + i)){
			equalLetters++; 
		}
	}

	
	if(equalLetters == numLetters + 1){
		printf("Word is Equal to Prev\n");
		return 5;
	}
	//First: Make sure it is a real word
	//Go into the hash map
	//Find it in the hash map
	if(SearchHashMap(HashMap, currWord) == NULL){
		printf("Word does not exist\n"); 
		return 6; 
	}
	
	//Originally, I was going to check if the word has already been used, but now I am making the concious decision to say that
	//reusing the word is a valid move, because it can show them that they made need to remove words

	else if(equalLetters == numLetters){ 
		return 1; 
	}
	
}

char* toLowerCase(char* input){
	//This is the new, lowercase word that will be used for comparing
	char* lower = malloc(sizeof(char) * MAX_SIZE); 
	//This is the index
	int i = 0; 
	//Goes through and takes the character from input, lowercases, and puts it into lower
	while(*(input + i) != '\0'){
		
		*(lower + i) = tolower(*(input + i));  
		i++; 
	}
	*(lower + i) = '\0';
	free(input); 
	
	return lower; 
	
}
//This takes a string, and extracts a substring from it 
char* substr(char* word, int start, int end, int replace){
	size_t s = end - start + 2; 
	char* subWord = malloc(s); 
	int i; 
	for(i = start; i <= end; i++){
		*(subWord + (i - start)) = *(word + i);  
	}
	
	subWord[s-1] = '\0';
	if(replace == 1){
		free(word); 
	} 
	return subWord; 
}

int safeStrcat(char** dest, const char* src, int destLength, int buff, int start){
	//It starts at where it left off last time
	//As long as it is less than the buff, and it's not hit a null pointer, and it's destination can contain it  it's good to go. 
	int i = start; 
	 
	
	for(; i < buff + start; i++){
		if(destLength == i){
			printf("Error - Safe String Cat: Array Out of Bounds Exception");
			exit(0); 
		}		
		
		
		(*dest)[i] = src[i-start];
				
	}
	

	//The end of the destination is a null pointer
	(*dest)[i] = '\0'; 
	return i; 
	//So how this is supposed to work. 
}

