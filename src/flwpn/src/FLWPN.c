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

/* The path as it stands right now.
 *
 * NOT userConnections, which is the obvious place and is wrong after an undo.
 * Undo_Struct steps the storage back and rebuilds what is drawn from the
 * snapshot at the front of it; it never shortens userConnections, so that list
 * still holds the move that was just taken back. Reading it made undo appear to
 * do nothing at all here - the word stayed spent and the port stayed called at.
 *
 * This is the same place CopyWordLLOntoArrayList reads, which is what puts the
 * route on the board, so what is spent always agrees with what the player can
 * see. */
static struct intList* currentPath(struct GameComponentsFLWPN* gc){
	return (struct intList*)gc->walk->storage->next->listHeader;
}

/* The rule, applied to the path as it now stands.
 *
 * Every word on the path is spent and nothing else is. Rebuilt from scratch
 * rather than adjusted, so that undo, redo, a rewind to a word in the middle,
 * and a reset all come out right without any of them knowing this exists - the
 * pathfinder's undo rebuilds the path and has never heard of a word set.
 *
 * Resetting the whole set is safe because only one game is ever live: the module
 * that owns these pointers frees every other mode before it builds one.
 */
static void spendExactlyThePath(struct GameComponentsFLWPN* gc, struct DataStructures* data){
	resetWordSet(data);

	struct intList* w = currentPath(gc);
	for(w = w->next; w != NULL; w = w->next){
		markUsed_WordSet(w->data, data->wordSet);
	}
}

/* How many ports have been called at, read off the path in order.
 *
 * Derived for the same reason the word set is: an undo that steps back over an
 * arrival has to un-call that port, and a counter bumped on the way past would
 * have to be decremented by something that knows an arrival happened. The path
 * already says.
 */
static int portsCalledAt(struct GameComponentsFLWPN* gc){
	int reached = 0;
	struct intList* w = currentPath(gc);

	for(w = w->next; w != NULL && reached < gc->numLegs; w = w->next){
		if(w->data == gc->goals[reached]){
			reached++;
		}
	}
	return reached;
}

/*Defined below; named here because taking a move uses it before then*/
static void relayTheGoal(struct GameComponentsFLWPN* gc, struct DataStructures* data);

/*Whether this word is a port that is not the one being sailed for*/
static int isAPortNotYetDue(int id, struct GameComponentsFLWPN* gc){
	int i;
	for(i = gc->legsDone + 1; i < gc->numLegs; i++){
		if(gc->goals[i] == id){
			return 1;
		}
	}
	return 0;
}

/* Whether the whole chain can actually be walked under this mode's rule, which
 * is stricter than the graph's.
 *
 * A leg has to be walkable without touching a later port and without reusing a
 * word already spent on an earlier leg. Neither the dealer nor chooseGoalBFS
 * knows that - they measure distance through the open graph - so a chain that
 * looks fine can have a leg whose only routes run through a port that is not
 * due yet. Walking it here is the difference between refusing such a board and
 * dealing one that cannot be finished.
 */
static int chainCanBeWalked(const int* goals, int numLegs, int start,
	struct DataStructures* data){
	int total = data->I2W->numWords;
	int* spent = calloc(total, sizeof(int));
	int* seen = malloc(sizeof(int) * total);
	int* queue = malloc(sizeof(int) * total);
	int* cameFrom = malloc(sizeof(int) * total);
	if(spent == NULL || seen == NULL || queue == NULL || cameFrom == NULL){
		free(spent); free(seen); free(queue); free(cameFrom);
		return 0;
	}

	spent[start] = 1;
	int from = start, leg, ok = 1;

	for(leg = 0; leg < numLegs && ok; leg++){
		int to = goals[leg];
		int i, head = 0, tail = 0;
		for(i = 0; i < total; i++){
			seen[i] = 0;
			cameFrom[i] = -1;
		}
		seen[from] = 1;
		queue[tail++] = from;
		int found = 0;

		while(head < tail && !found){
			int curr = queue[head++];
			struct intList* c = getConnections(curr, data->I2W);
			for(c = c->next; c != NULL; c = c->next){
				int next = c->data;
				if(seen[next]){
					continue;
				}
				/*a word already spent on an earlier leg is gone for good*/
				if(spent[next] && next != to){
					continue;
				}
				/*and a port that is not due may not be touched at all*/
				int laterPort = 0, j;
				for(j = leg + 1; j < numLegs; j++){
					if(goals[j] == next){ laterPort = 1; break; }
				}
				if(laterPort){
					continue;
				}
				seen[next] = 1;
				cameFrom[next] = curr;
				if(next == to){ found = 1; break; }
				queue[tail++] = next;
			}
		}

		if(!found){
			ok = 0;
			break;
		}

		/*spend the route this leg took, so the next leg has to go around it*/
		int at = to;
		while(at != -1){
			spent[at] = 1;
			at = cameFrom[at];
		}
		from = to;
	}

	free(spent); free(seen); free(queue); free(cameFrom);
	return ok;
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

	/* Walkable under THIS mode's rule, not just connected in the graph. A chain
	   whose second leg can only be reached through its third port is a board
	   that cannot be finished, and nothing before this point would have noticed.
	   Given up rather than dealt; the caller deals another. */
	if(!chainCanBeWalked(goals, numLegs, walk->start, data)){
		free(goals);
		freeGameComponentsFLWP(walk, data);
		return NULL;
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

	spendExactlyThePath(gc, data);
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

	/* A port that is not the one being sailed for is refused before the walk
	   ever sees it. Touching it would spend it - see the note in FLWPN.h - and
	   the board would be dead while looking perfectly healthy. */
	int asked = convertWordToInt(userInput, data);
	if(asked >= 0 && isAPortNotYetDue(asked, gc)){
		return PORT_NOT_DUE;
	}

	int result = userEntersWord_FLWP(userInput, gc->walk, data);
	if(result != VALID){
		return result;
	}

	/* Both read off the path rather than counted, so that undo and redo need
	   know nothing about either. */
	spendExactlyThePath(gc, data);
	relayTheGoal(gc, data);
	return VALID;
}

/* Put the goal wherever the path says it should be, and work out the route to
   it again. Called after anything that can change the path. */
static void relayTheGoal(struct GameComponentsFLWPN* gc, struct DataStructures* data){
	gc->legsDone = portsCalledAt(gc);
	if(gc->legsDone < gc->numLegs){
		gc->walk->goal = gc->goals[gc->legsDone];
		getSolution_FLWP(gc->walk->prevInput, gc->walk->goal, gc->walk, data);
	}
}

/* Take a move back, and put everything the rule depends on back with it. The
   pathfinder's own undo is what moves the path; this is what makes the word set
   and the leg count agree with it again. */
void undoMoveFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return;
	}

	undoMoveFLWP(gc->walk, data);
	spendExactlyThePath(gc, data);
	relayTheGoal(gc, data);
}

void redoMoveFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return;
	}

	redoMoveFLWP(gc->walk, data);
	spendExactlyThePath(gc, data);
	relayTheGoal(gc, data);
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
	spendExactlyThePath(gc, data);
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
