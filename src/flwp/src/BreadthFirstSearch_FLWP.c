#include <stddef.h>
#include "../includes/BreadthFirstSearch_FLWP.h"
#include "../../structs/includes/Queue.h"
#include "../../structs/includes/IntLinkedList.h"

struct GameComponents *findFLWPStartAndGoal(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data){
	struct GameComponents *gc = malloc(sizeof(struct GameComponents)); 

	// Initialize the array list that will store all of the potentially valid words
	int validIDs[data->I2W->numWords];
	int numValidWords = 0; 

	// Check each word to see if it can have a valid goal 
	for(int id = 0; id < data->I2W->numWords; id++){

		// Ensure each start word has a valid number of adjacencies
		int adj = getNumAdjacencies(id, data); 
		if(adj >= minAdjacenciesToStart && adj <= maxAdjacenciesToStart){

			// if it can find at least one valid word, choose it 
			if(BFS_IsFLWPStartValid(id, minDistance, maxDistance, minAdjacenciesToGoal, maxAdjacenciesToGoal, data) == 1){
				validIDs[numValidWords++] = id; 
			}
		}
		// if it can, add it to the word set

	}
	
	int start = validIDs[rand() % numValidWords]; 
	int goal = chooseGoalBFS_FLWP(start, minDistance, maxDistance, minAdjacenciesToGoal, maxAdjacenciesToGoal, data); 	
	// Choose a word randomly from the eword set, figure out all words it can connect to given the constraints and then choose one of those
	gc->start = start; 
	gc->goal = goal; 
	gc->minConnections = 4; 
	
	getSolution_FLWP(gc->start, gc->goal, gc, data); 
	return gc; 


}



int BFS_IsFLWPStartValid(int id, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data){

	// Initialize the Queue, this keeps track of the current word that is being explored, and all words that will be explored
	struct Queue* q = init_Queue(); 
	enqueue(id, 0, NULL,  q);

	
	// Explored Nodes, keeps track of all explored Nodes
	struct WordSet* x = init_WordSet(data->I2W->numWords); 
	markUsed_WordSet(id, x); 
	while(!isEmpty_Queue(q)){
		struct QueueNode* parent = dequeue(q); 
		int currId = parent->data->id; 
		// if the distance is greater than the max distance and we have not been able to find a word, false
		if(parent->data->distance > maxDistance){
			continue; 
		}
		struct intList* c = getConnections(currId, data->I2W); 
		c = c->next; 
		while(c != NULL){
			int childDistance = parent->data->distance + 1; 
			int c_id = c->data; 
			
			if(checkIfUsed_WordSet(c_id, x) == 0){
				enqueue(c_id, childDistance, parent, q); 
				markUsed_WordSet(c_id, x); 
			}
			// if it happens to bump into a word within the parameters who abides by the parameters, it can free everything
			int a = getNumAdjacencies(c_id, data); 
			if(childDistance >= minDistance && childDistance <= maxDistance && a >= minAdjacenciesToGoal && a <= maxAdjacenciesToGoal){
				free_Queue(q); 
				free_WordSet(x); 
				return 1; 
			}
			c = c->next; 
		}
	}
	free_Queue(q); 
	free_WordSet(x); 

	return 0; 
}


int chooseGoalBFS_FLWP(int id, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data) {
	// Initialize the array of valid goals
	int validGoals[data->I2W->numWords]; 
	int numGoals = 0; 

	// Initialize the Queue of explored words
	struct Queue* q = init_Queue();
	enqueue(id, 0, NULL, q);

	// Explored Nodes
	struct WordSet* x = init_WordSet(data->I2W->numWords);
	markUsed_WordSet(id, x);

	while (!isEmpty_Queue(q)) {
		struct QueueNode* parent = dequeue(q);
		int currId = parent->data->id;
		int distance = parent->data->distance;

		// Stop BFS if we've exceeded maxDistance
		if (distance > maxDistance) {
			continue;
		}

		// Explore neighbors
		struct intList* c = getConnections(currId, data->I2W);
		c = c->next; 
		while (c != NULL) {
			int c_id = c->data;
			int childDistance = distance + 1;

			if (checkIfUsed_WordSet(c_id, x) == 0) {
				enqueue(c_id, childDistance, parent, q);
				markUsed_WordSet(c_id, x);
				int adj = getNumAdjacencies(c_id, data);
				if (childDistance >= minDistance && childDistance <= maxDistance &&
				adj >= minAdjacenciesToGoal && adj <= maxAdjacenciesToGoal) {
				validGoals[numGoals++] = c_id; 
				}

			}

			// Check if this child meets goal criteria
			
			c = c->next;
		}
	}

	// Cleanup BFS structures
	free_Queue(q);
	free_WordSet(x);

	// Return -1 if no valid goals found
	if(numGoals == 0){
		return -1; 
	}
		
	// Pick a random goal safely
	int index = rand() % numGoals;
	int goal = validGoals[index]; 
	return goal;
}


void getSolution_FLWP(int id, int goalId, struct GameComponents* gc, struct DataStructures* data) {

	// Initialize the Queue of explored words
	struct Queue* q = init_Queue();

	struct intList* solution = malloc(sizeof(struct intList)); 
	solution->size = 0; 
	solution->next = NULL; 

	enqueue(id, 0, NULL, q);

	// Explored Nodes
	struct WordSet* x = init_WordSet(data->I2W->numWords);
	markUsed_WordSet(id, x);

	while (!isEmpty_Queue(q)) {
		struct QueueNode* parent = dequeue(q);
		int currId = parent->data->id;
	
		if(currId == goalId){
			int goalDistance = 0; 
			while(parent != NULL){
				goalDistance++; 	
				AddToFront_IntLL(parent->data->id, solution); 
				parent = parent->parent; 
			}
			gc->solution = solution; 
			gc->minConnections = goalDistance; 	
			free_Queue(q);
			free_WordSet(x); 
			return; 
		}
		int distance = parent->data->distance;

		// Explore neighbors
		struct intList* c = getConnections(currId, data->I2W);
		c = c->next; 
		while (c != NULL) {
			int c_id = c->data;
			int childDistance = distance + 1;

			if (checkIfUsed_WordSet(c_id, x) == 0) {
				enqueue(c_id, childDistance, parent, q);
				markUsed_WordSet(c_id, x);

			}

			// Check if this child meets goal criteria
			
			c = c->next;
		}
	}

	// if the current id is equal to the goal id 
	gc->solution = NULL; 	
	gc->minConnections -1; 

	// Cleanup BFS structures
	free_Queue(q);
	free_WordSet(x);
}

