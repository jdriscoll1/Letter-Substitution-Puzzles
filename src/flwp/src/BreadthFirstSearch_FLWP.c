#include <stddef.h>
#include "../includes/BreadthFirstSearch_FLWP.h"
#include "../includes/GameFunctions.h"
#include "../../structs/includes/Queue.h"
#include "../../structs/includes/IntLinkedList.h"

struct GameComponents *findFLWPStartAndGoal(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data){
	struct GameComponents *gc = malloc(sizeof(struct GameComponents));

	// Every word whose own adjacency count is in range, which is the cheap half of the test
	int candidates[data->I2W->numWords];
	int numCandidates = 0;

	int start = -1;
	int goal = -1;

	for(int id = 0; id < data->I2W->numWords; id++){
		int adj = getNumAdjacencies(id, data);
		if(adj >= minAdjacenciesToStart && adj <= maxAdjacenciesToStart){
			candidates[numCandidates++] = id;
		}
	}

	// Walk the candidates in random order and keep the first one that can reach a goal.
	// Taking the first hit out of a shuffled list picks uniformly among the words that
	// qualify, exactly as scoring every word and then choosing one at random did, but
	// it runs a search per attempt instead of a search per word in the dictionary.
	// The goal search doubles as the validity test, so a start can no longer be accepted
	// and then fail to produce a goal.
	Shuffle_IntArray(candidates, numCandidates);
	for(int i = 0; i < numCandidates && start == -1; i++){
		int candidateGoal = chooseGoalBFS_FLWP(candidates[i], minDistance, maxDistance, minAdjacenciesToGoal, maxAdjacenciesToGoal, data);
		if(candidateGoal != -1){
			start = candidates[i];
			goal = candidateGoal;
		}
	}

	gc->start = start;
	gc->goal = goal;
	gc->minConnections = 4;
	gc->solution = NULL;

	// No word in the dictionary can host these parameters -- isStartValid_FLWP reports it
	if(start == -1){
		return gc;
	}

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

	// The goal was never reached, so there is no solution to hand back 
	Free_IntLL(solution); 
	gc->solution = NULL; 	

	// Cleanup BFS structures
	free_Queue(q);
	free_WordSet(x);
}

