#include <stddef.h>

#include "../includes/Queue.h"

struct Queue* init_Queue(){
	struct Queue* queue = malloc(sizeof(struct Queue)); 
	queue->header = NULL; 
	queue->tail = NULL; 
	queue->curr = NULL; 
	queue->isEmpty = 1; 
	return queue; 
}

void free_Queue(struct Queue *queue){
	while(queue->header != NULL){
		struct QueueNode* temp = queue->header->next; 	
		free(queue->header->data); 
		free(queue->header); 
		queue->header = temp; 
		
	}
	free(queue); 
}

void enqueue(int id, int distance, struct QueueNode* parent, struct Queue *queue){

	// Initialize the Data for the Queue 
	struct QueueNodeData* data = malloc(sizeof(struct QueueNodeData)); 
	data->id = id; 
	data->distance = distance; 

	// Initialize the Queue node that will be added to the Queue
	struct QueueNode* n = malloc(sizeof(struct QueueNode)); 
	n->parent = parent; 
	n->data = data; 
	n->next = NULL; 

	// if the queue is empty 		
	if(queue->header == NULL){
		// if the queue is empty because it has not been started, we set everything to true
		queue->tail = n; 
		queue->header = queue->tail; 
		queue->curr = queue->tail; 	
	
	}
	else{
		queue->tail->next = n; 	
		queue->tail = queue->tail->next; 
	}
	queue->isEmpty = 0; 

}
struct QueueNode* dequeue(struct Queue* queue){


	if(queue->isEmpty == 1){
		return NULL; 	
	}	
	if(queue->curr == queue->tail){
		queue->isEmpty = 1; 
		return queue->curr; 
		
	}

	queue->curr = queue->curr->next; 
	return queue->curr; 
	

}

void print_Queue(struct Queue* queue){
	if(queue->isEmpty == 1){
		printf("Queue is Empty");
		return ;
	}
	struct QueueNode* q = queue->header; 
	while(q != NULL){
		printf("{ID: %d Distance: %d}\n", q->data->id, q->data->distance);
		q = q->next; 
	}

}

int isEmpty_Queue(struct Queue* queue){
	return queue->isEmpty; 
}


struct arrayList* getPathToHeader_Queue(struct QueueNode* node){
	if(node == NULL){
		printf("Given Input is Null");
	}
	struct arrayList* pathToHeader = init_ArrayList(10, 10, NUM); 
	_getPathToHeader_Queue(pathToHeader, node); 
	return pathToHeader; 
}

void _getPathToHeader_Queue(struct arrayList* aList, struct QueueNode* curr){
	if(curr != NULL){	
		_getPathToHeader_Queue(aList, curr->parent);  
		add_ArrayList(&(curr->data->id), aList, NUM); 	
	}
}
