#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "GameFunctions.h"
#include "WordLinkedList.h"
#include "BreadthFirstSearch.h"
#include "HashFunctions.h"
#include "UserInput.h"
#include "Arrays.h"
#include "PathGameComponents.h"


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
char* ChooseStartWord(char** allWordsArray, struct wordConnections **(*HashMap), int TEST_TEMP){
	srand(time(0)); 
	//the total number of words
	int totalWordCount[3] = {30, 590, 2233}; 
	//randomly choosese a word via an index
	int randIndex = rand() % (totalWordCount[numLetters - 2] + 1);
	char* word; 
	//TEST_TEMP FOUND HERE -- sets the index to 2, such that it is able
	if(TEST_TEMP == 1){
		//randIndex = 27;
		//Also try bevy 
		word = "ruff";  
	}else{
		word = allWordsArray[randIndex]; 
	}

	
	//It checks to make sure that it can connect to _at least_ one word
	struct word* connections = linkOutput(word, HashMap[FirstHashFunction(word[0])][SecondHashFunction(word)], NULL, NEITHER_SET, 0);  
 	
	if(connections->next != NULL){
		Free_WordLL(connections); 
		return word; 
	}
	Free_WordLL(connections);
	//If the word it happens to choose can't connect to anything (for example, ahoy) it retries
	return ChooseStartWord(allWordsArray, HashMap, TEST_TEMP + 1);  
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
int goalCheck(char* input, char* goal){
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
	int gameWon = (strcmp(inputTrue, goal) == 0) ? 1 : 0; 
	
	//it is necessary to free the input after it has been checked (kind of the whole reason for this method)
	free(input); 
	return gameWon; 
	
	
}
/*This is the function which is the pathfinder game. You call this. You play the four letter word pathfinder*/
int PathfinderGame(int challenge, char** allWords, char** wordStorage, struct wordConnections **(*HashMap)){
	//enum Difficulty difficulty = ChooseDifficulty();
	//int minConnections = GetMinConnections(difficulty); 
	 
	int minConnections = challenge; 
	char** shortestConnection = ChoosePath(allWords, HashMap, minConnections); 
	
	//This is the list of words the user is typing to find the exit 
	struct word* userConnections = malloc(sizeof(struct word)); 
	userConnections->next = NULL;
	userConnections->dataMalloc = 0; 
	AddToBack_WordLL(strdup(shortestConnection[0]), userConnections, 1); 
	
	//Create the storage of all of the users moves
	struct GenericLinkedListNode *storage = malloc(sizeof(struct GenericLinkedListNode)); 
	storage->next = NULL; 
	storage->prev = NULL; 
	//This is the header that stores the storage --> Keeps track of all users moves such that if (s)he so chooses (s)he may be able to go back
	struct GenericLinkedListNode *storageHeader = storage;  
	int undoCalls = 0; 
	int numMoves = 0; 
	char* prevInput = malloc(numLetters + 1); 
	strcpy(prevInput, shortestConnection[0]); 
	AddToFront_GenericLinkedListNode(storage, WORD_LL); 
	CopyInto_GenericLinkedListNode(userConnections, storage, 1, WORD_LL);
	//In order to be able to undo something, make a move, and have that initial, undone move be gone, it is necessary to free it
	//To track the number of numbers that must be freed, each time they undo a move, there must be a number that tracks how many times they have undone a move
	
	printf("\nStart off with: %s Your goal is: to %s", shortestConnection[0], shortestConnection[minConnections]); 
	
	printf("\nStart: %s\n", shortestConnection[0]);  

	

	int gameEndCondition = GameInput(undoCalls, numMoves, prevInput, minConnections, shortestConnection, userConnections, HashMap, storage, storageHeader); 
	AfterGameOutput(gameEndCondition); 
	free(prevInput); 
	//Interpret Input
	//Loop from interpret input 
	Free_WordLL(userConnections); 
	
	
	Free_2DArray(minConnections + 2, (void***)shortestConnection, 0);   
	Free_GenericLinkedList(storageHeader); 
	return (gameEndCondition == 2) ? 1 : 0;  

	
}

char** ChoosePath(char** allWords, struct wordConnections*** HashMap, int minConnections){
	char** shortestConnection; 
	//printf("Your Commands are: \nAdd <word> - which adds a word.\nRemove <word> - Removes a word, and all words after it.\nUndo - Undoes your previous turn\nFinish - Ends the game\nHelp - Shows this again"); 
	
	do{
	
		char* start = ChooseStartWord(allWords, HashMap, 0);  
		shortestConnection = (char**)BreadthFirstSearch(start, NULL, minConnections + 1, HashMap, HASH_SET); 

	}while(shortestConnection == NULL); 
	return shortestConnection; 
	
}

void AfterGameOutput(int gameEndCondition){
	switch(gameEndCondition){
	
		case(0):
			printf("\nError!\n");
			break;
		case(1):
			printf("\nCongratulations! You Won\n");
			break; 
		case(2):
			printf("\nBetter Luck Next Time.\n");
			break;
	}
}

void Undo(int* undoCalls, int* numMoves, struct GenericLinkedListNode **storage, char** prevInput){
	if(*numMoves == 0){
		printf("No move to return to.\n"); 
	}
	else{
		//There has been an undo
		++*undoCalls; 
		//The number of moves has gone up 
		--*numMoves; 
  
		*storage = (*storage)->next; 
		//Change previousInput. It has to be changed here so that it if I add one, it will be able to recognize it
		strcpy(*prevInput, FindLast_WordLL((*storage)->next->listHeader));  
				 
	}	
}
void Redo(int* undoCalls, int* numMoves, struct GenericLinkedListNode **storage, char** prevInput){
	/*if it is even possible for a user to redo a move*/
	if(*undoCalls == 0){
		printf("No move to be redone"); 
	}
	else{
		//Undoes an undo
		--*undoCalls; 
		//Brings back a move
		++*numMoves; 
		//Changes the turn to the previous one 
		*storage = (*storage)->prev; 
		//makes the previous input the new one
		strcpy(*prevInput, FindLast_WordLL((*storage)->next->listHeader));  
	}
}

void AddWord(int* undoCalls, int* numMoves, char** prevInput, char** cmd, struct word* userConnections, struct GenericLinkedListNode **storage, struct GenericLinkedListNode *storageHeader, struct wordConnections **(*HashMap)){
	int isValid = Check_Input(*prevInput, *cmd, HashMap);  
	if(isValid == 1){
		//If I have previously undone a move, I need to free that move		
			if(*undoCalls != 0){
				ResetUndo(storageHeader, storage, userConnections, undoCalls);
			}
			AddToBack_WordLL(strdup(*cmd), userConnections, 1);
			strcpy(*prevInput, *cmd); 
			AddToFront_GenericLinkedListNode(*storage, WORD_LL); 
			CopyInto_GenericLinkedListNode(userConnections, *storage, 1, WORD_LL); 
			++*numMoves;
	}
	//if it is not valid, it just sets the command to "-" such that nothing will happen with it
	else{
		//make it so that you can't just type in the word by getting rid of the word you chose
		strcpy(*cmd, "-"); 
	}


				
			
}
void RemoveWord(int* undoCalls, int* numMoves, char** cmd, char **prevInput, struct word *userConnections, struct GenericLinkedListNode *storageHeader, struct GenericLinkedListNode **storage){
	//If I have previously undone a move, I need to free that move
	if(*undoCalls != 0){
		ResetUndo(storageHeader, storage, userConnections, undoCalls); 
	
	}
	//First we have to remove the exclamation point
	*cmd = substr(*cmd, 1, numLetters + 1, 1); 		
	//Then we have to Remove it from the list, and all the words after it 
	RemoveFrom_WordLL(*cmd, userConnections->next);
			
	//We have to change the word to be compared to the last word in the list
	strcpy(*prevInput, FindLast_WordLL(userConnections));
			
	//This adds to the storage such that the user can undo a move
	AddToFront_GenericLinkedListNode(*storage, WORD_LL); 
	CopyInto_GenericLinkedListNode(userConnections, *storage, 1, WORD_LL); 
			
	++*numMoves; 
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
	//printf("\nYour goal is to start off with the start word, and through letter substitution, find your way to teh gaol word!\nThe commands you are allowed are:\n<word>, which adds a word to the list\n-<word> (put a hyphine in front of the word), which removes a word from the list, and all the words after it\nu - which undoes your previous turn.\nq - Which ends the game\nGood Luck, and have a wacky good time!\n\n");
	
	
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


int GameInput(int undoCalls, int numMoves, char* prevInput, int minConnections, char** shortestConnection, struct word* userConnections, struct wordConnections **(*HashMap), struct GenericLinkedListNode *storage, struct GenericLinkedListNode *storageHeader){
		
		
		
		char* input; 
		int gameEndCondition; 
		do{
		//Take input 
		 
		input = toLowerCase(Take_Input_NoSize()); 
		if(strcmp(input, "\n") != 0){	
			input = strtok(input, "\n"); 
		}
		//Runs a command based on the input 
		//cmd = Interpret_Input(userConnections, prevInput, input);
		//If they want to remove from a word


		if(strchr(input, '-') != NULL){
			RemoveWord(&undoCalls, &numMoves, &input, &prevInput, userConnections, storageHeader, &storage); 
		}
	
		//if they want to undo their previous move
		else if(strcmp(input, "u") == 0){
			Undo(&undoCalls, &numMoves, &storage, &prevInput);
		}
		// if cmd equals r -- it will be time to redo the previous move*/
	
		else if(strcmp(input, "r") == 0){
			Redo(&undoCalls, &numMoves, &storage, &prevInput);
			
		}
		else if(strcmp(input, "g") == 0){
			printf("Your goal word is %s\n", shortestConnection[minConnections]); 
		}
		//Help command
		else if(strcmp(input, "h") == 0){
			Help(shortestConnection[minConnections]); 
		}
		
		else if(strcmp(input, "q") != 0 && strcmp(input, "finish") != 0){
			AddWord(&undoCalls, &numMoves, &prevInput, &input, userConnections, &storage, storageHeader, HashMap); 
		}
			
		//If there is no undo, it will print where we are, otherwise it will print the next location
		Print_WordLL((undoCalls == 0) ? userConnections: storage->next->listHeader, LINES); 
	
		printf("\n"); 
  
	
	}while((gameEndCondition = goalCheck(input, shortestConnection[minConnections])) == 0); 
	return gameEndCondition; 
	
	
	
}

int trueGame(int minConnections, char** allWords, char** wordStorage, struct wordConnections **(*HashMap)){
	//All it does it initialize the Pathfinder Pointers
	struct GameComponents *gc = InitializeGameComponents(allWords, HashMap, minConnections);
	int endCondition;
	char* input;  
	printf("Your goal is to start at %s, and arrive at %s", gc->shortestPath[0], gc->shortestPath[minConnections]); 
	do{
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
				printf("Your goal word is %s\n", gc->shortestPath[minConnections]); 
			}
			//Help command
			else if(strcmp(input, "h") == 0){
				Help(gc->shortestPath[minConnections]); 
			}
			
			else if(strcmp(input, "q") != 0 && strcmp(input, "finish") != 0){
				AddWord_Struct(gc, input, HashMap); 
			}
				
			//If there is no undo, it will print where we are, otherwise it will print the next location
		//	Print_WordLL((gc->undoCalls == 0) ? gc->userConnections: gc->storage->next->listHeader, LINES); 
			char* output = toString_WordLL(gc->storage->next->listHeader, LINKED); 
			printf("%s", output); 
			free(output); 
			printf("\n"); 
		
		
		}
		
	}while(endCondition = goalCheck(input, gc->shortestPath[minConnections]) == 0); 
	FreeGameComponents(gc);
	//Until the game is won it just loop s
	
}
