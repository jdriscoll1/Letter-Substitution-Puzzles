#ifndef seenFLWCTutorial
#define seenFLWCTutorial

#include "../includes/FLWG-API.h"
#include "../../structs/includes/ArrayList.h"
#include "../../flwc/includes/Challenges.h"
#include "../../flwc/includes/FLWC.h"
#include "../../flwp/includes/UserInput.h"
#include "../../flwg/includes/FLWGGame.h"


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWC* initFLWC(int numAdjacenciesToStartWord, char** goalWords, char** avoidWords, int minGoalDistance, int minAvoidDistance, int maxGoalDistance, int maxAvoidDistance, struct DataStructures* dataStructures){


	// Create the FLWC Components
	struct GameComponentsFLWC* flwcComponents = malloc(sizeof(struct GameComponentsFLWC)); 

	
	flwcComponents->goalWords = convertCharPtrPtrToWordSet(goalWords, dataStructures); 
	flwcComponents->avoidWords = convertCharPtrPtrToWordSet(avoidWords, dataStructures); 

	struct StartWordParametersFLWC params = {
	    .goalWords = flwcComponents->goalWords,
	    .avoidWords = flwcComponents->avoidWords,
	    .minGoalDistance = minGoalDistance,
	    .maxGoalDistance = maxGoalDistance,
	    .minAvoidDistance = minAvoidDistance,
	    .maxAvoidDistance = maxAvoidDistance,
	    .minAdjacencies = numAdjacenciesToStartWord,
	    .maxAdjacencies = 100,
	};
	flwcComponents->wordId = chooseStartWord_FLWCGeneral(params, flwcComponents, dataStructures);
	
	printf("Starting Word: %s", Convert_IntToWord(flwcComponents->wordId, dataStructures->I2W)); 
	reset_WordSet(dataStructures->wordSet); 
	markUsed_WordSet(flwcComponents->wordId, dataStructures->wordSet); 
	return flwcComponents; 

}

// Choose a word
char* getStartWordFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures *data){
	return Convert_IntToWord(flwcComponents->wordId, data->I2W); 

}

// Allow the user to insert a word 
int userEntersWordFLWC(char* userInput, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	int isValid = Check_Input(flwcComponents->wordId,(const char*)userInput, data); 

	if(isValid == VALID){
		// if the word is valid, mark it as such
		int id = Convert_WordToInt(userInput, data); 
		markUsed_WordSet(id, data->wordSet); 
		flwcComponents->wordId = id; 
	}

	return isValid; 


}

// while the result is != +1, the game should continue
int isGameWonFLWC(struct GameComponentsFLWC* flwcComponents){
	// if the game is tied, it gets set to 0
	if(flwcComponents->wordId == -1){
		// tie 
		return 0; 
	}
	// if a goal word is put, the game is won 
	if(checkIfUsed_WordSet(flwcComponents->wordId, flwcComponents->goalWords)){
		// win 
		return 1; 
	}
	// if an avoid word gets added, the game is lost
	if(checkIfUsed_WordSet(flwcComponents->wordId, flwcComponents->avoidWords)){
		// lose
		return 2; 
	}
	return -1; 



}

int botTakesTurnFLWC(int botType, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	// This is what will be returend 
	int result; 	
	if(botType == -1){
		result = botPly_MaxAdjacencies(flwcComponents->wordId, flwcComponents->goalWords, data); 	
	}
	if(botType > 0){
		result = botPly_FLWC(flwcComponents->wordId, botType, flwcComponents->avoidWords, flwcComponents->goalWords, data);
	}

	flwcComponents->wordId = result;  

	if(result == -1){
		return -1; 
	}

	// if the bot has trapped the user, let the user know 
	if(isTrapped(flwcComponents->wordId, data) == 1){
		return -2; 
	}
	return result; 

}

char* getSolutionFLWC(struct GameComponentsFLWC* flwcComponents){
	return flwcComponents->solution; 		

}

void freeGameComponentsFLWC(struct GameComponentsFLWC* flwcComponents){
	free_WordSet(flwcComponents->avoidWords); 
	free_WordSet(flwcComponents->goalWords); 
	free(flwcComponents->solution); 
	free(flwcComponents); 


}

int isStartValidFLWC(struct GameComponentsFLWC* flwcComponents){
	return flwcComponents->wordId != -1;
}
char** getAllWords(struct DataStructures* dataStructures){
	
	char** allWords = malloc(sizeof(char*) * dataStructures->I2W->numWords); 
	for(int i = 0; i < dataStructures->I2W->numWords; i++){
		allWords[i] = dataStructures->I2W->array[i]->word; 
	}
	return allWords; 
}

#endif
