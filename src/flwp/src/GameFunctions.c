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
#include "../../shared/includes/Log.h"
#include "../../shared/includes/Relax.h"

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

/* A word to open on, with about this many neighbours.
 *
 * "About", because the count is a preference. A band the dictionary cannot
 * fill used to come back as -1 and the whole board was thrown away and asked
 * for again from the same table, which is a loop rather than a retry. The
 * band is widened instead until the dictionary has something in it, and the
 * last round takes any word at all - so this answers with a word whenever
 * there is one to answer with, which is whenever the dictionary is not empty.
 */
int getWordWithNumberOfConnections(int minConnections, int maxConnections, struct DataStructures* data){
	struct Band asked = { minConnections, maxConnections };

	/* One round past the last, which is the one that used to be the last. Every
	round before it also refuses a word this board may not use; that round drops
	even the cap, so nothing this could answer before can fail to answer now. */
	for(int round = 0; round <= RELAXATION_ROUNDS; round++){
		int takeAnythingPlayable = (round == RELAXATION_ROUNDS);
		struct Band band = loosen(asked, round);

		int wordsWithinRange[data->I2W->numWords];
		int numWordsWithinRange = 0;
		for(int i  = 0; i < data->I2W->numWords; i++){
			int adj = getNumAdjacencies(i, data);
			/* The cap is not a preference the way the band is. A board opening
			on a word nobody has heard of is the fault this exists to stop, so
			it is refused every round rather than widened along with the band. */
			if(adj >= band.min && adj <= band.max
				&& (takeAnythingPlayable || !isTooObscure(i, data))){
				wordsWithinRange[numWordsWithinRange++]	= i;
			}
		}

		if(numWordsWithinRange > 0){
			if(round > 0){
				FLWG_LOG("No word had %d-%d neighbours; opened on one with %d-%d instead\n",
					minConnections, maxConnections, band.min, band.max);
			}
			return wordsWithinRange[rand() % numWordsWithinRange];
		}
	}

	/* Only reachable from an empty dictionary: the last round takes anything. */
	FLWG_LOG("There is no word to open on at all!\n");
	return -1;
}

void Shuffle_IntArray(int* values, int count){
	//Fisher-Yates: walk from the back, swapping each slot with a random earlier one
	for(int i = count - 1; i > 0; i--){
		int j = rand() % (i + 1);
		int temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	}
}

/* Where an adversarial game opens, with about this many ways out.
 *
 * About, for the same reason as the turns game: the count is what makes the
 * board easy or hard, not what makes it a board. A band nothing in the
 * dictionary fills is widened until something does, rather than handed back as
 * -1 for the app to throw the board away and ask again from the same table.
 */
int ChooseStart_Range(struct wordDataArray* IntToWord_HashMap, int minAdjacencies, int maxAdjacencies){
	struct Band asked = { minAdjacencies, maxAdjacencies };

	/* One round past the last, which is the one that used to be the last. Every
	round before it also refuses a word this board may not use; that one drops
	even the cap, so nothing this could answer before can fail to answer now. */
	for(int round = 0; round <= RELAXATION_ROUNDS; round++){
		int takeAnythingPlayable = (round == RELAXATION_ROUNDS);
		struct Band band = loosen(asked, round);

		struct arrayList *aList = init_ArrayList(10, 5, NUM);
		for(int i = 0; i < IntToWord_HashMap->numWords; i++){
			int adj = IntToWord_HashMap->array[i]->numConnections;
			/* The cap is not a preference the way the band is: a board opening
			on a word nobody has heard of is the fault it exists to stop, so it
			is refused every round rather than widened along with the band. */
			if(adj >= band.min && adj <= band.max
				&& (takeAnythingPlayable || !isTooObscureForGraph(i, IntToWord_HashMap))){
				add_ArrayList((void*)(&i), aList, NUM);
			}
		}

		if(aList->currPrecision > 0){
			int id = ((int*)(aList)->list)[rand() % aList->currPrecision];
			free_ArrayList(aList);
			if(round > 0){
				FLWG_LOG("No word had %d-%d neighbours; opened on one with %d-%d instead\n",
					minAdjacencies, maxAdjacencies, band.min, band.max);
			}
			return id;
		}

		free_ArrayList(aList);
	}

	/* Only reachable from an empty dictionary: the last round takes anything. */
	FLWG_LOG("There is no word to open on at all!\n");
	return -1;
}

/*Randomly chooses a word based on an index*/ 
/* A word with exactly this many neighbours, or as near as the dictionary has.
 *
 * A band of one, which is the narrowest request there is and the likeliest to
 * match nothing - so it is asked for as a band and widened the same way. */
int ChooseStart(struct wordDataArray* IntToWord_HashMap, int numAdjacencies){
	return ChooseStart_Range(IntToWord_HashMap, numAdjacencies, numAdjacencies);
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
int goalCheck(char* input, int goal, int isValid, int isCommand, struct DataStructures* data){
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
	

		int inputNum = Convert_WordToInt(input, data); 
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
			FLWG_LOG("\nError!\n");
			break;
		case(1):
			FLWG_LOG("\nCongratulations! You Won! Your Score is: %d%%\n", getScore(gc));
			//Don't forget, when they add a word, add it to the goal hash set
			break; 
		case(2):
			FLWG_LOG("\nBetter Luck Next Time.\n");
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
	FLWG_LOG("Welcome to the four letter word pathway!\n");
	FLWG_LOG("Your goal is to start from the first, and by changing a single letter, turn it into the goal word.\n");
	FLWG_LOG("\n\nThe current goal word is: %s\n\n", goal);
	FLWG_LOG("The commands you have at your disposal are:\n");
	FLWG_LOG(" <Word> --> Adds a word to the list\n");
	FLWG_LOG(" -<Word> --> Removes a word from the list, and all words that follow it\n");
	FLWG_LOG(" u --> Undoes a move\n");
	FLWG_LOG(" r --> Redoes a move\n");
	FLWG_LOG(" finish or q --> Ends the game before the word is found\n");
	FLWG_LOG("Good luck!\n");         
	//FLWG_LOG("\nYour goal is to start off with the start word, and through letter substitution, find your way to teh gaol word!\nThe commands you are allowed are:\n<word>, which adds a word to the list\n-<word> (put a hyphine in front of the word), which removes a word from the list, and all the words after it\nu - which undoes your previous turn.\nq - Which ends the game\nGood Luck, and have a wacky good time!!\n");
	
	
}

void EnterText(){
	FLWG_LOG("Welcome to the Four Letter Word Game!\n");
	FLWG_LOG("Your goal is to start off with one word, and through the four letter word game (single letter substitution), make it into a goal word!!\n");
	FLWG_LOG("The commands you are allowed are:\n");
	FLWG_LOG("<word>  --> which will add a word to the list\n");
	FLWG_LOG("-<word> --> which removes a word from the list\n");
	FLWG_LOG("u --> undoes a move\n");
	FLWG_LOG("r --> redoes a move\n");       
	FLWG_LOG("q or finish --> ends game\n\n");
	FLWG_LOG("If you have any questions, please type h for help\n"); 
	FLWG_LOG("After typing a command, please press enter\n");
}


int round_FLWP(struct GameComponents* gc, struct PathfinderGame *pc, struct DataStructures* data){

	gc->hc->hintPoints = pc->hintPoints; 
	int endCondition;
	char* input;  
	FLWG_LOG("Your goal is to start at %s, and arrive at %s\nYou have %d hint points.\n", Convert_IntToWord(gc->start, data->I2W), Convert_IntToWord(gc->goal, data->I2W), gc->hc->hintPoints); 
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
				input = RemoveWord_Struct(gc, input, 1, data); 
		
			}
			else if(input[0] == '?'){
				input = substr(input, 1, data->I2W->numLetters + 1, 1);
				//Check the length
				int i = 0; 
				char c; 
				while((c = input[i]) != '\0'){
					i++; 
				}
				if(i == data->I2W->numLetters){
				
					int id = Convert_WordToInt(input, data); 
					if(id > pow(26, data->I2W->numLetters) || id <= 0){
						
					}
					int inDic = inDictionary(id); 
					if(inDic == 1){
						FLWG_LOG("That is a valid word\n"); 
					}
					else{
						FLWG_LOG("That is not a valid word\n"); 
					}
				}
				else{
					if(i < data->I2W->numLetters){
						FLWG_LOG("That word is too short\n"); 
					}
					else{
						FLWG_LOG("That word is too long\n"); 
					}
					
				}
			}
			 
		
			//if they want to undo their previous move
			else if(strcmp(input, "u") == 0){
				Undo_Struct(gc, data->I2W);
			}
			// if cmd equals r -- it will be time to redo the previous move*/
		
			else if(strcmp(input, "r") == 0){
				Redo_Struct(gc, data->I2W);
				
			}
			else if(strcmp(input, "g") == 0){
				FLWG_LOG("Your goal word is %s\n", Convert_IntToWord(gc->goal, data->I2W)); 
			}
			//Help command
			else if(strcmp(input, "h") == 0){
				Help(Convert_IntToWord(gc->goal, data->I2W)); 
			}
			else if(strcmp(input, "1") == 0){
				char* output = hint1((uintptr_t)gc); 
				FLWG_LOG("%s\n", output);
				free(output);  
				
			}
			else if(strcmp(input, "2") == 0){
				char* output = hint2((uintptr_t)gc, data->I2W); 
				FLWG_LOG("%s\n", output);
				free(output);  
			}
			else if(strcmp(input, "3") == 0){
				char* output = hint3((uintptr_t)gc, data->I2W, data->wordSet); 
				FLWG_LOG("%s\n", output);
				free(output);  
			}
			else if(strcmp(input, "p") == 0){ 
				FLWG_LOG("Hint Points Available: %d", gc->hc->hintPoints); 
		
		
			}		
			else if(strcmp(input, "q") == 0 || strcmp(input, "finish") == 0){
				isCommand = 2; 
			}
			else{
				isValid = AddWord_Struct(gc, input, data); 
				isCommand = 0; 
			}
			
				
	
			FLWG_LOG("%s", (char*)gc->aList->list); 
			
			FLWG_LOG("\n"); 
		
		
		}
		//Goal Check Returns: 
		//0 - The user hasn't finished
		//1 - The user won
		//2 - The user quit
	}while( (endCondition = goalCheck(input, gc->goal, isValid, isCommand, data)) == 0)  ; 
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

