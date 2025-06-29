/**********************************
Name: Jordan D. 
Date: Feb 2, 2020
Purpose: The four letter pathfinder, this time better 
**********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "./api/includes/FLWG-API.h"
#include "./api/includes/FLWT-API.h"
#include "./api/includes/FLWC-API.h"

#include "./algs/includes/Minimax-2.h"
#include "./algs/includes/MinimaxTests.h"
#include "./algs/includes/BreadthFirstSearch.h"
#include "./algs/includes/MaxN.h"
#include "./algs/includes/MontyCarlosTreeSearch.h"

#include "./structs/includes/HashMap.h"
#include "./structs/includes/TreeSet.h"
#include "./structs/includes/WordSet.h"
#include "./structs/includes/IntLinkedList.h"
#include "./structs/includes/Queue.h"


#include "./flwp/includes/PathfinderGame.h"
#include "./flwp/includes/GameFunctions.h"
#include "./flwp/includes/UserInput.h"

#include "./flwg/includes/FLWGGame.h"
#include "./flwg/includes/Hints2.h"
#include "./flwc/includes/FLWC.h"
#include "./flwg/includes/MultiplayerFLWG.h"

void _FLWT(); 
void _FLWP(); 
// FLWG
void flwg_example(); 

//FLWC
void _FLWC(); 
void flwc_validparameters(); 
void flwc_invalidparameters(); 
int __FLWC();

void _FLWGP();


// flwc skeletal code, no game logic
int flwc();
int flwg();

int main(){
	return flwg();   
}

int flwg(){

	// Initialize Structures
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 

	// FLWG Parameters
	int minAdjacencies = 4; 
	int maxAdjacencies = 4; 

	// Initialize the Game
	struct GameData* gameData = initFLWG(dataStructures, minAdjacencies, maxAdjacencies);
	if (isStartValidFLWG(gameData) == 0){
		printf("Invalid FLWG Parameters");
		return -1; 

	}
	printf("Start Word: %s\n", getCurrWord(gameData, dataStructures));


	/******GAME LOGIC GOES HERE**********/

	// End the Game
	freeGameComponentsFLWG(gameData); 
	close(fd); 
	freeDataStructures(dataStructures);	
	return 0; 
}



int flwc(){
	// Initialize Structures
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 

	// FLWC Parameters
	int numAdjacenciesToStartWord = 4; 
	int botType = -1;
	char goalCharacter = 'e';
	int minGoalCharacterDistance = 1;
	char avoidCharacter = ' ';
	int minAvoidCharacterDistance = 0;

	// Initialize the Game
	struct GameComponentsFLWC* flwcComponents = initFLWC(numAdjacenciesToStartWord, goalCharacter, minGoalCharacterDistance, avoidCharacter, minAvoidCharacterDistance, dataStructures);
	if (!isStartValidFLWC(flwcComponents) == 0){
		printf("First Failed"); 
		return -1; 

	}
	printf("First Went Through Well!");
	flwcComponents = initFLWC(16, goalCharacter, 15, avoidCharacter, minAvoidCharacterDistance, dataStructures);
	if (!isStartValidFLWC(flwcComponents) == 0){
		printf("Second Failed"); 
		return -1; 
	}
	printf("Start Word: %s\n", getStartWordFLWC(flwcComponents, dataStructures));


	/******GAME LOGIC GOES HERE**********/

	// End the Game
	freeGameComponentsFLWC(flwcComponents); 
	close(fd); 
	freeDataStructures(dataStructures);	
	return 0; 
}


void _FLWGP(){
	// initialize flwg ds
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 4); 
	
	// Initialize FLWGP 
	struct GameComponentsFLWGP* flwgp = initiateFLWGP(10, 'e', 3, ' ', 0, data); 

	// Check if the game is won and enter words 
	int isWon = isGameWonFLWC(flwgp->flwcComponents); 

	 userEntersWord_FLWGP("root", flwgp, data); 
	 printf("%s\n", Convert_IntToWord(flwgp->flwcComponents->wordId, data->I2W)); 

	 userEntersWord_FLWGP("boot", flwgp, data); 
	 printf("%s\n", Convert_IntToWord(flwgp->flwcComponents->wordId, data->I2W)); 

	 userEntersWord_FLWGP("loot", flwgp, data); 
	 printf("%s\n", Convert_IntToWord(flwgp->flwcComponents->wordId, data->I2W)); 

	 undoMoveFLWGP(flwgp, data); 
	 printf("%s\n", Convert_IntToWord(flwgp->flwcComponents->wordId, data->I2W)); 

	 userEntersWord_FLWGP("bolt", flwgp,  data); 
	 printf("%s\n", Convert_IntToWord(flwgp->flwcComponents->wordId, data->I2W)); 

	 userEntersWord_FLWGP("belt", flwgp, data); 
	 printf("%s\n", Convert_IntToWord(flwgp->flwcComponents->wordId, data->I2W)); 
	
	 isWon = isGameWonFLWC(flwgp->flwcComponents); 
	 printf("Is Won: %d", isWon); 

	freeGameComponentsFLWGP(flwgp, data); 



}


void trapTest(){

	//_FLWP(); 
	int fd = open("docs/playerWin.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 4); 
	printf("Is Trapped: %d\n", isTrapped(0, data)); 
	markUsed_WordSet(1, data->wordSet);	
	printf("Is Trapped: %d", isTrapped(0, data)); 
}


void _FLWT(){
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 
	struct GameComponentsFLWT* flwt = initFLWT(20, 20, data); 
	char* start = getStartWordFLWT(flwt, data); 
	printf("Start Word: %s", start);
	int hint = directAdjacencyHint(flwt->startWordId, data); 
	printf("Yo Snazzy Hint: %d", hint);




}

int __FLWC(){
	// Initialize Game
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 
	int numAdjacenciesToStartWord = 16;

	// Determine the Bot Type 
	int botType = -1;

	// Establish the goal character
	char goalCharacter = 'e';
	int minGoalCharacterDistance = 5;
	char avoidCharacter = ' ';
	int minAvoidCharacterDistance = 0; 
	struct GameComponentsFLWC* flwcComponents = initFLWC(numAdjacenciesToStartWord, goalCharacter, minGoalCharacterDistance, avoidCharacter, minAvoidCharacterDistance, dataStructures);
	// If the start is not valid, end the function
	if (!isStartValidFLWC(flwcComponents) == 0){
		printf("No valid combination"); 
		return -1; 

	}
	printf("Get to a word with 'e' in it\n");
	printf("Start Word: %s\n", getStartWordFLWC(flwcComponents, dataStructures));
	/* 
	int turn = 0; 
	while(isGameWonFLWC(flwcComponents) == -1){
		if(turn % 2 == 0){
			flwcComponents->wordId = userPly(flwcComponents->wordId, dataStructures); 
		}
		else{
			botTakesTurnFLWC(6, flwcComponents, dataStructures);
		}
		printf("%s: %s\n", (turn % 2 == 0) ? "Johnny (dumber):" : "Jimmy (smarter):", (flwcComponents->wordId != -1) ?  Convert_IntToWord(flwcComponents->wordId, dataStructures->I2W) : "I lose :(");

		turn++; 
	}	
	printf("\n\nBest  Solution:\n%s\n", getSolutionFLWC(flwcComponents));
	int result = isGameWonFLWC(flwcComponents); 
	printf("Result: %d\n", result);
	switch(result){
		case -1: 
			printf("Error Status");
			break; 
		case 0: 
			printf("Game is tied");
			break; 
		case 1:
			printf("Word in goal set reached");
			break; 
		case 2: 
			printf("Word in avoid set reached");
			break; 
	}
	freeGameComponentsFLWC(flwcComponents); 

	close(fd); 
	freeDataStructures(dataStructures);	
	*/ 
	return 0; 

	/*
	int fd = open("docs/small.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 2); 
	int word = 0; 
	word = botPly(word, 6, data->I2W, data->wordSet); 
	printf("Choice: %d", word);
	*/
}

void _FLWP(){

	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	int numAdjacenciesToStartWord = 16;
	char goalCharacter = 'e';
	int minGoalCharacterDistance = 1;
	char avoidCharacter = ' ';
	int minAvoidCharacterDistance = 0; 
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 

	struct GameComponentsFLWGP* flwgp = initiateFLWGP(numAdjacenciesToStartWord, goalCharacter, minGoalCharacterDistance, avoidCharacter, minAvoidCharacterDistance, dataStructures);
	printf("Get to a word with 'e' in it\n");
	printf("Start Word: %s\n", getStartWordFLWP(flwgp->flwpComponents, dataStructures));
	int turn = 0; 
	while(isGameWonFLWC(flwgp->flwcComponents) == -1){
		struct arrayList* aList = getCurrentWordsFLWP(flwgp->flwpComponents); 
		print_ArrayList(aList, STR); 	
		char str[BUFSIZ]; 
		fgets(str, BUFSIZ, stdin);
		// Remove trailing newline if present
		str[strcspn(str, "\n")] = 0;
		int errorId = userEntersWord_FLWGP(str, flwgp, dataStructures); 
		printf("Error Id: %d\n", errorId); 
		int result = isGameWonFLWC(flwgp->flwcComponents);
		printf("Result: %d\n", result);
	}	
	int result = isGameWonFLWC(flwgp->flwcComponents);
	printf("Result: %d\n", result);
	switch(result){
		case -1: 
			printf("Error Status");
			break; 
		case 0: 
			printf("Game is tied");
			break; 
		case 1:
			printf("Word in goal set reached");
			break; 
		case 2: 
			printf("Word in avoid set reached");
			break; 
	}
	freeGameComponentsFLWGP(flwgp, dataStructures); 

	close(fd); 
	freeDataStructures(dataStructures);	

}

void flwcChooseGoals_Example(){


	// Initialize the Data Structures
	srand(time(0)); 
	int numLetters = 3; 
	int fd = open("docs/3.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 4); 

	// Initialize the starting word
	int startId = 0; 

	// Initialize the goal words
	int* goalWordSet = NULL;   

	while(goalWordSet == NULL){
	//	goalWordSet = getGoalWordSet(startId, distanceFromGoalWord, data); 
	}
	
	printf("\nStarting Word: %s\n", Convert_IntToWord(startId, data->I2W)); 
	char* w0 = Convert_IntToWord(goalWordSet[0], data->I2W); 
	char* w1 = Convert_IntToWord(goalWordSet[1], data->I2W); 
	printf("Opponent A: %s\n", w0);  
	printf("Opponent B: %s\n", w1);  
	free(goalWordSet); 

	// While game is not over
		// Allow the bot to take a turn 
		// Allow the user to take a turn 
	
	// Declare the winner 
	freeDataStructures(data); 
}

void flwg_example(){
	srand(time(0)); 
	int numLetters = 3; 
	int fd = open("docs/3.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 
	printf("Num Letters: %d", data->I2W->numLetters);
        int num_games = 100;
        int bot_wins = 0;
        int random_wins = 0;
	for (int i = 0; i < num_games; i++){
                if (i % 100 == 0) {
                  printf("%d/%d games complete\n", i, num_games);
                }
		int word = ChooseStart(data->I2W, 12); //12 is an arbitrarily chosen # of direct adjacencies
                //int word = 0;
		markUsed_WordSet(word, data->wordSet); 
		int winner = -1; 
		int rounds = 0; 
		int whoseTurn = 1; 

		while (winner == -1){
                        assert(word != -1);
			
			if(whoseTurn == 1){
				word = botPly_Random(word, data);
			}
			else{
				word = botPly(word, 6, data->I2W, data->wordSet); 
			}
			//printf("%s: %s\n", (whoseTurn == 1) ?  "Random" : "Bot",  (word == -1) ? "Loss" : Convert_IntToWord(word, data->I2W));
			whoseTurn = (whoseTurn + 1) % 2; 
			if(word == -1){
				winner = whoseTurn; 
			}
			rounds++; 
		}
                if (winner == 0) {
                  random_wins++;
                } else {
                  bot_wins++;
                }
		reset_WordSet(data->wordSet); 
	}

	//printf("%s Wins\n\n", (winner != 0) ? "Minimax": "Random");
        printf("User wins: %d Bot wins: %d\n", bot_wins, random_wins);

}
