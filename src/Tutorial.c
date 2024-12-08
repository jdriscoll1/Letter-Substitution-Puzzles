#include "Tutorial.h"
#include "./flwp/includes/UserInput.h"
#include "./flwp/includes/GameFunctions.h"
#include "./structs/includes/WordSet.h"

#include <stdlib.h>


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWT* initFLWT(int minAdjacencies, int maxAdjacencies, struct DataStructures* data){

	struct GameComponentsFLWT* flwtComponents = malloc(sizeof(struct GameComponentsFLWT)); 
	flwtComponents->numAdjacenciesFound = 0; 
	flwtComponents->minAdjacenciesUserNeedsToFind = minAdjacencies; 
	flwtComponents->maxAdjacenciesThatCanBeFound = maxAdjacencies; 
	flwtComponents->prevWords = init_ArrayList(minAdjacencies, 5, STR); 
	flwtComponents->startWordId = getWordWithNumberOfConnections(maxAdjacencies, data->I2W);
	markUsed_WordSet(flwtComponents->startWordId, data->wordSet); 
	return flwtComponents; 

}

// Choose a word
char* getStartWordFLWT(struct GameComponentsFLWT* flwtComponents, struct DataStructures *data){
	return Convert_IntToWord(flwtComponents->startWordId, data->I2W); 


}

// Allow the user to insert a word 
int userEntersWordFLWT(char* userInput, struct GameComponentsFLWT* flwtComponents,  struct DataStructures* data){
	
	int isValid = Check_Input(flwtComponents->startWordId,(const char*)userInput, data->W2I, data->I2W, data->wordSet); 

	if(isValid == VALID){
			//Once it is valid, we can convert it into an integer
			int id = Convert_WordToInt(userInput, data->W2I); 

			//Copies the new word into the string arraylist
			addString_ArrayList(userInput, numLetters, flwtComponents->prevWords); 

			// The game component increments the number of moves
			flwtComponents->numAdjacenciesFound++;

			markUsed_WordSet(id, data->wordSet); 
	}

	return isValid; 
}


struct arrayList* getCurrentWordsFLWT(struct GameComponentsFLWT* flwtComponents)
{
	return flwtComponents->prevWords; 
}

int isGameWonFLWT(struct GameComponentsFLWT* flwtComponents){
	return flwtComponents->numAdjacenciesFound == flwtComponents->minAdjacenciesUserNeedsToFind; 
}

void freeGameComponentsFLWT(struct GameComponentsFLWT* flwtComponents){
	free_ArrayList(flwtComponents->prevWords); 	
	free(flwtComponents); 

}

void printFLWTComponents(struct GameComponentsFLWT* flwtComponents){
	struct arrayList *words = getCurrentWordsFLWT(flwtComponents);
	int length = (words->currPrecision) / (numLetters);
	char *rawStr = (char*)words->list;
	char tempStr[numLetters + 1];
	tempStr[numLetters] = '\0';
	printf("Raw String: %s\n", rawStr);
	for (int i = 0; i < length; i ++) {
		char *currStr = &rawStr[i*numLetters];
		strncpy(tempStr, currStr, numLetters);
		printf("String:  %s\n", tempStr);
	}
	printf("\n\n"); 

}

