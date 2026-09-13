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
#include "./flwp/includes/BreadthFirstSearch_FLWP.h"
#include "./flwp/includes/GameFunctions.h"
#include "./flwp/includes/UserInput.h"

#include "./flwg/includes/FLWGGame.h"
#include "./flwg/includes/Hints2.h"
#include "./flwc/includes/FLWC.h"
#include "./flwg/includes/MultiplayerFLWG.h"


int fix_flwgp_redo_crash(); 
void _FLWP(); 
// FLWG
void flwg_example(); 

//FLWC
int __FLWC();

void _FLWGP();


// flwc skeletal code, no game logic
void flwc();
void flwic();
int flwg();
void flwp();
int flwgp(); 
void flwt(); 

int level21();
int flwg3p();
/*The menu in debug_console.c: pick a mode, name the word it starts on, and
play it. The functions above are the older way round - one mode each, with its
parameters written into the source - and they still double as the usage
documentation for the APIs. To run one of those instead, call it here.*/
void debugConsole();

int main(){
	srand(time(0));
	debugConsole();
}


int fix_flwgp_redo_crash(){
		// Initialize Structures
	printf("-----FLWGP----\n"); 
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 

	// FLWGP Parameters
	int minAdjacenciesToStart = 1; 
	int maxAdjacenciesToStart = 30; 

	int minGoalDistance = 2; 
	int maxGoalDistance = 2; 

	int minAvoidDistance = 0; 
	int maxAvoidDistance = 0; 

	int minGoalAdjacencies = 1; 
	int maxGoalAdjacencies = 30; 

	// These are temporary values that are replaced later
	char *goalWords[] = {"ties", "pies", "lies", NULL};
	char *avoidWords[] = {NULL}; 

	// Initialize the Game
	struct GameComponentsFLWGP* flwgpComponents = initiateFLWGP(minAdjacenciesToStart, maxAdjacenciesToStart,  goalWords, avoidWords, minGoalDistance, minAvoidDistance, maxGoalDistance, maxAvoidDistance, minGoalAdjacencies, maxGoalAdjacencies, data);
	if(!isStartValid_FLWGP(flwgpComponents)){
		freeGameComponentsFLWGP(flwgpComponents, data);
		close(fd);
		freeDataStructures(data);
		return -1;
	}
	printf("[GAME MESSAGE]\nUNDO PRESSED"); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	undoMoveFLWGP(flwgpComponents, data); 
	printf("[GAME MESSAGE]\nREDO PRESSED"); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	redoMoveFLWGP(flwgpComponents, data); 
	
	printf("[GAME MESSAGE]\nStart: %s\n", getStartWordFLWP(flwgpComponents->flwpComponents, data));
	printf("[HINT MESSAGE]\nThere are %d connections at minimum\n", hintGetMinAdjacenciesFLWGP(flwgpComponents)); 
	printf("[HINT MESSAGE]\nA direct adjacency towards teh goal is %s\n", hintWordTowardsGoalFLWGP(flwgpComponents, data)); 
	printf("[HINT MESSAGE]\nA valid goal word is %s\n", hintGetValidGoalWordFLWGP(flwgpComponents, data)); 
	//printf("[GAME MESSAGE]\n Solution:\n%s\n", getSolutionFLWGP(flwgpComponents)); 
	freeGameComponentsFLWGP(flwgpComponents, data); 
	close(fd); 
	freeDataStructures(data);	

	return 0; 	


}



int flwgp(){
	// Initialize Structures
	printf("-----FLWGP----\n"); 
	srand(time(0)); 
	int numLetters = 3; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 

	// FLWGP Parameters
	int minAdjacenciesToStart = 1; 
	int maxAdjacenciesToStart = 30; 

	int minGoalDistance = 2; 
	int maxGoalDistance = 2; 

	int minAvoidDistance = 0; 
	int maxAvoidDistance = 0; 

	int minGoalAdjacencies = 1; 
	int maxGoalAdjacencies = 30; 

	// These are temporary values that are replaced later
	char *goalWords[] = {"ties", "pies", "lies", NULL};
	char *avoidWords[] = {NULL}; 

	// Initialize the Game
	struct GameComponentsFLWGP* flwgpComponents = initiateFLWGP(minAdjacenciesToStart, maxAdjacenciesToStart,  goalWords, avoidWords, minGoalDistance, minAvoidDistance, maxGoalDistance, maxAvoidDistance, minGoalAdjacencies, maxGoalAdjacencies, data);
	if(!isStartValid_FLWGP(flwgpComponents)){
		freeGameComponentsFLWGP(flwgpComponents, data);
		close(fd);
		freeDataStructures(data);
		return -1;
	}
	printf("[GAME MESSAGE]\nStart: %s\n", getStartWordFLWP(flwgpComponents->flwpComponents, data));
	printf("[HINT MESSAGE]\nThere are %d connections at minimum\n", hintGetMinAdjacenciesFLWGP(flwgpComponents));
	printf("[HINT MESSAGE]\nA direct adjacency towards teh goal is %s\n", hintWordTowardsGoalFLWGP(flwgpComponents, data));
	printf("[HINT MESSAGE]\nA valid goal word is %s\n", hintGetValidGoalWordFLWGP(flwgpComponents, data));
	//printf("[GAME MESSAGE]\n Solution:\n%s\n", getSolutionFLWGP(flwgpComponents));
	freeGameComponentsFLWGP(flwgpComponents, data);
	close(fd);
	freeDataStructures(data);
	return 0;
}

void flwt(){
	// Initialize Structures
	printf("-----FLWT----\n"); 
	srand(time(0)); 
	int numLetters = 3; 

	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 

	// FLWT Parameters
	int numTurns = 3; 
	int minAdj = 10; 
	int maxAdj = 30; 

	// Initialize the Game
	struct GameComponentsFLWT* flwt = initFLWT(numTurns, minAdj,  maxAdj, data); 
	flwt->startWordId = Convert_WordToInt("boo", data);
	printf("[GAME MESSAGE] Start: %s\n", getStartWordFLWT(flwt, data));
	printf("[GAME MESSAGE] Hint Character: %c\n", hint_letterToConsiderFLWT(flwt, data));
	printf("[GAME MESSAGE] Hint Num Options: %d\n", hint_numOptionsFLWT(flwt, data));
	char input[100];
 	printf("Enter a string: ");
	fgets(input, sizeof(input), stdin);
	// Remove trailing newline from fgets
	input[strcspn(input, "\n")] = 0;
	int result = userEntersWordFLWT(input, flwt, data);	
	printf("RESULT: %d", result);
	freeGameComponentsFLWT(flwt); 
	close(fd); 
	freeDataStructures(data);	
	
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
		freeGameComponentsFLWG(gameData);
		close(fd);
		freeDataStructures(dataStructures);
		return 0;
	}
	else{
		printf("[GAME MESSAGE] Start: %s\n", getCurrWord(gameData, dataStructures));
		printf("[HINT MESSAGE] Hint Character: %c\n", hintLetterToConsiderFLWG(gameData, dataStructures));
		printf("[HINT MESSAGE] Hint Num Options: %d\n", hintNumOptionsFLWG(gameData, dataStructures));


		/******GAME LOGIC GOES HERE**********/

		// End the Game
		freeGameComponentsFLWG(gameData); 
	}
	close(fd); 
	freeDataStructures(dataStructures);	
	return 0; 
}


void flwp(){
	// Set the Game Data
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 

	// Set the FLWP Parameters
	int minAdjToStart = 4; 
	int maxAdjToStart = 16; 
	int minDistance = 4; 
	int maxDistance = 8; 
	int minAdjToGoal = 4; 
	int maxAdjToGoal = 16; 
	struct GameComponents* gc = initiateFLWP(minAdjToStart, maxAdjToStart, minDistance, maxDistance, minAdjToGoal, maxAdjToGoal, data); 
	printf("[GAME MESSAGE]: Start: %s\n", Convert_IntToWord(gc->start, data->I2W)); 
	printf("[GAME MESSAGE]: Goal: %s\n", Convert_IntToWord(gc->goal, data->I2W)); 
	printf("[HINT MESSAGE]: A direct adjacency to the start word is: %s\n", hintGetHeadAdjacencyFLWP(gc, data));
	printf("[HINT MESSAGE]: A direct adjacency to the goal word is: %s\n", hintGetTailAdjacencyFLWP(gc, data));
	printf("[HINT MESSAGE]: The minimum distance between the start & goal is: %d\n", hintGetMinAdjacenciesFLWP(gc, data));
	printf("[GAME MESSAGE]: Solution: \n");
	PrintStrings_IntLL(gc->solution, data->I2W); 

	freeGameComponentsFLWP(gc, data); 
	close(fd); 
	freeDataStructures(data);	
	
}



void flwc(){
	printf("-----FLWC----\n"); 
	// Initialize Structures
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 

	// FLWC Parameters
 
	int minAdjacenciesToStart = 1; 
	int maxAdjacenciesToStart = 30; 
	int minGoalDistance = 2; 
	int maxGoalDistance = 6; 
	int minAvoidDistance = 0; 
	int maxAvoidDistance = 0; 
	int minGoalAdjacencies = 1; 
	int maxGoalAdjacencies = 30; 
	int numTurns = 8; 
	int botType = 1;
	char *goalWords[] = {"lack", "lock", "pack", "tack", "back", "suck", "hack", "hock", "pock", "rack", "sack", "sock", "suck", "buck", "dock", "duck", "lick", "luck", "muck", "peck", "sick", "mock", "pick", "rock", "tuck", "jack", "jock", "tick", "beck", "deck", "heck", "kick", "wick", "yuck", "neck", NULL};
	char *avoidWords[] = {NULL}; 
	// Initialize the Game
	struct GameComponentsFLWC* flwcComponents = initFLWC(
		minAdjacenciesToStart, 
		maxAdjacenciesToStart,  
		goalWords, 
		avoidWords, 
		minGoalDistance, 
		minAvoidDistance, 
		maxGoalDistance, 
		maxAvoidDistance, 
		minGoalAdjacencies, 
		maxGoalAdjacencies,
		numTurns,
		data);
		
	flwcComponents->wordId = 1336;
	char* startWord = getStartWordFLWC(flwcComponents, data); 
	int aireId = Convert_WordToInt("aire", data); 
	
	
		
	printf("[GAME MESSAGE]: Start: %s\n", startWord);
	
	//int id = botPly_MaxAdjacencies(flwcComponents->wordId, flwcComponents->goalWords, data); 
	printf("[GAME MESSAGE]: A Potential Goal Is %s", hintGoalWordFLWC(flwcComponents, data));
	char* pathToGoal = hintPathToGoalFLWC(flwcComponents, data);
	printf("[GAME MESSAGE]: A Way To Get to the Goal Is: %s", pathToGoal);
	free(pathToGoal); 
	printf("[GAME MESSAGE]: A Word That Gets You Closer to the Goal Is: %s", hintAdjacencyTowardsGoalFLWC(flwcComponents, data));
	printf("[GAME MESSAGE]: You are %d words away from the goal", hintMinDistanceToGoalFLWC(flwcComponents, data));
	
	// End the Game
	freeGameComponentsFLWC(flwcComponents); 
	close(fd); 
	freeDataStructures(data);	
}
void flwic(){
	printf("-----Inverse FLWC----\n"); 
	// Initialize Structures
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 

	// FLWC Parameters
 
	int minAdjacenciesToStart = 1; 
	int maxAdjacenciesToStart = 30; 
	int minGoalDistance = 0; 
	int maxGoalDistance = 0; 
	int minAvoidDistance = 1; 
	int maxAvoidDistance = 3; 
	int minGoalAdjacencies = 1; 
	int numTurns = 15; 
	int maxGoalAdjacencies = 30; 
	int botType = 1;
	char *goalWords[] = {NULL};
	char *avoidWords[] = {"ties", "pies", "lies", NULL}; 
	// Initialize the Game
	struct GameComponentsFLWC* flwcComponents = initFLWC(
		minAdjacenciesToStart, 
		maxAdjacenciesToStart,  
		goalWords, 
		avoidWords, 
		minGoalDistance, 
		minAvoidDistance, 
		maxGoalDistance, 
		maxAvoidDistance, 
		minGoalAdjacencies, 
		maxGoalAdjacencies,
		numTurns,
		 data);
		
	printf("[GAME MESSAGE]\nStart: %s\n", getStartWordFLWC(flwcComponents, data));
	printf("[HINT MESSAGE]\nTo Stay Away From The Avoid Word, You Should Use %s\n", hintBestDirectAdjacencyFLWIC(flwcComponents, data)); 
	printf("[HINT MESSAGE]\nThe Closest Avoid Word is %d away\n", hintDistanceFromNearestAvoidWordFLWIC(flwcComponents, data));
	// End the Game
	freeGameComponentsFLWC(flwcComponents); 
	close(fd); 
	freeDataStructures(data);	
}






/*
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

	int fd = open("docs/small.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, 2); 
	int word = 0; 
	word = botPly(word, 6, data->I2W, data->wordSet); 
	printf("Choice: %d", word);
}

	*/
void _FLWP(){

	/*
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

int level21(){

	printf("-----LEVEL 21-----\n"); 
	// Initialize Structures
	srand(time(0)); 
	int numLetters = 4; 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd, numLetters); 
	// FLWC Parameters
 
	int minAdjacenciesToStart = 2; 
	int maxAdjacenciesToStart = 30; 
	int minGoalDistance = 2; 
	int maxGoalDistance = 3; 
	int minAvoidDistance = 0; 
	int maxAvoidDistance = 0; 
	int minGoalAdjacencies = 12; 
	int maxGoalAdjacencies = 30; 
	int numTurns = 8; 
	int botType = 1;

	char *goalWords[] = {"boos", "boot", "coos", "loot", "soot", "hook", "hoot", "loon", "moos", "moot", "root", "cook", "foot", "look", "toot", "book", "boon", "fool", "hood", "moon", "poop", "rook", "woos", "cool", "coop", "food", "pool", "boom", "good", "goon", "hoof", "loom", "mood", "noon", "poof", "room", "took", "tool", "wood", "woof", "goof", "hoop", "loop", "nook", "roof", "soon", "wool", "oops", "poor", "doom", "pooh", "zoom", "zoos", "door", "shoo", "ooze", "oozy", NULL}; 
	char *avoidWords[] = {NULL}; 
	// Initialize the Game
	struct GameComponentsFLWC* flwcComponents = initFLWC(
		minAdjacenciesToStart, 
		maxAdjacenciesToStart,  
		goalWords, 
		avoidWords, 
		minGoalDistance, 
		minAvoidDistance, 
		maxGoalDistance, 
		maxAvoidDistance, 
		minGoalAdjacencies, 
		maxGoalAdjacencies,
		numTurns,
		data);
		
	char* startWord = getStartWordFLWC(flwcComponents, data); 
	printf("[GAME MESSAGE]: Start: %s\n", startWord);
	printf("[GAME MESSAGE]: A Potential Goal Is %s", hintGoalWordFLWC(flwcComponents, data));
	char* pathToGoal = hintPathToGoalFLWC(flwcComponents, data);
	printf("[GAME MESSAGE]: A Way To Get to the Goal Is: %s", pathToGoal);
	free(pathToGoal); 
	printf("[GAME MESSAGE]: A Word That Gets You Closer to the Goal Is: %s", hintAdjacencyTowardsGoalFLWC(flwcComponents, data));
	printf("[GAME MESSAGE]: You are %d words away from the goal", hintMinDistanceToGoalFLWC(flwcComponents, data));
	
	// End the Game
	freeGameComponentsFLWC(flwcComponents);
	close(fd);
	freeDataStructures(data);
	return 0;
}

/*----------------------------------------------------------------------------
Three player FLWG, played out move by move so it can be watched: the multiplayer
MCTS against a bot that plays at random and a bot that always moves to the word
with the fewest adjacencies left. Ordinary FLWG rules, so the player who runs out
of moves loses and the other two win.
----------------------------------------------------------------------------*/
static const char* PLAYER_NAMES[] = {"MCTS  ", "RANDOM", "MINADJ"};

/*How many of a word's adjacencies nobody has played yet*/
static int demo_optionsLeft(int id, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	int available = 0;
	while(adjacency != NULL){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			available++;
		}
		adjacency = adjacency->next;
	}
	return available;
}

/*The opposite of botPly_MaxAdjacencies: leave the next player the least room*/
static int demo_chooseMinAdjacencies(int id, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	int best = -1;
	int fewest = 0;
	while(adjacency != NULL){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			int available = demo_optionsLeft(adjacency->data, data);
			if(best == -1 || available < fewest){
				best = adjacency->data;
				fewest = available;
			}
		}
		adjacency = adjacency->next;
	}
	return best;
}

int flwg3p(){
	int numLetters = 3;
	int fd = open("docs/3.txt", O_RDONLY);
	struct DataStructures* data;
	int word;
	int seat = 0;
	int ply = 0;
	int i;

	if(fd == -1){
		printf("[GAME MESSAGE]\nCould not open docs/3.txt -- run this from the repository root\n");
		return -1;
	}
	data = initDataStructures(fd, numLetters);
	close(fd);

	/*Any word with somewhere to go*/
	do{
		word = rand() % data->I2W->numWords;
	}while(getNumAdjacencies(word, data) == 0);
	markUsed_WordSet(word, data->wordSet);

	printf("\n===============================================\n");
	printf(" THREE PLAYER FLWG -- MCTS vs RANDOM vs MINADJ\n");
	printf("===============================================\n");
	printf("Change one letter each turn, no word twice.\n");
	printf("The player with no move left loses; the other two win.\n\n");
	printf("[GAME MESSAGE]\nStart word: %s (%d options)\n\n",
		Convert_IntToWord(word, data->I2W), demo_optionsLeft(word, data));
	fflush(stdout);

	for(;;){
		int move;
		if(seat == 0){
			move = montyCarlosTreeSearch_Multiplayer(word, 3, data->wordSet, data->I2W);
		}
		else if(seat == 1){
			move = chooseRandom(word, data->I2W, data->wordSet);
		}
		else{
			move = demo_chooseMinAdjacencies(word, data);
		}

		if(move == -1){
			printf("\n[GAME MESSAGE]\n%s is stuck at '%s' after %d moves -- %s LOSES.\n",
				PLAYER_NAMES[seat], Convert_IntToWord(word, data->I2W), ply, PLAYER_NAMES[seat]);
			printf("[GAME MESSAGE]\nWinners: ");
			for(i = 0; i < 3; i++){
				if(i != seat){
					printf("%s%s", PLAYER_NAMES[i], (i < 2 && (i + 1) != seat) ? ", " : "");
				}
			}
			printf("\n\n");
			fflush(stdout);
			break;
		}

		markUsed_WordSet(move, data->wordSet);
		ply++;
		printf("  %2d. %s plays %s   (%d options left)\n", ply, PLAYER_NAMES[seat],
			Convert_IntToWord(move, data->I2W), demo_optionsLeft(move, data));
		fflush(stdout);
		/*Slow enough to follow along with*/
		usleep(350000);

		word = move;
		seat = (seat + 1) % 3;
	}

	freeDataStructures(data);
	return 0;
}
