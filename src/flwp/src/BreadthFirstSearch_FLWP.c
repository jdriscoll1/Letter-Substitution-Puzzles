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

	/* One round past the last, and it is the one that used to be the last.
	   Every round before it refuses to open on a word this board may not use
	   and confines the route to those words; the extra round drops both, so
	   nothing that could be dealt before can fail to be dealt now. */
	for(int round = 0; round <= RELAXATION_ROUNDS && start == -1; round++){
		int takeAnythingPlayable = (round == RELAXATION_ROUNDS);
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
			/* The cap is not a preference the way the bands are: a walk that
			   opens on a word nobody has heard of is the fault it exists to
			   stop, so it is refused every round rather than widened along
			   with them. */
			if(adj >= startBand.min && adj <= startBand.max
				&& !isOffLimits(id, data)
				&& (takeAnythingPlayable || !isTooObscure(id, data))){
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
			int candidateGoal = chooseGoalBFS_FLWP(candidates[i], distance.min, distance.max, goalBand.min, goalBand.max, !takeAnythingPlayable, data);
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



int chooseGoalBFS_FLWP(int id, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, int withinTier, struct DataStructures* data) {
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

			/* A word this board may not use is neither offered as a goal nor
			   walked through on the way to one, so the goal and the road to it
			   are both made of the words the board would deal. Narrowing a
			   graph only ever makes distances longer, so what comes back is a
			   goal at least as far out as the band asked for - never one that
			   is secretly nearer by a route through a word nobody knows. */
			if (isOffLimits(c_id, data) || (withinTier && isTooObscure(c_id, data))) {
				c = c->next;
				continue;
			}

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


/* The route the board is dealt with, and the number the game then quotes.
 *
 * gc->minConnections comes out of this, and that number is not private: the
 * score is figured against it and the first hint reads it out as the number of
 * moves the puzzle takes. So it has to be a distance the player can actually
 * walk. A route that runs through a word nobody has heard of makes the quoted
 * number true of the dictionary and false of the person holding the phone.
 *
 * The two ends are never tested. The start is where the player already is and
 * the goal is what they have been told to reach - refusing either would be
 * refusing the board rather than routing around a word. Only what lies between
 * is this function's to choose.
 */
static int solve_FLWP(int id, int goalId, struct GameComponents* gc, int withinTier, struct DataStructures* data) {

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
			return 1;
		}
		int distance = parent->data->distance;

		// Explore neighbors
		struct intList* c = getConnections(currId, data->I2W);
		c = c->next;
		while (c != NULL) {
			int c_id = c->data;
			int childDistance = distance + 1;

			/* The goal is exempt because it is an end and not a step. */
			if ((isOffLimits(c_id, data) && c_id != goalId)
				|| (withinTier && c_id != goalId && isTooObscure(c_id, data))) {
				c = c->next;
				continue;
			}

			if (checkIfUsed_WordSet(c_id, x) == 0) {
				enqueue(c_id, childDistance, parent, q);
				markUsed_WordSet(c_id, x);

			}

			c = c->next;
		}
	}

	// The goal was never reached, so there is no solution to hand back
	Free_IntLL(solution);
	gc->solution = NULL;

	// Cleanup BFS structures
	free_Queue(q);
	free_WordSet(x);
	return 0;
}

/* The board's own words first, and the rest of the dictionary only if there is
 * no route at all through them. A walk with a longer honest route is still a
 * walk; a walk with no route is not one, and refusing to deal it would be a
 * worse answer than quoting a number that needs a word off the list.
 */
void getSolution_FLWP(int id, int goalId, struct GameComponents* gc, struct DataStructures* data) {
	if(solve_FLWP(id, goalId, gc, 1, data)){
		return;
	}

	FLWG_LOG("No route to the goal through the words this board deals; measuring through the whole dictionary instead\n");
	solve_FLWP(id, goalId, gc, 0, data);
}
