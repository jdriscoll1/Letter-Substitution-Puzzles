#ifndef seenFLWCTutorial
#define seenFLWCTutorial

#include "../includes/FLWG-API.h"
#include "../../structs/includes/ArrayList.h"
#include "../../flwc/includes/Challenges.h"
#include "../../flwc/includes/FLWC.h"
#include "../../flwp/includes/UserInput.h"
#include "../../flwg/includes/FLWGGame.h"
#include "../../algs/includes/BreadthFirstSearch.h"


// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWC* initFLWC(int minAdjacenciesToStart, int maxAdjacenciesToStart, char** goalWords, char** avoidWords, int minGoalDistance, int minAvoidDistance, int maxGoalDistance, int maxAvoidDistance, int minGoalAdjacencies, int maxGoalAdjacencies, int numTurns,struct DataStructures* dataStructures){


	// Create the FLWC Components
	struct GameComponentsFLWC* flwcComponents = malloc(sizeof(struct GameComponentsFLWC)); 

	
	flwcComponents->goalWords = convertCharPtrPtrToWordSet(goalWords, dataStructures); 
	flwcComponents->avoidWords = convertCharPtrPtrToWordSet(avoidWords, dataStructures); 

	
	/* A floor under what a caller may ask for, as well as under what the
	 * search is allowed to relax to.
	 *
	 * A level naming a minimum of nought or one is describing a board that
	 * can be won in a single move, which is not a board - so the ask is
	 * raised to the floor here and held there by the search.
	 *
	 * Left alone when the caller is not asking about goals at all. Nought
	 * and nought together mean do not look, which is what an avoid board
	 * hands in, and putting a floor under a band nobody reads would turn it
	 * into a demand that a goal word exist. */
	int goalFloor = minGoalDistance;
	if(maxGoalDistance > 0 && goalFloor < NEAREST_GOAL_ALLOWED){
		goalFloor = NEAREST_GOAL_ALLOWED;
	}

	struct StartWordParametersFLWC params = {
	    .goalWords = flwcComponents->goalWords,
	    .avoidWords = flwcComponents->avoidWords,
		// adding + 1 to each to account for the initial word pins->pies is counted as 2, but it is thought of as one adjacency away
	    .minGoalDistance = goalFloor,
		// for max same applies: pies->tins is pies->ties->tins. It's counted as 3. but because it's > 2 it would not be allowed
	    .maxGoalDistance = maxGoalDistance,
	    .minAvoidDistance = minAvoidDistance,
	    .maxAvoidDistance = maxAvoidDistance,
	    .minAdjacencies = minAdjacenciesToStart,
	    .maxAdjacencies = maxAdjacenciesToStart,
	    .minGoalAdjacencies = minGoalAdjacencies,
	    .maxGoalAdjacencies = maxGoalAdjacencies,
	    .numTurns = numTurns,
	};
	flwcComponents->wordId = chooseStartWord_FLWCGeneral(params, flwcComponents, dataStructures);
	reset_WordSet(dataStructures->wordSet); 
	if(flwcComponents->wordId != -1){
		markUsed_WordSet(flwcComponents->wordId, dataStructures->wordSet); 
	}
	return flwcComponents; 
}

// Choose a word
char* getStartWordFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures *data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return NULL;
	}

	return Convert_IntToWord(flwcComponents->wordId, data->I2W); 

}

// Allow the user to insert a word 
int userEntersWordFLWC(char* userInput, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return -1;
	}

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
	/* nothing to work with */
	if(flwcComponents == NULL){
		return 0;
	}

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
	/* nothing to work with */
	if(flwcComponents == NULL){
		return -1;
	}

	// This is what will be returend.
	// Started at -1 (no move): an unrecognised bot type used to leave this
	// uninitialised and then assign the garbage straight into wordId
	int result = -1; 	
	if(botType == -2){
		result = botPly_Mirror(flwcComponents->wordId); 
	}
	if(botType == -1){
		result = botPly_MaxAdjacencies(flwcComponents->wordId, flwcComponents->goalWords, data); 	
	}
	if(botType == 0){
		result = botPly_Random(flwcComponents->wordId, data); 
	}
	if(botType > 0){
		result = botPly_FLWC(flwcComponents->wordId, botType, flwcComponents->avoidWords, flwcComponents->goalWords, data);
	}

	flwcComponents->wordId = result;  

	// Bot Loses If Result is -1 
	if(result == -1){
		return -1; 
	}

	// Bot Wins (User Is Trapped) on Return -2 
	if(isTrapped(flwcComponents->wordId, data) == 1){
		return -2; 
	}
	return result; 

}


void freeGameComponentsFLWC(struct GameComponentsFLWC* flwcComponents){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return;
	}

	free_WordSet(flwcComponents->avoidWords); 
	free_WordSet(flwcComponents->goalWords); 
	free(flwcComponents); 


}

int isStartValidFLWC(struct GameComponentsFLWC* flwcComponents){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return 0;
	}

	return flwcComponents->wordId != -1;
}
char** getAllWords(struct DataStructures* dataStructures){
	
	char** allWords = malloc(sizeof(char*) * dataStructures->I2W->numWords); 
	for(int i = 0; i < dataStructures->I2W->numWords; i++){
		allWords[i] = dataStructures->I2W->array[i]->word; 
	}
	return allWords; 
}



char* hintGoalWordFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return NULL;
	}

	struct StartWordParametersFLWC p = {
		.goalWords=flwcComponents->goalWords,
		.avoidWords=flwcComponents->avoidWords,
		.minGoalDistance=0,
		.maxGoalDistance=100,
		.minAvoidDistance=0,
		.maxAvoidDistance=100,
		.minAdjacencies=1,
		.maxAdjacencies=100,
		.minGoalAdjacencies=1,
		.maxGoalAdjacencies=100,
		.numTurns = 100
	};
	struct arrayList* solution = getPathToNearestWordInWordSet(flwcComponents->wordId, p, data);
	//An empty path means no goal word was reachable. malloc(0) hands back a non-NULL
	//pointer, so the path has to be judged by its length rather than by its pointer
	if(solution == NULL){
		return NULL;
	}
	if(solution->currPrecision == 0){
		free_ArrayList(solution);
		return NULL;
	}

	int goalId = ((int*)(solution->list))[solution->currPrecision - 1]; 
	char* goalStr = Convert_IntToWord(goalId, data->I2W); 
	
	free_ArrayList(solution); 

	return goalStr; 
}
char* hintPathToGoalFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return NULL;
	}

	struct StartWordParametersFLWC p = {
		.goalWords=flwcComponents->goalWords,
		.avoidWords=flwcComponents->avoidWords,
		.minGoalDistance=0,
		.maxGoalDistance=100,
		.minAvoidDistance=0,
		.maxAvoidDistance=100,
		.minAdjacencies=1,
		.maxAdjacencies=100,
		.minGoalAdjacencies=1,
		.maxGoalAdjacencies=100
	};
	struct arrayList* solution = getPathToNearestWordInWordSet(flwcComponents->wordId, p, data);
	if(solution == NULL){
		return NULL;
	}
	if(solution->currPrecision == 0){
		free_ArrayList(solution);
		return NULL;
	}
	char* hint = idArrayListToString(solution, data);
	free_ArrayList(solution); 
	return hint; 

}

char* hintAdjacencyTowardsGoalFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return NULL;
	}

	struct StartWordParametersFLWC p = {
		.goalWords=flwcComponents->goalWords,
		.avoidWords=flwcComponents->avoidWords,
		.minGoalDistance=0,
		.maxGoalDistance=100,
		.minAvoidDistance=0,
		.maxAvoidDistance=100,
		.minAdjacencies=1,
		.maxAdjacencies=100,
		.minGoalAdjacencies=1,
		.maxGoalAdjacencies=100
	};
	struct arrayList* solution = getPathToNearestWordInWordSet(flwcComponents->wordId, p, data);
	//The step towards the goal is the second entry, so a path shorter than two is no hint
	if(solution == NULL){
		return NULL;
	}
	if(solution->currPrecision < 2){
		free_ArrayList(solution);
		return NULL;
	}

	int goalId = ((int*)(solution->list))[1]; 
	char* goalStr = Convert_IntToWord(goalId, data->I2W); 
	
	free_ArrayList(solution); 

	return goalStr; 
}
int hintMinDistanceToGoalFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return -1;
	}

	struct StartWordParametersFLWC p = {
		.goalWords=flwcComponents->goalWords,
		.avoidWords=flwcComponents->avoidWords,
		.minGoalDistance=0,
		.maxGoalDistance=100,
		.minAvoidDistance=0,
		.maxAvoidDistance=100,
		.minAdjacencies=1,
		.maxAdjacencies=100,
		.minGoalAdjacencies=1,
		.maxGoalAdjacencies=100
	};
	struct arrayList* solution = getPathToNearestWordInWordSet(flwcComponents->wordId, p, data);
	if(solution == NULL){
		return -1;
	}
	if(solution->currPrecision == 0){
		free_ArrayList(solution);
		return -1;
	}
	int hint = solution->currPrecision-1;
	free_ArrayList(solution); 

	return hint; 
}

char* hintBestDirectAdjacencyFLWIC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return NULL;
	}


	// Given FLWC Components, it runs an FLWC Bot Ply After Switching Avoid & Goal Words
	int id = botPly_FLWC(flwcComponents->wordId, 5, flwcComponents->goalWords, flwcComponents->avoidWords, data); 
	if(id != -1){
		markUnused_WordSet(id, data->wordSet); 
		return Convert_IntToWord(id, data->I2W); 
	}
	return NULL; 
}
int hintDistanceFromNearestAvoidWordFLWIC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	/* nothing to work with */
	if(flwcComponents == NULL){
		return -1;
	}

	struct StartWordParametersFLWC p = {
		.goalWords=flwcComponents->avoidWords,
		.avoidWords=flwcComponents->goalWords,
		.minGoalDistance=0,
		.maxGoalDistance=100,
		.minAvoidDistance=0,
		.maxAvoidDistance=100,
		.minAdjacencies=1,
		.maxAdjacencies=100,
		.minGoalAdjacencies=1,
		.maxGoalAdjacencies=100
	};
	struct arrayList* solution = getPathToNearestWordInWordSet(flwcComponents->wordId, p, data);
	if(solution == NULL){
		return -1;
	}
	if(solution->currPrecision == 0){
		free_ArrayList(solution);
		return -1;
	}
	int hint = solution->currPrecision-1;
	free_ArrayList(solution); 

	return hint; 

}

#endif
