#include <string.h>

#include "../includes/Challenges.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../../algs/includes/TreeStorageNode.h"

void swapAvoidGoal(struct StartWordParametersFLWC* p);
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


int isWordValid_StartWordParameters(int i, struct StartWordParametersFLWC p, struct DataStructures* data){
	struct arrayList* pathToNearestWord = getPathToNearestWordInWordSet(i, p, data);  
	int distanceFromGoal = pathToNearestWord->currPrecision; 
	free_ArrayList(pathToNearestWord); 
	return distanceFromGoal >= p.minGoalDistance && distanceFromGoal <= p.maxGoalDistance; 

}

int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){
	
	// The array of valid words
	struct arrayList* validWords = init_ArrayList(20, 10, NUM); 	
	

	
	/*
	struct WordSet *goalWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strstr(w1, "oo") != NULL){
			markUsed_WordSet(i, goalWordSet); 
		}
	}
	flwcComponents->goalWords = goalWordSet; 
	p.goalWords = goalWordSet; 

	// Avoid Words
	struct WordSet *avoidWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strchr(w1, 'd') != NULL){
			markUsed_WordSet(i, avoidWordSet); 
		}
	}
	flwcComponents->avoidWords = avoidWordSet; 
	p.avoidWords = avoidWordSet; 
	*/
	

	// For Each word
	for(int i = 0; i < data->I2W->numWords; i++){

		// Ensure that it has the correct # of Connections
		int numConnections = data->I2W->array[i]->numConnections; 
		if(numConnections >= p.minAdjacencies && numConnections <= p.maxAdjacencies){

			int isValid_Goal = 1; 
			int isValid_Avoid = 1; 
			// Find the Path To The Nearest Word In the Goal Set
			// If its distance from the nearest goal word is w/in bounds
			if(p.minGoalDistance != 1 || p.maxGoalDistance != 1){
				isValid_Goal = isWordValid_StartWordParameters(i, p, data); 
			}
			if(p.minAvoidDistance != 1 || p.maxAvoidDistance != 1){
				swapAvoidGoal(&p); 
				isValid_Avoid = isWordValid_StartWordParameters(i, p, data); 
				swapAvoidGoal(&p); 
			}
			if(isValid_Goal == 1 && isValid_Avoid == 1){
				add_ArrayList(&i, validWords, NUM); 
			}
		}
	}

	if(validWords->currPrecision == 0){
		printf("There are no valid words!!!\n"); 
		free_ArrayList(validWords); 
		return -1; 
	}
	
	int choiceId = rand() % validWords->currPrecision; 
	int startWordId = ((int*)validWords->list)[choiceId]; 

	
	free_ArrayList(validWords); 
	
	
//	struct arrayList* solutionArray = getPathToNearestWordInWordSet(startWordId, p, data);  
//	flwcComponents->solution = idArrayListToString(solutionArray, data); 
//	free_ArrayList(solutionArray); 
	
	return startWordId; 
}
void swapAvoidGoal(struct StartWordParametersFLWC* p) {
    int a = p->minAvoidDistance;
    int b = p->maxAvoidDistance;
    struct WordSet* c = p->avoidWords;
    
    p->minAvoidDistance = p->minGoalDistance;
    p->maxAvoidDistance = p->maxGoalDistance;
    p->avoidWords = p->goalWords;
    
    p->minGoalDistance = a;
    p->maxGoalDistance = b;
    p->goalWords = c;
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
