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
void _FLWG(); 
void _FLWC(); 
int __FLWC();
void _FLWGP();

void _FLWGP(){
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 4); 
	struct GameComponentsFLWGP* flwgp = initiateFLWGP(10, 'e', 3, ' ', 0, data); 
	int isWon = isGameWonFLWC(flwgp->flwcComponents); 
	userEntersWordFLWC("good", flwgp->flwcComponents, data); 
	 isWon = isGameWonFLWC(flwgp->flwcComponents); 
	userEntersWordFLWC("gold", flwgp->flwcComponents, data); 
	 isWon = isGameWonFLWC(flwgp->flwcComponents); 
	userEntersWordFLWC("hold", flwgp->flwcComponents, data); 
	 isWon = isGameWonFLWC(flwgp->flwcComponents); 
	userEntersWordFLWC("hole", flwgp->flwcComponents, data); 
	freeGameComponentsFLWGP(flwgp, data); 



}

int main(){

	_FLWGP();
	return 0; 
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
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* dataStructures = initDataStructures(fd, numLetters); 
	int numAdjacenciesToStartWord = 16;
	int botType = -1;
	char goalCharacter = 'e';
	int minGoalCharacterDistance = 1;
	char avoidCharacter = ' ';
	int minAvoidCharacterDistance = 0; 
	struct GameComponentsFLWC* flwcComponents = initFLWC(numAdjacenciesToStartWord, goalCharacter, minGoalCharacterDistance, avoidCharacter, minAvoidCharacterDistance, dataStructures);
	printf("Get to a word with 'e' in it\n");
	printf("Start Word: %s\n", getStartWordFLWC(flwcComponents, dataStructures));
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
void _FLWC(){

	/*
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
	*/
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
	/*
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
	*/	

}
