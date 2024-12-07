#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "../includes/UserInput.h"
#include "../includes/PathGameComponents.h"
#include "../includes/GameFunctions.h"

#include "../../algs/includes/BreadthFirstSearch.h"

#include "../../structs/includes/WordLinkedList.h"
#include "../../structs/includes/HashFunctions.h"


#include <stdint.h>

extern int numLetters; 


int GetMinConnections(enum Difficulty difficulty){
	int easyMin = 2; 
	int easyMax = 4;
	int medMin = 5;
	int medMax = 7; 
	int hardMin = 8; 
	int hardMax = 12; 
	if(difficulty == 0){
		/*Range, 2-4*/ 
		return (rand() % (easyMax - easyMin + 1)) + easyMin; 	
	}
	/*if medium 5-8*/
	/*if hard, 9-(12?)*/
	else{
		return (difficulty == 1) ? ((rand() % (medMax - medMin + 1)) + medMin) : ((rand() % (hardMax - hardMin + 1)) + hardMin);  
	} 
	
}

int getWordWithNumberOfConnections(int numConnections, struct wordDataArray* IntToWord_HashMap){
	int minId = -1; 
	int maxId = -1; 
	for(int i = 0; i < IntToWord_HashMap->numWords; i++){
		int currNumConnections = IntToWord_HashMap->array[i]->numConnections; 
	
		// if the number of  is equal to the numConnections that are desired 		
		if(currNumConnections == numConnections){
			if (minId == -1){
				minId = i; 	
			}
		}
		if(currNumConnections < numConnections){
			if (maxId == -1){
				maxId = i; 
			}

		}
	}	

	return (rand() % (maxId - minId)) + minId; 


}



/*Randomly chooses a word based on an index*/ 
int ChooseStart(struct wordDataArray* IntToWord_HashMap){
 	int wordID = 0;
	int validWord = 0;
	//randomly choosese a word via an index
	while(validWord == 0){
		wordID = rand() % (IntToWord_HashMap->numWords);
		if(IntToWord_HashMap->array[wordID]->numConnections >= 1){validWord = 1;}
	}
	
	return wordID;  
}


char* enumToString(enum Difficulty difficulty){
	/*Makes it such that we can output a string for an enum*/
	switch(difficulty){
		case 0: 
			return "easy"; 
		case 1: 
			return "medium"; 
		case 2: 
			return "hard"; 
	} 
	return "error"; 
}

/*Method that determines when the game will be stopped*/ 
int goalCheck(char* input, int goal, int isValid, int isCommand, struct DummyHeadNode*** WordToInt_HashMap){
	//This is the boolean that determines if the game is won
	int gameWon = 0; 

	if(strcmp("\n\0", input) == 0){
		free(input); 
		return 0; 
		
	}
	if(isCommand == 1){
		free(input); 
		return 0; 
	}
	if(isCommand == 2){
		free(input); 
		return 2; 
	}

	//if user decides to quit
	
	if(isValid == 1){
	

		int inputNum = Convert_WordToInt(input, WordToInt_HashMap); 
		//Is the inputted word equal to the goal word
		gameWon = (inputNum == goal) ? 1 : 0; 
	}
	//it is necessary to free the input after it has been checked (kind of the whole reason for this method)
	free(input); 
	return gameWon; 
	
	
}
  


void AfterGameOutput(int gameEndCondition, struct GameComponents* gc){
	switch(gameEndCondition){
	
		case(0):
			printf("\nError!\n");
			break;
		case(1):
			printf("\nCongratulations! You Won! Your Score is: %d%%\n", getScore(gc));
			//Don't forget, when they add a word, add it to the goal hash set
			break; 
		case(2):
			printf("\nBetter Luck Next Time.\n");
			break;
	}
}





//if there have been several undo calls, this will reset it
void ResetUndo(struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage, struct intList* userConnections, int* undoCalls){
	//To replace the linked list
	//Free the linked list without freeing the header 
	Free_IntLL(userConnections->next);
	userConnections->next = NULL;  
	//Then add the new list to the back of it 
	//@param storage->next->listHeader - we need next because taht's how it worked when you printed it out, otherwise you may accidentally print out the header
	Copy_IntLLToIntLL(userConnections, (*storage)->next->listHeader);
	//Move the storage to the front
	*storage = storageHeader;  
	//I have to free however many calls there are  
	FreeQuantity_GenericLinkedList(*undoCalls, storageHeader); 
	//Then I have to copy the word linked list into the current one 
	//I also have to set the storage to the storage header 
	*undoCalls = 0; 
}

void Help(char* goal){
	printf("Welcome to the four letter word pathway!\n");
	printf("Your goal is to start from the first, and by changing a single letter, turn it into the goal word.\n");
	printf("\n\nThe current goal word is: %s\n\n", goal);
	printf("The commands you have at your disposal are:\n");
	printf(" <Word> --> Adds a word to the list\n");
	printf(" -<Word> --> Removes a word from the list, and all words that follow it\n");
	printf(" u --> Undoes a move\n");
	printf(" r --> Redoes a move\n");
	printf(" finish or q --> Ends the game before the word is found\n");
	printf("Good luck!\n");         
	//printf("\nYour goal is to start off with the start word, and through letter substitution, find your way to teh gaol word!\nThe commands you are allowed are:\n<word>, which adds a word to the list\n-<word> (put a hyphine in front of the word), which removes a word from the list, and all the words after it\nu - which undoes your previous turn.\nq - Which ends the game\nGood Luck, and have a wacky good time!!\n");
	
	
}

void EnterText(){
	printf("Welcome to the Four Letter Word Game!\n");
	printf("Your goal is to start off with one word, and through the four letter word game (single letter substitution), make it into a goal word!!\n");
	printf("The commands you are allowed are:\n");
	printf("<word>  --> which will add a word to the list\n");
	printf("-<word> --> which removes a word from the list\n");
	printf("u --> undoes a move\n");
	printf("r --> redoes a move\n");       
	printf("q or finish --> ends game\n\n");
	printf("If you have any questions, please type h for help\n"); 
	printf("After typing a command, please press enter\n");
}


int round_FLWP(struct GameComponents* gc, struct PathfinderGame *pc, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){

	gc->hc->hintPoints = pc->hintPoints; 
	int endCondition;
	char* input;  
	printf("Your goal is to start at %s, and arrive at %s\nYou have %d hint points.\n", Convert_IntToWord(gc->start, IntToWord_HashMap), Convert_IntToWord(gc->goal, IntToWord_HashMap), gc->hc->hintPoints); 
	//If the user asks to remove a word
	int isCommand; 
	int isValid; 
	do{
		//The user has not yet asked to remove a word
		isCommand = 1; 
		isValid = 0; 
		input = toLowerCase(Take_Input_NoSize());
		if(strcmp(input, "\n\0") != 0){
			
			
			if(strcmp(input, "\n") != 0){	
				input = strtok(input, "\n"); 
			}

			//If they want to remove from a word
	
	
			if(input[0] == '-'){
				input = RemoveWord_Struct(gc, input, 1, WordToInt_HashMap, IntToWord_HashMap); 
		
			}
			else if(input[0] == '?'){
				input = substr(input, 1, numLetters + 1, 1);
				//Check the length
				int i = 0; 
				char c; 
				while((c = input[i]) != '\0'){
					i++; 
				}
				if(i == numLetters){
				
					int id = Convert_WordToInt(input, WordToInt_HashMap); 
					if(id > pow(26, numLetters) || id <= 0){
						
					}
					int inDic = inDictionary(id); 
					if(inDic == 1){
						printf("That is a valid word\n"); 
					}
					else{
						printf("That is not a valid word\n"); 
					}
				}
				else{
					if(i < numLetters){
						printf("That word is too short\n"); 
					}
					else{
						printf("That word is too long\n"); 
					}
					
				}
			}
			 
		
			//if they want to undo their previous move
			else if(strcmp(input, "u") == 0){
				Undo_Struct(gc, IntToWord_HashMap);
			}
			// if cmd equals r -- it will be time to redo the previous move*/
		
			else if(strcmp(input, "r") == 0){
				Redo_Struct(gc, IntToWord_HashMap);
				
			}
			else if(strcmp(input, "g") == 0){
				printf("Your goal word is %s\n", Convert_IntToWord(gc->goal, IntToWord_HashMap)); 
			}
			//Help command
			else if(strcmp(input, "h") == 0){
				Help(Convert_IntToWord(gc->goal, IntToWord_HashMap)); 
			}
			else if(strcmp(input, "1") == 0){
				char* output = hint1((uintptr_t)gc); 
				printf("%s\n", output);
				free(output);  
				
			}
			else if(strcmp(input, "2") == 0){
				char* output = hint2((uintptr_t)gc, IntToWord_HashMap); 
				printf("%s\n", output);
				free(output);  
			}
			else if(strcmp(input, "3") == 0){
				char* output = hint3((uintptr_t)gc, IntToWord_HashMap, wordSet); 
				printf("%s\n", output);
				free(output);  
			}
			else if(strcmp(input, "p") == 0){ 
				printf("Hint Points Available: %d", gc->hc->hintPoints); 
		
		
			}		
			else if(strcmp(input, "q") == 0 || strcmp(input, "finish") == 0){
				isCommand = 2; 
			}
			else{
				isValid = AddWord_Struct(gc, input, WordToInt_HashMap, IntToWord_HashMap, wordSet); 
				isCommand = 0; 
			}
			
				
	
			printf("%s", (char*)gc->aList->list); 
			
			printf("\n"); 
		
		
		}
		//Goal Check Returns: 
		//0 - The user hasn't finished
		//1 - The user won
		//2 - The user quit
	}while( (endCondition = goalCheck(input, gc->goal, isValid, isCommand, WordToInt_HashMap)) == 0)  ; 
	int score = (endCondition == 2)? -1 : getScore(gc); 
	AfterGameOutput(endCondition, gc); 
	return score; 

	
	//Until the game is won it just loop s
	
}

int getScore(struct GameComponents* gc){
	double a = gc->minConnections; 
	double x = gc->numMoves; 
	//This is how many times the min connection should be equal to 0%
	double c = 5; 
	double y = -( (x-a) / (a * (c - 1)) ) + 1;  
	y *= 100; 
	y = ceil(y); 
	//Makes sure y is never less than 0 
	if(y < 0){
		y = 0; 
	}
	return (int)y; 
}

int calcScore(int n1, int n2){
	double a = n1; 
	double x = n2; 
	//This is how many times the min connection should be equal to 0%
	double c = 5; 
	double y = -( (x-a) / (a * (c - 1)) ) + 1;  
	y *= 100; 
	y = ceil(y); 
	//Makes sure y is never less than 0 
	if(y < 0){
		y = 0; 
	}
	return (int)y; 
}

