#include <stddef.h>
#include "../includes/BreadthFirstSearch_FLWP.h"
#include "../includes/GameFunctions.h"
#include "../../structs/includes/Queue.h"
#include "../../shared/includes/Relax.h"
#include "../../shared/includes/Log.h"
#include "../../structs/includes/IntLinkedList.h"

/* A start and a goal with a route between them.
 *
 * Three preferences - how well connected the start is, how far the goal sits
 * from it, how well connected the goal is - and one condition, which is that a
 * route exists at all. The BFS below is both: it only ever offers a goal it
 * has actually walked to, so a start that comes back with one is a start with
 * a route.
 *
 * The preferences are asked for and then given up a little at a time, because
 * the dictionary may have nothing that far out and asking again would produce
 * the same three numbers. The condition is not given up, because a board whose
 * goal cannot be reached is not a puzzle.
 */
struct GameComponents *findFLWPStartAndGoal(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data){
	struct GameComponents *gc = malloc(sizeof(struct GameComponents));

	struct Band startAsked = { minAdjacenciesToStart, maxAdjacenciesToStart };
	struct Band distanceAsked = { minDistance, maxDistance };
	struct Band goalAsked = { minAdjacenciesToGoal, maxAdjacenciesToGoal };

	int start = -1;
	int goal = -1;

	for(int round = 0; round < RELAXATION_ROUNDS && start == -1; round++){
		struct Band startBand = loosen(startAsked, round);
		struct Band goalBand = loosen(goalAsked, round);

		/* Distance is how far the BFS walks, so its ceiling cannot be handed
		   INT_MAX - that is the loop's bound, not a sentinel. Given up, it
		   becomes "anywhere at least one move away", which is the whole graph
		   and still a real puzzle. */
		struct Band distance = isAnything(round)
			? (struct Band){ 1, data->I2W->numWords }
			: loosen(distanceAsked, round);
		if(distance.min < 1){
			distance.min = 1;
		}

		// Every word whose own adjacency count is in range, which is the cheap half of the test
		int candidates[data->I2W->numWords];
		int numCandidates = 0;

		for(int id = 0; id < data->I2W->numWords; id++){
			int adj = getNumAdjacencies(id, data);
			if(adj >= startBand.min && adj <= startBand.max){
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
			int candidateGoal = chooseGoalBFS_FLWP(candidates[i], distance.min, distance.max, goalBand.min, goalBand.max, data);
			if(candidateGoal != -1){
				start = candidates[i];
				goal = candidateGoal;
			}
		}

		if(start != -1 && round > 0){
			FLWG_LOG("No route fit the board as asked; dealt one %d round(s) looser\n", round);
		}
	}

	gc->start = start;
	gc->goal = goal;
	gc->minConnections = 4;
	gc->solution = NULL;

	/* Only reachable when no word in the dictionary has a neighbour at all: the
	   last round asks for any start and any goal one move away. */
	if(start == -1){
		FLWG_LOG("There is no route to deal at all!\n");
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

