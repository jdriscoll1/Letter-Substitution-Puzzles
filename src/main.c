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

#include "./flwg/includes/FLWGGame.h"
#include "./flwg/includes/Hints2.h"
#include "./flwc/includes/FLWC.h"
#include "./flwg/includes/MultiplayerFLWG.h"

void _FLWT(); 
void _FLWP(); 
void _FLWG(); 
void _FLWC(); 


int main(){
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 
	//struct GameComponentsFLWC* flwcComponents = initFLWC(0, dataStructures);	
	//printf("Start Word: %s\n", getStartWordFLWC(flwcComponents, dataStructures));
	int turn = 0; 
	markUsed_WordSet(Convert_WordToInt("blat", dataStructures), dataStructures->wordSet); 
	markUsed_WordSet(Convert_WordToInt("blab", dataStructures), dataStructures->wordSet); 
	for(int i = 0; i < 10; i++){
		printf("%d\n", directAdjacencyHint(Convert_WordToInt("blah", dataStructures), dataStructures)); 
	}
	//freeGameComponentsFLWC(flwcComponents); 
	close(fd); 
	freeDataStructures(dataStructures);	


	/*
	int fd = open("docs/small.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 2); 
	int word = 0; 
	word = botPly(word, 6, data->I2W, data->wordSet); 
	printf("Choice: %d", word);
	*/
}

void _FLWC(){

	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 
	struct GameComponentsFLWC* flwcComponents = initFLWC(0, dataStructures);	
	printf("Get to a word with 'e' in it\n");
	printf("Do not touch any words with 't' in it\n"); 
	printf("Start Word: %s\n", getStartWordFLWC(flwcComponents, dataStructures));
	int turn = 0; 
	while(isGameWonFLWC(flwcComponents) == -1){
		 	
		if(turn % 2 == 0){
			flwcComponents->wordId = userPly(flwcComponents->wordId, dataStructures); 
		
		}
	
		else{
			botTakesTurnFLWC(flwcComponents, dataStructures);
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

void _FLWG(){
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
		int word = ChooseStart(data->I2W); 
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
