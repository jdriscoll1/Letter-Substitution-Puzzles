/********************
Name: Jordan Driscoll
Date: July 14, 2019
Description: This creates the documents that will be used for the four letter word game

gcc -O -ansi -Wall -Wextra -o flwd LetterDocumentWriter.c
*******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	int alignedLetters = 0; 
	/*Words in a document*/
	int wordCount = 0; 
	/*Number of letters per word*/
	int maxNum = 2; 
	/* The character we are on in the All English Words file */
	char ch; 
	/*File of All English Words*/
	FILE *allWords = fopen("WordDocuments\\Four_Letters.txt", "r");
	/*File of All Certain Letter Quanitity Words*/
	FILE *letterQuantity = fopen("WordDocuments\\Four_Letter_Connections.txt", "w"); 
	/*The character that reads from the 2nd file*/
	char ch2; 
	/* The letter that we are on in going through the word */
	int letterCount = 0;	
	int letterCount2 = 0; 
	/* The current word that we are on  */
	char currentWord[maxNum + 2];
	/*The word which wil become the additional word*/
	char additionalWord[maxNum + 2];
	/* If the file can't be found, digga darn */
	if(allWords == NULL){
		printf("File Could Not Be Found");
		return 0; 
		
	}

	
	/* Goes through each individual letter */
	while((ch = fgetc(allWords)) != EOF){ 
		/* Sets the letter into the current word, assuming that it is within hte range */
		if(letterCount < maxNum + 1){
			currentWord[letterCount] = ch; 
		
			
		}
		/* Makes it go up each letter */
		letterCount++; 
			
		/* when it reaches a \n, it the letter count is going to determine if it gets written into the file  */
		if(ch == '\n'){
			letterCount2 = 0; 
			/*GOAL: Create a document, which has every word, and aligned with it, all the words that it can become */
			/*so, when it reaches a \n, it will throw on a \0*/
			currentWord[maxNum] = '\0';
		
			/* if the word has four letters, it needs to be +1 because of the line break  */
			if(letterCount == maxNum + 1){
				
				fputs(currentWord, letterQuantity);
				printf("%s", currentWord); 
				FILE *allWords2 = fopen("WordDocuments\\Two_Letters.txt", "r");
				if(allWords2 == NULL){
					printf("File Could Not Be Found");
					return 0; 
		
				}
				
				while((ch2 = fgetc(allWords2)) != EOF){
					/*Sets the letter into the word, if the letters are hte same, then if it's three, it'll put it into the file with a space*/
					additionalWord[letterCount2 + 1] = ch2; 
			
					if(currentWord[letterCount2] == additionalWord[letterCount2 + 1]){
						/* printf("%d. %c = %c", letterCount2, currentWord[letterCount2], additionalWord[letterCount2]); */
						alignedLetters++; 
						
						
					}
					letterCount2++; 
					if(ch2 == '\n'){
						
						additionalWord[0] = ' '; 
						additionalWord[maxNum + 1] = '\0'; 
						/* printf("Current Word: %s, Additional Word: %s; %d", currentWord, additionalWord, alignedLetters);  */
						if(alignedLetters == maxNum - 1){
							
							printf("%s", additionalWord); 
							fputs(additionalWord, letterQuantity);
							
							
						}
						alignedLetters = 0; 
						letterCount2 = 0; 
						/* printf("\n"); */
					}
					
					
					
				
				}
				printf("\n");
				fputs("\n", letterQuantity);
				fclose(allWords2);
			
				/* Sets letter count down  */
				letterCount = 0; 
			}
		}
	}
	/* Closes the File */
	fclose(allWords);
	fclose(letterQuantity); 
	printf("Word Count: %d", wordCount);
	return 0; 
	/* Let's just try to get it to read a file */
}
