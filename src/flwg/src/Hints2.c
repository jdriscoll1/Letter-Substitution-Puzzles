#include "../includes/Hints2.h"
#include "../../structs/includes/ArrayList.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../includes/FLWGGame.h"

int directAdjacencyHint(int wordId, struct DataStructures* data){
	// 1) Convert the integer to a word 
	if(isTrapped(wordId, data)){
		return -1; 
	}
	// 2) Look at all of the valid words
	int n = data->I2W->array[wordId]->numConnections;
	struct intList* conn = getConnections(wordId, data->I2W);  
	struct arrayList* alist = init_ArrayList(n, 5, NUM); 
	while(conn->next != NULL){
		conn = conn->next; 
		int id = conn->data; 
		if(checkIfUsed_WordSet(id, data->wordSet) == 0){
			add_ArrayList((void*)&id, alist, NUM); 
		}
	}
	int chosenArrayListId = rand() % alist->currPrecision; 	
	int result = ((int*)alist->list)[chosenArrayListId]; 
	free_ArrayList(alist); 
	return result; 

}


char letterToConsiderHint(int id, struct DataStructures* data){
	// Take the start word ID
	int hintWordId = -1; 
	char hint = '?';
	struct intList* c = getConnections(id, data->I2W); 
	c = c->next; 
	// Loop through the num adjacenceis
	while(c != NULL){
		int c_id = c->data; 
		if(checkIfUsed_WordSet(c_id, data->wordSet) == 0){
			// if the word is not used, then you can set it to the hint word id and Break 	
			hintWordId = c_id; 
			break;
		}
		c = c->next; 

	}
	
	char* goalWord = Convert_IntToWord(hintWordId, data->I2W); 
	char* currWord = Convert_IntToWord(id, data->I2W); 
	for(int i = 0; i < data->I2W->numLetters; i++){
		if(goalWord[i] != currWord[i]){
			hint = goalWord[i]; 
		}

	}
	return hint; 

}


int numOptionsHint(int id, struct DataStructures* data){
	// Take the start word ID
	int numOptions = 0;
	struct intList* c = getConnections(id, data->I2W); 
	c = c->next; 
	// Loop through the num adjacenceis
	while(c != NULL){
		int c_id = c->data; 
		if(checkIfUsed_WordSet(c_id, data->wordSet) == 0){
			// if the word is not used, then you can set it to the hint word id and Break 	
			numOptions++;
			c = c->next; 
		}
	}
	return numOptions; 
}
