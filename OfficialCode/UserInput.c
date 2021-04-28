#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "UserInput.h"
#include "GameFunctions.h"



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
		safeStrcpy(&temp, (const char*)input, size, size);
		/*Takes care of the \n*/ 
		fgets(input, size, stdin);
		/*Makes the last character on the string the end of the string*/  
		input[size] == '\0';
		/*Copies the information from the temp back onto the input*/ 
		
		safeStrcpy(&input, (const char*)temp, size, size);
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


int Check_Input(int prevWord, const char* currWord, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//Has to make sure that word is numLetters letters
	//Has to make sure that word has numLetters - 1 letters in commond
	//First, find prev word 
	char* prev = Convert_IntToWord(prevWord, IntToWord_HashMap);
	
	
	int i = 0;
	int equalLetters = 0;  
	//for(i = 0; i < strlen(currWord)+1; i++){
	int terminate = 0; 
	while(terminate == 0){
		terminate = (*(currWord + i) == '\0') ? 1 : 0;
		if(i > numLetters){
			printf("Word is too long\n"); 
			return 3; 

		}
		else if(i < numLetters && *(currWord + i) == '\0'){
			printf("Word is too short\n");
			return 2; 
		
		}
		else if(equalLetters < numLetters - 1 && *(currWord + i) == '\0'){
			printf("Not enough letters in common\n");
			return 4;
		  
		}

		if(*(prev + i) == *(currWord + i)){
			equalLetters++; 
		}
		i++; 
		  
	}

	
	if(equalLetters == numLetters + 1){
		printf("Word is Equal to Prev\n");
		return 5;
	}
	//First: Make sure it is a real word
	//Go into the hash map
	//Find it in the hash map
	int id = Convert_WordToInt((char*)currWord, WordToInt_HashMap); 
 
	if(inDictionary(id) == 0){
		printf("Word does not exist\n"); 
		return 6; 
	}
	
	//Originally, I was going to check if the word has already been used, but now I am making the concious decision to say that
	//reusing the word is a valid move, because it can show them that they made need to remove words

	else if(equalLetters == numLetters){ 
		return 1; 
	}
	
}

int ContinueGames(){
	printf("\nWould you like to advance??\ny - harder level \nr - equal level\np - easier level\nq - quit\n");
	
	char* s = Take_Input_NoSize();
	//If the user chooses to quit it returns 1
	if(strcmp(s, "q\n") == 0 || strcmp(s, "n\n") == 0){
		free(s); 
		return 1; 
	}
	//If the uesr wants to continue, it returns 0 
	else if(strcmp(s, "y\n") == 0){
		free(s); 
		return 0;  
	}  
	
	//If the user wants to go back a level, it'll return 2
	else if(strcmp(s, "p\n") == 0){
		free(s);
		return 2; 
	}
	
	//If the user wants to stay at the same level it'll return 3 
	else if(strcmp(s, "s\n") == 0){
		free(s); 
		return 3; 
	}
	else if(strcmp(s, "r\n")== 0){
		free(s); 
		return 4; 
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

void safeStrcpy(char** dest, const char* src, int minLength, int maxLength){
	//Makes sure it's not too long
	if(maxLength >= 4096){
		printf("Max Length is too Long");
		exit(0);  
	}
	//Make sure the min isn't longer than the max
	if(minLength > maxLength + 1){
		printf("Source String Is Too Long");
		exit(0);  	
	}
	int i; 
	//Goes through and copies it 
	for(i = 0; i < minLength && src[i] != '\0'; i++){
		(*dest)[i] = src[i]; 
	}
	(*dest)[i] = '\0'; 
	
}

//Start will begin at where it left off
int safeStrtok(char* line, char token, int start){
	//I'm going to take a word, and return an array of strings
	int i = start + 1; 
	//It will loop until it reaches the last node
	while(line[i] != '\0'){
		if(line[i] == token){
			return start; 
		}
		i++; 
	} 
	
	return start; 
	
	
}

int safeStrLen(char* word){
	int i = 0;
	char c;  
	while((c = word[i]) != '\0'){
		i++; 
	}
	return i; 
	
}



