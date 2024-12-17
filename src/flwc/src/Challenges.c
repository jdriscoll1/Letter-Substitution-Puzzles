#include <string.h>

#include "../includes/Challenges.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../../algs/includes/TreeStorageNode.h"

struct WordSet* createWordSetGivenCondition(struct EndWordParametersFLWC wordParams, struct DataStructures* data){

	struct WordSet* wordSet = init_WordSet(data->I2W->numWords);
	int (*comparator[])(char*, char) = {doesWordContainLetter}; 
	// for each word in the data set
	for(int i = 0; i < data->I2W->numWords; i++){
		if(comparator[wordParams.comparatorId](Convert_IntToWord(i, data->I2W), wordParams.letter)){
			markUsed_WordSet(i, wordSet); 
		}
	}
	return wordSet; 


}

// Conditions 
int doesWordContainLetter(char* word, char letter) {
	return (strchr(word, letter) != NULL) ? 1 : 0;   
}

int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){

	// The array of valid words
	struct arrayList* validWords = init_ArrayList(20, 10, NUM); 	
	
	// Go through each word and check the following:
	for(int i = 0; i < data->I2W->numWords; i++){
		int numConnections = data->I2W->array[i]->numConnections; 

		// if it has between the minimum and maximum number of connections 
		if(numConnections >= p.minAdjacencies && numConnections <= p.maxAdjacencies){
			// It is within the minimum and maximum distance from any goal words
			struct arrayList* pathToNearestWord = getPathToNearestWordInWordSet(i, p.maxGoalDistance,  p.goalWords, p.avoidWords, data);  
			int distanceFromGoal = pathToNearestWord->currPrecision; 
			if(distanceFromGoal > p.minGoalDistance && distanceFromGoal < p.maxGoalDistance){
				add_ArrayList(&i, validWords, NUM); 
			}
			free_ArrayList(pathToNearestWord); 
		}
	}

	if(validWords->currPrecision == 0){
		printf("There are no valid words!!!"); 
		exit(0); 
	}
	
	int choiceId = rand() % validWords->currPrecision; 
	int startWordId = ((int*)validWords->list)[choiceId]; 

	
	free_ArrayList(validWords); 
	
	
	struct arrayList* solutionArray = getPathToNearestWordInWordSet(startWordId, p.maxGoalDistance,  p.goalWords, p.avoidWords, data);  
	flwcComponents->solution = idArrayListToString(solutionArray, data); 
	free_ArrayList(solutionArray); 
	
	return startWordId; 
}


void getSetOfSurroundingWords(int id, int distance, struct WordSet* wordSet, struct DataStructures* data){

	struct BFSResults result = BreadthFirstSearch_Distance(id, distance, data->I2W, data->wordSet); 

	struct TreeStorageNode* curr = result.dataStorage->ReverseTreeHeader; 
	curr = curr->next;

	while(curr != NULL){
		markUsed_WordSet(curr->id, wordSet);
		curr = curr->next; 
	}

	free_ArrayList(result.list); 
	Free_BFSComponents(result.dataStorage, data->wordSet);

}
