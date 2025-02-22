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
