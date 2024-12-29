#ifndef seenFLWCTutorial
#define seenFLWCTutorial

#include "../includes/FLWG-API.h"
#include "../../structs/includes/ArrayList.h"
#include "../../flwc/includes/Challenges.h"
#include "../../flwc/includes/FLWC.h"
#include "../../flwp/includes/UserInput.h"

struct EndWordParametersFLWC getChallengeGoalParameters(int challengeId);
struct EndWordParametersFLWC getChallengeAvoidParameters(int challengeId);

// This creates the first word and returns a set of game components for the flwt
struct GameComponentsFLWC* initFLWC(int challengeId, struct DataStructures* data){


	// Create the FLWC Components
	struct GameComponentsFLWC* flwcComponents = malloc(sizeof(struct GameComponentsFLWC)); 

	
	// Create the parameters for the goal word 
	struct EndWordParametersFLWC goalParams = getChallengeGoalParameters(challengeId); 

	// Create teh parameters for the avoid words 
	struct EndWordParametersFLWC avoidParams = getChallengeAvoidParameters(challengeId); 
	
	
	// Create the list of goal words 
	flwcComponents->goalWords = createWordSetGivenCondition(goalParams, data); 
	flwcComponents->avoidWords = createWordSetGivenCondition(avoidParams, data); 

	struct StartWordParametersFLWC params = {
	    .goalWords = flwcComponents->goalWords,
	    .avoidWords = flwcComponents->avoidWords,
	    .minGoalDistance = 3,
	    .maxGoalDistance = 6,
	    .minAvoidDistance = 2,
	    .maxAvoidDistance = 6,
	    .minAdjacencies = 5,
	    .maxAdjacencies = 100,
	};

	flwcComponents->wordId = chooseStartWord_FLWCGeneral(params, flwcComponents, data);
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

int botTakesTurnFLWC(struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	
	int depth = 6; 
	int result = botPly_FLWC(flwcComponents->wordId, depth, flwcComponents->avoidWords, flwcComponents->goalWords, data);
	flwcComponents->wordId = result;  
	return result; 

}

char* getSolutionFLWC(struct GameComponentsFLWC* flwcComponents){
	return flwcComponents->solution; 		

}

void freeFLWC(struct GameComponentsFLWC* flwcComponents){
	free_WordSet(flwcComponents->avoidWords); 
	free_WordSet(flwcComponents->goalWords); 
	free(flwcComponents->solution); 
	free(flwcComponents); 


}

struct EndWordParametersFLWC getChallengeGoalParameters(int challengeId){
	struct EndWordParametersFLWC params; 
	switch(challengeId){
		case 0: 
			params.comparatorId=0;
			params.letter='e';
			break; 
	}
	return params; 

}


struct EndWordParametersFLWC getChallengeAvoidParameters(int challengeId){
	struct EndWordParametersFLWC params; 
	switch(challengeId){
		case 0: 
			params.comparatorId=0;
			params.letter='t';
			break; 
	}
	return params; 


}

#endif
