#ifndef QUEUE
#define QUEUE

#include "../../api/includes/FLWG-API.h"
#include "../includes/ArrayList.h"

struct Queue{
	struct QueueNode* header; 
	struct QueueNode* curr;
	struct QueueNode* tail; 
	int isEmpty; 
}; 
struct QueueNode{
	struct QueueNodeData* data; 
	struct QueueNode* next; 
	struct QueueNode* parent; 

}; 
struct QueueNodeData {
	int distance; 
	int id; 
}; 

void free_Queue(struct Queue *queue); 
struct Queue* init_Queue(); 
void enqueue(int id, int distance, struct QueueNode* parent, struct Queue *queue); 
struct QueueNode* dequeue(struct Queue* queue); 
void print_Queue(struct Queue* queue);
int isEmpty_Queue(struct Queue* queue); 

struct arrayList* getPathToHeader_Queue(struct QueueNode* node);
void _getPathToHeader_Queue(struct arrayList* aList, struct QueueNode* curr);
#endif
