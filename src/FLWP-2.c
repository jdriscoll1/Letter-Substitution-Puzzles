/**********************************
Name: Jordan D. 
Date: Feb 2, 2020
Purpose: The four letter pathfinder, this time better 
**********************************/
int numLetters = 4; 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "./FLWG-API.h"
#include "./Tutorial.h"

#include "./algs/includes/Minimax-2.h"
#include "./algs/includes/MinimaxTests.h"
#include "./algs/includes/BreadthFirstSearch.h"
#include "./algs/includes/MaxN.h"
#include "./algs/includes/MontyCarlosTreeSearch.h"

#include "./structs/includes/HashMap.h"
#include "./structs/includes/TreeSet.h"
#include "./structs/includes/WordSet.h"
#include "./structs/includes/IntLinkedList.h"


#include "./flwp/includes/PathfinderGame.h"
#include "./flwp/includes/GameFunctions.h"

#include "./flwg/includes/FLWGGame.h"
#include "./flwc/includes/FLWC.h"
#include "./flwg/includes/MultiplayerFLWG.h"

void testAlpaBetaPruning(struct wordDataArray *IntToWord_HashMap); 
void runMultiplayerFLWG();
void PathfindBetweenTwoWords(char* start, char* end, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet);
void runFLWG();
void testMCTS();
void initializeStructures();
void fourletterwordgame_example(); 
void test_new_flwg(); 

void flwcChooseGoals_Example(); 

int flwt_example();
void FLWC_Example(); 
int main(){
	flwt_example(); 
}
int flwt_example(){

	srand(time(0)); 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd); 

	
	struct GameComponentsFLWT* flwtComponents =  initFLWT(5, 5, data);
	printf("%s", getStartWordFLWT(flwtComponents, data));
	int result = userEntersWordFLWT("fare", flwtComponents, data); 
	printf("Result: %d", result);
	result = userEntersWordFLWT("fare", flwtComponents, data); 
	printf("Result: %d", result);
	


}
void FLWC_Example(){

	// Initialize the Data Structures
	srand(time(0)); 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd); 
	int startId = 0; 
	int distance = 2; 

	FLWC_Test(data); 
	freeDataStructures(data); 
}
void flwcChooseGoals_Example(){


	// Initialize the Data Structures
	srand(time(0)); 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd); 

	// Initialize the starting word
	int startId = 0; 

	int distanceFromGoalWord = 2; 
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

void test_new_flwg(){
	srand(time(0)); 
	int fd = open("docs/4.txt", O_RDONLY);
	struct DataStructures* data = initDataStructures(fd); 
        int num_games = 800;
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
		int depth = 8; 
		int rounds = 0; 
		int whoseTurn = 1; 

		while (winner == -1){
                        assert(word != -1);
			
			if(whoseTurn){
				word = botPly_FLWC(word, depth, data); 
				//printf("Minimax Chooses: %s\n", (word == -1) ? "LOSE CONDITION" : Convert_IntToWord(word, data->I2W));
			}
			else{
				word = botPly_Random(word, data); 
				//printf("Random Chooses: %s\n", (word == -1) ? "LOSE CONDITION" : Convert_IntToWord(word, data->I2W));
			}
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
        printf("Bot wins: %d Random wins: %d\n", bot_wins, random_wins);
	
}

/*
void fourletterwordgame_example(){

	srand(time(0));

	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	//char* path = "../docs/t/mini.txt"; 
	char* path = "./docs/4a.txt";
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet* wordSet = init_WordSet(5361);
	//FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet); 
	//FLWG_Test(IntToWord_HashMap, wordSet);
	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	

	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//FLWP("dill", "toss", WordToInt_HashMap, IntToWord_HashMap, wordSet);
	
	free_WordSet(wordSet); 
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
}



void PathfindBetweenTwoWords(char* start, char* end, struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet){

	int i1 = Convert_WordToInt(start, WordToInt_HashMap);
	int i2 = Convert_WordToInt(end, WordToInt_HashMap);
	struct intList* list = BreadthFirstSearch_Goal(i1, i2, IntToWord_HashMap, wordSet);
	char* output = toString_IntLL(list, LINKED, IntToWord_HashMap); 
	printf("%s", output);

}






void testMCTS(){
	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	//char* path = "../docs/t/mini.txt"; 
	char* path = "../docs/4.txt";

	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet* wordSet = init_WordSet(IntToWord_HashMap->numWords);
	
	Play_FLWP(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	**************INITIALIZATION COMPLETE******************
	
	//This is the starting word
	//int rootID = 2000; 


	//int s = montyCarlosTreeSearch(rootID, wordSet, IntToWord_HashMap);

	//Simulates mcts

	for(run = 0; run < 1; run++){
		//Set the root ID to found 
		markUsed_WordSet(rootID, wordSet);


		int i; 
		for(i = 0; i < 10; i++){
		
			int s = montyCarlosTreeSearch(rootID, wordSet, IntToWord_HashMap);
		    printf("MCST Result: %d\n", s);
			int y = botPly(rootID, 15, IntToWord_HashMap, wordSet, minimax);
			printf("Optimal Result: %d\n", y);
			markUnused_WordSet(y, wordSet);
			printf("\n");
        }
		//printf("\nCorrect Result: %d", y);
	}
		
		
		
		
	

	//reset_WordSet(wordSet);

	//Obtain the correct answer to be compared w/ the mcts results
	//int y = botPly(rootID, 15, IntToWord_HashMap, wordSet, minimax);
	
	//outputs the correct results
	//printf("\nCorrect Result: %d", y);
	

	//FREEING BEGINS
	free_WordSet(wordSet); 
	
	//Frees the hash maps
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
	
	
}

void runMultiplayerFLWG(){
	srand(time(0)); 
	
	
    
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);

	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);

	
	Multiplayer_FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//MultiplayerTest(IntToWord_HashMap);
	
	free_WordSet(wordSet);
	
	FLWG_Test(IntToWord_HashMap, wordSet);
	
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);

	
}
void testAlpaBetaPruning(struct wordDataArray *IntToWord_HashMap){
	struct WordSet* wordSet = init_WordSet(IntToWord_HashMap->numWords);
	int depth = 4;  
	struct minimaxOutput* a = createOutput(-100, 0, -1, -1); 
	struct minimaxOutput* b = createOutput(100, 1, -1, -1); 
	struct minimaxOutput* output = minimax(0, depth, depth, 1, *a, *b, IntToWord_HashMap, wordSet); 
	printf("Final Choice: %d", output->id); 
	
	free(output);
	free(a);
	free(b); 
	free_WordSet(wordSet);
}

void runFLWG(){
		srand(time(0));
	
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word
	struct wordDataArray *IntToWord_HashMap; 
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	//Allocates the Int2Word HashMap
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	//Initializes both hash maps, filling them out with their paths and words
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);
	//FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	//int s  = ChooseStart(IntToWord_HashMap);
	//printf("%s", Convert_IntToWord(s, IntToWord_HashMap));
	FLWG(WordToInt_HashMap, IntToWord_HashMap, wordSet);
	
	free_WordSet(wordSet); 
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);
}

void initializeStructures(){
	
	srand(time(0));
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	
	char* path = "../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
	
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);
	
	
	*********INSERT CODE HERE*******************
	
	free_WordSet(wordSet); 
	Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap);

}
*/
