#include <string.h>

#include "../includes/Challenges.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../../algs/includes/TreeStorageNode.h"

struct WordSet* convertCharPtrPtrToWordSet(char** words, struct DataStructures* data){

	struct WordSet* wordSet = init_WordSet(data->I2W->numWords);
	int i = 0; 
	int wordId; 
	while(words[i] != NULL){
		
		wordId = convertWordToInt(words[i], data);
	 	markUsed_WordSet(wordId, wordSet); 	
		i++; 
	}
	return wordSet; 
}

int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	
	// The array of valid words
	struct arrayList* validWords = init_ArrayList(20, 10, NUM); 	
	

	/*
	struct WordSet *goalWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strchr(w1, 'e') != NULL){
			markUsed_WordSet(i, goalWordSet); 
		}
	}
	flwcComponents->goalWords = goalWordSet; 
	p.goalWords = goalWordSet; 
	*/
	for(int i = 0; i < data->I2W->numWords; i++){

		int numConnections = data->I2W->array[i]->numConnections; 

		// if it has between the minimum and maximum number of connections 
		if(numConnections >= p.minAdjacencies && numConnections <= p.maxAdjacencies){
			// It is within the minimum and maximum distance from any goal words
			struct arrayList* pathToNearestWord = getPathToNearestWordInWordSet(i, p, data);  
			int distanceFromGoal = pathToNearestWord->currPrecision - 1; 
			if(distanceFromGoal >= p.minGoalDistance && distanceFromGoal <= p.maxGoalDistance){
				add_ArrayList(&i, validWords, NUM); 
			}
			free_ArrayList(pathToNearestWord); 
		}
	}

	if(validWords->currPrecision == 0){
		printf("There are no valid words!!!"); 
		return -1; 
	}
	
	int choiceId = rand() % validWords->currPrecision; 
	int startWordId = ((int*)validWords->list)[choiceId]; 

	
	free_ArrayList(validWords); 
	
	
	struct arrayList* solutionArray = getPathToNearestWordInWordSet(startWordId, p, data);  
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
