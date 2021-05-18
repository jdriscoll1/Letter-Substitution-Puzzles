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
#include "MinimaxTests.h"

extern int numLetters; 

int FLWG(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//So, first choose a start word
	int word = 0;//Convert_WordToInt("care", WordToInt_HashMap); //ChooseStart(IntToWord_HashMap); 
	setAlgFound(word, IntToWord_HashMap); 
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
		//printf("%s\n", Convert_IntToWord(word, IntToWord_HashMap)); 
		
		if(whoseTurn == 0){
			
			word = botPly(word, depth, IntToWord_HashMap, minimax); 	
		}
		else if(whoseTurn == 1){
			word = botPly(word, depth, IntToWord_HashMap, minimax_NoBeta);
			
			
		 
		}
		
		whoseTurn = (whoseTurn + 1) % 2;  
		if(word == -1){
			winner = whoseTurn; 
		}
		rounds++; 
		
	}
	printf("%s Wins!\n%d Rounds\n", (winner == 0) ? "Bot A": "Bot B", rounds); 
	//return winner
	return winner; 
	
	
	
	
	
	
}

void FLWG_Test(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	int A = 0; 
	int B = 0; 
	//So, first choose a start word
	int w = 0; 
	int i = 0; 
	for(i = 0; i < 300; i++){
		w = i; 
		setAlgFound(w, IntToWord_HashMap); 
		//Variable that determines winner: 1 - Algorithm, 0 - player
		int winner = -1;
		//How deep does the bot check? 
		int depth = 5; 
		int rounds = 0; 
		char* wordStr;   
		//Determines if a word is valid
		int isValid = 0; 
		int whoseTurn = 0; 
		while(w >= 0){
			if(whoseTurn == 0){
				
				w = botPly(w, depth, IntToWord_HashMap, minimax); 	
			}
			else if(whoseTurn == 1){
				w = botPly(w, depth, IntToWord_HashMap, minimax_NoBeta);
				
				
			 
			}
			
			whoseTurn = (whoseTurn + 1) % 2;  
			if(w == -1){
				winner = whoseTurn; 
			}
			rounds++; 
			
		}
		printf("%s Wins!\n%d Rounds\n", (winner == 0) ? "Bot A" : "Bot B", rounds); 
		if(winner == 0){
			A++; 
		} 
		else{
			B++; 
		}
		reset_HashSet(IntToWord_HashMap);
	}
	printf("A: %d, B: %d", A, B); 
	
	
	
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


int botPly(int word, int depth, struct wordDataArray* IntToWord_HashMap, struct minimaxOutput* (*minimax_func)(int, int, int, int, struct wordDataArray*) ){


	//Then, the bot takes this word and runs the minimax algorithm
	struct minimaxOutput* output = (*minimax)(word, depth, depth, 1, IntToWord_HashMap);
	 
	//If it returns NULL -- game over
	if(output == NULL){
		//The bot won, there is nothing to free
		return -1;  
		
	}
	word = output->id;
	setAlgFound(word, IntToWord_HashMap);
	//Print_MinimaxOutput(output);
	free(output); 
	 
	return word; 
	
}

int userPly(int word, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	//Take the user's input
	return Input_FLWG(word, WordToInt_HashMap, IntToWord_HashMap); 
	
	
	
}
