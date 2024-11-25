#include "Tutorial.h"
#include "./flwp/includes/UserInput.h"
#include "./structs/includes/WordSet.h"

#include <stdlib.h>


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWT* initFLWT(int minAdjacencies, struct DataStructures* data){

	struct GameComponentsFLWT* flwtComponents = malloc(sizeof(struct GameComponentsFLWT)); 
	flwtComponents->numAdjacenciesFound = 0; 
	flwtComponents->minAdjacencies = minAdjacencies; 
	flwtComponents->prevWords = init_ArrayList(minAdjacencies, 5, STR); 
	flwtComponents->startWord = getStartWordFLWT(minAdjacencies, data); 
	return flwtComponents; 

}

// Choose a word
int getStartWordFLWT(int minAdjacencies, struct DataStructures *data){
	int chosenWord = 0; 
	// Find all words that have n-connections & choose randomly among them 
	markUsed_WordSet(chosenWord, data->wordSet); 
	return chosenWord; 	


}

// Allow the user to insert a word 
int userEntersWordFLWT(char* userInput, struct GameComponentsFLWT* flwtComponents,  struct DataStructures* data){
	
	int isValid = Check_Input(flwtComponents->startWord,(const char*)userInput, data->W2I, data->I2W); 
	if(isValid == 1){
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
	return flwtComponents->numAdjacenciesFound == flwtComponents->minAdjacencies; 
}

void freeGameComponentsFLWT(struct GameComponentsFLWT* flwtComponents){
	free_ArrayList(flwtComponents->prevWords); 	
	free(flwtComponents); 

}

