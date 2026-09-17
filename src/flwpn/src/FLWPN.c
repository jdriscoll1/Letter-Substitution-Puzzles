/*************************
Name: Jordan D.
Purpose: The walk with more than one port of call.

************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/FLWPN.h"
#include "../../flwp/includes/BreadthFirstSearch_FLWP.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../flwp/includes/UserInput.h"
#include "../../structs/includes/IntLinkedList.h"
#include "../../structs/includes/WordSet.h"

/* How many goes at finding a port for one leg before the whole chain is given
up. A goal that lands on a port already in the chain is refused rather than
allowed, because a chain that visits the same word twice is a shorter chain
wearing a longer one's clothes. */
#define TRIES_PER_LEG 24

/*Whether this word is already a port somewhere in the chain*/
static int alreadyAPort(int id, const int* goals, int found){
	int i;
	for(i = 0; i < found; i++){
		if(goals[i] == id){
			return 1;
		}
	}
	return 0;
}

/*How long the shortest route from one word to another is, in moves*/
static int legLength(int from, int to, struct GameComponents* walk, struct DataStructures* data){
	getSolution_FLWP(from, to, walk, data);
	if(walk->solution == NULL){
		return 0;
	}
	/* The solution holds every word on the way including both ends, so the
	   number of MOVES is one fewer. */
	return walk->solution->size > 0 ? walk->solution->size - 1 : 0;
}

struct GameComponentsFLWPN* initiateFLWPN(int minAdjacenciesToStart, int maxAdjacenciesToStart,
	int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal,
	int numLegs, struct DataStructures* data){
	/* nothing to work with */
	if(data == NULL || numLegs < 1){
		return NULL;
	}

	/* The first leg is an ordinary board, dealt the ordinary way. Only the legs
	   after it are new.
	   
	   initiateFLWP RATHER THAN findFLWPStartAndGoal, and the difference is not
	   cosmetic: the dealer fills in four fields - start, goal, minConnections
	   and solution - and leaves the rest of the struct as whatever malloc
	   returned. The word list, the undo storage, the hint components and
	   prevInput are all finished afterwards by FinishGameComponents, which
	   initiateFLWP calls and the dealer does not. Built on the dealer alone
	   this segfaulted on the first move, in AddWord_Struct, reading a
	   userConnections that was never a pointer. */
	struct GameComponents* walk = initiateFLWP(minAdjacenciesToStart,
		maxAdjacenciesToStart, minDistance, maxDistance, minAdjacenciesToGoal,
		maxAdjacenciesToGoal, data);
	if(walk == NULL || walk->start == -1 || walk->goal == -1){
		if(walk != NULL){
			freeGameComponentsFLWP(walk, data);
		}
		return NULL;
	}

	int* goals = malloc(sizeof(int) * numLegs);
	if(goals == NULL){
		freeGameComponentsFLWP(walk, data);
		return NULL;
	}

	goals[0] = walk->goal;
	int found = 1;
	int total = legLength(walk->start, goals[0], walk, data);

	while(found < numLegs){
		int attempt, next = -1;
		for(attempt = 0; attempt < TRIES_PER_LEG && next == -1; attempt++){
			int candidate = chooseGoalBFS_FLWP(goals[found - 1], minDistance, maxDistance,
				minAdjacenciesToGoal, maxAdjacenciesToGoal, 1, data);
			/* NOT THE WORD THE WHOLE THING STARTED ON, and this one is load
			   bearing rather than tidy. A walk spends exactly one word - the
			   one it opens on, marked used in FinishGameComponents - and will
			   not let you return to it. A port that WAS the start would be a
			   port nobody could ever call at, on a board that looked perfectly
			   normal. Every other word may be revisited freely, which is why
			   nothing else here needs protecting.

			   Nor a port already in the chain: visiting the same word twice is
			   a shorter chain wearing a longer one's clothes. */
			if(candidate != -1 && candidate != walk->start
				&& !alreadyAPort(candidate, goals, found)){
				next = candidate;
			}
		}

		/* No port that far from the last one, or none that is not already in
		   the chain. The board is given up rather than shortened: a level that
		   asked for three ports and dealt two is not the level. */
		if(next == -1){
			free(goals);
			freeGameComponentsFLWP(walk, data);
			return NULL;
		}

		total += legLength(goals[found - 1], next, walk, data);
		goals[found] = next;
		found++;
	}

	struct GameComponentsFLWPN* gc = malloc(sizeof(struct GameComponentsFLWPN));
	if(gc == NULL){
		free(goals);
		freeGameComponentsFLWP(walk, data);
		return NULL;
	}

	gc->walk = walk;
	gc->goals = goals;
	gc->numLegs = numLegs;
	gc->legsDone = 0;
	gc->shortestRoute = total;

	/* The measuring above left the solution pointing at the last leg. The board
	   is about to be played from the first one, and the route hint reads this. */
	walk->goal = goals[0];
	getSolution_FLWP(walk->start, goals[0], walk, data);

	return gc;
}

struct GameComponents* getFLWPComponentsFLWPN(struct GameComponentsFLWPN* gc){
	/* nothing to work with */
	if(gc == NULL){
		return NULL;
	}

	return gc->walk;
}

int isStartValidFLWPN(struct GameComponentsFLWPN* gc){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return 0;
	}

	return gc->walk->start != -1 && gc->numLegs > 0;
}

int currentGoalFLWPN(struct GameComponentsFLWPN* gc){
	/* nothing to work with */
	if(gc == NULL || gc->legsDone >= gc->numLegs){
		return -1;
	}

	return gc->goals[gc->legsDone];
}

int legsDoneFLWPN(struct GameComponentsFLWPN* gc){
	return gc == NULL ? 0 : gc->legsDone;
}

int legsTotalFLWPN(struct GameComponentsFLWPN* gc){
	return gc == NULL ? 0 : gc->numLegs;
}

int shortestRouteFLWPN(struct GameComponentsFLWPN* gc){
	return gc == NULL ? 0 : gc->shortestRoute;
}

int userEntersWordFLWPN(char* userInput, struct GameComponentsFLWPN* gc,
	struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return -1;
	}

	int result = userEntersWord_FLWP(userInput, gc->walk, data);
	if(result != VALID){
		return result;
	}

	/* Arrived. The goal moves on rather than the game ending, until there is
	   nowhere left to move it to. */
	if(gc->walk->prevInput == gc->walk->goal && gc->legsDone < gc->numLegs){
		gc->legsDone++;
		if(gc->legsDone < gc->numLegs){
			gc->walk->goal = gc->goals[gc->legsDone];
			getSolution_FLWP(gc->walk->prevInput, gc->walk->goal, gc->walk, data);
		}
	}

	return VALID;
}

int isGameWonFLWPN(struct GameComponentsFLWPN* gc){
	/* nothing to work with */
	if(gc == NULL){
		return 0;
	}

	return gc->legsDone >= gc->numLegs;
}

void resetFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return;
	}

	ResetFLWP(gc->walk, data);
	gc->legsDone = 0;
	gc->walk->goal = gc->goals[0];
	getSolution_FLWP(gc->walk->start, gc->goals[0], gc->walk, data);
}

void freeGameComponentsFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL){
		return;
	}

	freeGameComponentsFLWP(gc->walk, data);
	free(gc->goals);
	free(gc);
}
