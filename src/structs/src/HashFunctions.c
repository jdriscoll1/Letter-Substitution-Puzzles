#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


#include "../includes/HashFunctions.h"

extern int numLetters; 

/*returns First Letter of a word*/
int FirstHashFunction(const char firstLetter){
	int output = firstLetter - 97; 
	//Returns the word 
	if(output < 0 || output > 25){
		return -1; 
	}
	//Returns the letter it acquired
	return output; 
	
}
/*returns the fist vowel in the word*/
int SecondHashFunction(const char* word){
	
	char vowels[] = {'a', 'e', 'i', 'o', 'u', 'y'}; 
	/*The parser that goes through each letter in the word*/
	int wordParse = 0; 
	int vowelCheck; 
	/*Loops through until it finds a vowel*/
	for(wordParse = 1; wordParse < numLetters; wordParse++){
		 vowelCheck = VowelSearch(word[wordParse], vowels);
		 if(vowelCheck != -1){
			 return vowelCheck; 
			 
		 }
		
		
	}
	vowelCheck = VowelSearch(word[0], vowels); 
	return (vowelCheck != -1) ? vowelCheck : 0;
}
/*Loops through a word searching for a letter*/
int VowelSearch(char letter, char vowels[6]){
	int vowelParse = 0; 
	/*Loops through all of the vowels and checks if it the word matches it*/
	for(vowelParse = 0; vowelParse <= 5; vowelParse++){
		if(letter == vowels[vowelParse]){
			/*returns the first vowel in teh word*/
			return vowelParse; 
			
		}
		
		
	}
	return -1; 
}


