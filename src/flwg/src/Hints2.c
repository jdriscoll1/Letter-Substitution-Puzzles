#include "../includes/Hints2.h"
#include "../../structs/includes/ArrayList.h"
#include "../../algs/includes/BreadthFirstSearch.h"

int direct_adjacency_hint(int wordId, struct DataStructures* data){
	// 1) Convert the integer to a word 
	char* word = Convert_IntToWord(wordId, data->I2W); 
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
	if(alist->currPrecision == 0){
		return -1; 
	}
	int chosenArrayListId = rand() % alist->currPrecision; 	
	int result = ((int*)alist->list)[chosenArrayListId]; 
	return result; 

}
