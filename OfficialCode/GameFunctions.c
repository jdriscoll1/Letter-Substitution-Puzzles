#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "GameFunctions.h"
#include "WordLinkedList.h"
#include "BreadthFirstSearch.h"
#include "HashFunctions.h"
#include "UserInput.h"
#include "Arrays.h"
#include "PathGameComponents.h"
#include "HashMap.h"


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



/*Randomly chooses a word based on an index*/ 
char* ChooseStart(char** allWordsArray, struct wordConnections **(*HashMap), int TEST_TEMP){
	//srand(time(0)); 
	//the total number of words
	int totalWordCount[3] = {30, 590, 2233}; 
	 
	//randomly choosese a word via an index
	int randIndex = rand() % (totalWordCount[numLetters - 2] + 1);

	char* word; 
	//TEST_TEMP FOUND HERE -- sets the index to 2, such that it is able
	if(TEST_TEMP == 1){
		//randIndex = 27;
		//Also try bevy 
		randIndex = 164; 
		
	}
	word = allWordsArray[randIndex]; 


	
	//It checks to make sure that it can connect to _at least_ one word
	struct word* connections = hashMapOutput(word, HashMap);  
 	
	if(connections->next != NULL){
		Free_WordLL(connections); 
		return word; 
	}
	Free_WordLL(connections);
	//If the word it happens to choose can't connect to anything (for example, ahoy) it retries
	return ChooseStart(allWordsArray, HashMap, 0);  
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
int goalCheck(char* input, char* goal, int isValid){

	if(strcmp("\n\0", input) == 0){
		free(input); 
		return 0; 
		
	}

	char* inputTrue = strtok(input, "\n"); 
	//if user decides to quit
	if(strcmp(inputTrue, "finish") == 0 || strcmp(inputTrue, "q") == 0){
		free(input); 
		return 2; 
	}
	//Is the inputted word equal to the goal word
	int gameWon = (isValid == 1 && strcmp(input, goal) == 0) ? 1 : 0; 
	
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
void ResetUndo(struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage, struct word* userConnections, int* undoCalls){
	//To replace the linked list
	//Free the linked list without freeing the header 
	Free_WordLL(userConnections->next);
	userConnections->next = NULL;  
	//Then add the new list to the back of it 
	//@param storage->next->listHeader - we need next because taht's how it worked when you printed it out, otherwise you may accidentally print out the header
	Copy_WordToWordLL(userConnections, (*storage)->next->listHeader);
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
	printf("After typig a command, please press enter\n");
}


int round_FLWP(int minConnections, char** allWords, struct wordConnections **(*HashMap), struct GameComponents* gc, struct PathfinderGame *pc){

	gc->hc->hintPoints = pc->hintPoints; 
	int endCondition;
	char* input;  
	printf("Your goal is to start at %s, and arrive at %s\nYou have %d hint points.\n", gc->start, gc->goal, gc->hc->hintPoints); 
	//If the user asks to remove a word
	int isRemove; 
	int isValid; 
	do{
		//The user has not yet asked to remove a word
		isRemove = 0; 
		isValid = 0; 
		input = toLowerCase(Take_Input_NoSize());
		if(strcmp(input, "\n\0") != 0){
			
			
			if(strcmp(input, "\n") != 0){	
				input = strtok(input, "\n"); 
			}
			//Runs a command based on the input 
			//cmd = Interpret_Input(userConnections, prevInput, input);
			//If they want to remove from a word
	
	
			if(strchr(input, '-') != NULL){
				RemoveWord_Struct(gc, input, 1); 
				isRemove = 1; 
			}
			else if(input[0] == '?'){
				input = substr(input, 1, numLetters + 1, 1);
				int inDic = inDictionary(input, HashMap); 
				if(inDic == 1){
					printf("That is a valid word\n"); 
				}
				else{
					printf("That is not a valid word\n"); 
				}
			}
			 
		
			//if they want to undo their previous move
			else if(strcmp(input, "u") == 0){
				Undo_Struct(gc);
			}
			// if cmd equals r -- it will be time to redo the previous move*/
		
			else if(strcmp(input, "r") == 0){
				Redo_Struct(gc);
				
			}
			else if(strcmp(input, "g") == 0){
				printf("Your goal word is %s\n", gc->goal); 
			}
			//Help command
			else if(strcmp(input, "h") == 0){
				Help(gc->goal); 
			}
			else if(strcmp(input, "1") == 0){
				char* output = hint1((unsigned long long)gc); 
				printf("%s\n", output);
				free(output);  
				
			}
			else if(strcmp(input, "2") == 0){
				char* output = hint2((unsigned long long)gc, HashMap); 
				printf("%s\n", output);
				free(output);  
			}
			else if(strcmp(input, "3") == 0){
				char* output = hint3((unsigned long long)gc, HashMap); 
				printf("%s\n", output);
				free(output);  
			}
			else if(strcmp(input, "p") == 0){ 
				printf("Hint Points Available: %d", gc->hc->hintPoints); 
		
		
			}		
			else if(strcmp(input, "q") != 0 && strcmp(input, "finish") != 0){
				isValid = AddWord_Struct(gc, input, HashMap); 
			}
			
				
	
			printf("%s", (char*)gc->aList->list); 
			
			printf("\n"); 
		
		
		}
		//If the user removes a word (isRemove == 1), then we don't need to check. Otherwise, we do
	}while((isRemove == 1)?1:(endCondition = goalCheck(input, gc->goal, isValid)) == 0); 
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

