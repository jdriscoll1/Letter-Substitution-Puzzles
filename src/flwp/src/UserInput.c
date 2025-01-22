#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "../includes/UserInput.h"
#include "../includes/GameFunctions.h"



#define MAX_SIZE 256

//This takes the input, and returns a pointer -- this is with a known size
char* Take_Input(int size){
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
		input[size] = '\0';
		/*Copies the information from the temp back onto the input*/ 
		
		safeStrcpy(&input, (const char*)temp, size, size);
		/*The temp value is now, no longer an issue*/ 
		free(temp);    
		 
  		return input; 
	}
	/*If the user is dumb, they get to try again*/ 
	Take_Input(size);
	return input; 
	 
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


int Check_Input(int prevWord, const char* currWord, struct DataStructures *data){
	//First, find prev word 
	char* prev = Convert_IntToWord(prevWord, data->I2W);

	int i = 0;
	int equalLetters = 0;  
	//for(i = 0; i < strlen(currWord)+1; i++){
	int terminate = 0; 
	while(terminate == 0){
		terminate = (*(currWord + i) == '\0') ? 1 : 0;
		if(i > data->I2W->numLetters){
			// WORD IS TOO LONG
			return TOO_LONG; 

		}
		else if(i < data->I2W->numLetters && *(currWord + i) == '\0'){
			return TOO_SHORT; 
		
		}
		i++; 
		  
	}
	
	//First: Make sure it is a real word
	//Go into the hash map
	//Find it in the hash map
	int id = Convert_WordToInt((char*)currWord, data); 
	// Check if the word is used
	if(equalLetters == data->I2W->numLetters + 1 || checkIfUsed_WordSet(id, data->wordSet)){
		return WORD_USED;
	}
 	if(inDictionary(id) == 0){
		return WORD_DOES_NOT_EXIST; 
	}
	switch(Order_Check(prevWord, Convert_WordToInt((char*) currWord, data), data)){

		case(0):
			return VALID;
		case(1):
			return NOT_ENOUGH_LETTERS_IN_COMMON; 
		case(2):
			return WRONG_ORDER; 
	}
	
	return UNKNOWN_ERROR; 
	
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
	return -1; 
		
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


int Order_Check(int w1, int w2, struct DataStructures* data){
	int A[26] = {0};
	int B[26] = {0};
	char* a = Convert_IntToWord(w1, data->I2W); 
	char* b = Convert_IntToWord(w2, data->I2W); 
	// not necessary in right order
	int lettersInCommon = 0; 
	// in right order
	int equalLetters = 0; 
	
	for(int i = 0; i < data->I2W->numLetters; i++){
		A[a[i] - 'a']++; 
		B[b[i] - 'b']++; 
		if(a[i] == b[i]){
			equalLetters++; 
		}
	}
	for(int i = 0; i < 26; i++){
		if(A[i] == B[i]){
			lettersInCommon++; 	
		}
	}
	if(equalLetters == data->I2W->numLetters - 1 && lettersInCommon == data->I2W->numLetters - 1){
		return 1; 	
	}
	if(equalLetters < data->I2W->numLetters - 1) {
		return 2; 
	}
	return 0; 
}
