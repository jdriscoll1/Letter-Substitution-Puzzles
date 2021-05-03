/*
Title: FLWGGame.c
Description: This file will contain the functions for the four letter word game
Author: Jordan Driscoll
Date: 5/1/21

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FLWGGame.h"
#include "GameFunctions.h"
#include "UserInput.h"
#include "Minimax.h"

extern int numLetters; 

int FLWG(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//So, first choose a start word
	int word = Convert_WordToInt("ways", WordToInt_HashMap); //ChooseStart(IntToWord_HashMap); 

	//Variable that determines winner: 1 - Algorithm, 0 - player
	int winner = -1;
	
	//How deep does the bot check? 
	int depth = 5; 
	 int rounds = 0; 
	char* wordStr;   
	//Determines if a word is valid
	int isValid = 0; 
	int whoseTurn = 0; 
	while(word >= 0){
		//Output the current word
		printf("%s\n", Convert_IntToWord(word, IntToWord_HashMap)); 
		
		if(whoseTurn == 0){
			word = botPly(word, depth, IntToWord_HashMap); 
		}
		else if(whoseTurn == 1){
			word = botPly(word, depth, IntToWord_HashMap); 	
			
		
		}
		
		whoseTurn = (whoseTurn + 1) % 2;  
		if(word == -1){
			winner = whoseTurn; 
		}
		rounds++; 
		
	}
	printf("%s Wins!\n%d Rounds\n", (winner == 0) ? "User": "Bot", rounds); 
	//return winner
	return winner; 
	
	
	
	
	
	
}

int Input_FLWG(int prevWord, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	int isValid = 0; 
	int wordID; 
	char* wordStr; 
	//Then, the player says a word
	while(isValid != 1){
		printf("Choose a word: "); 
		//Player chooses word
		//NOTICE -- DISCREET MALLOC
		wordStr = toLowerCase(Take_Input_NoSize());  
		if(strcmp(wordStr, "\n") != 0){	
			wordStr = strtok(wordStr, "\n"); 
		}
		if(strcmp(wordStr, "end") == 0){
			free(wordStr); 
			return -1; 
		}
		wordStr[numLetters] = '\0'; 
		//if the word is end -- game over 
		
		//It checks if the word is valid
		isValid = Check_Input(prevWord, wordStr, WordToInt_HashMap, IntToWord_HashMap); 
		if(isValid == 1){
			wordID = Convert_WordToInt(wordStr, WordToInt_HashMap);
			//if the word has been used
			if(getAlgFound(wordID, IntToWord_HashMap) == 1){
				printf("Word Already Used.\n"); 
				free(wordStr); 
				isValid = 0; 
			}
		}
		else{
			free(wordStr); 
		}
		
	}

	//Add the word to the hash map
	setAlgFound(wordID, IntToWord_HashMap); 

	free(wordStr); 
	return wordID; 
	
	
}

int botPly(int word, int depth, struct wordDataArray* IntToWord_HashMap){


	//Then, the bot takes this word and runs the minimax algorithm
	struct minimaxOutput* output = minimax(word, depth, depth, 1, IntToWord_HashMap);
	 
	//If it returns NULL -- game over
	if(output == NULL){
		//The bot won, there is nothing to free
		return -1;  
		
	}
	word = output->id;
	setAlgFound(word, IntToWord_HashMap);

	free(output); 
	return word; 
	
}

int userPly(int word, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//Take the user's input
	return Input_FLWG(word, WordToInt_HashMap, IntToWord_HashMap); 
	
	
	
}
