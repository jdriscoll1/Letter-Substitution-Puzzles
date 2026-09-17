/*************************
Name: Jordan D.
Purpose: The walk whose ports are rules rather than words.

************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/FLWGPN.h"
#include "../../flwc/includes/Challenges.h"
#include "../../flwp/includes/BreadthFirstSearch_FLWP.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../flwp/includes/UserInput.h"
#include "../../structs/includes/IntLinkedList.h"
#include "../../structs/includes/WordSet.h"

/* How many whole boards are tried before the deal is given up. A board fails
when a leg's rule has nothing in the distance band from where the last leg
ended, which depends entirely on where the start landed. */
#define TRIES_PER_BOARD 40

/* The path as it stands right now.
 *
 * NOT userConnections. Undo_Struct steps the storage back and rebuilds what is
 * drawn from the snapshot at the front of it; it never shortens
 * userConnections, so that list still holds the move just taken back. The chain
 * learned this the hard way - reading it made undo appear to do nothing at all.
 * This is the same place the board reads, so what the count says always agrees
 * with what the player can see. */
static struct intList* currentPath(struct GameComponentsFLWGPN* gc){
	return (struct intList*)gc->walk->storage->next->listHeader;
}

/* How many rules have been satisfied, read off the path in order.
 *
 * Derived rather than counted, for the reason the chain derives its own: an
 * undo that steps back over an arrival has to un-satisfy that rule, and a
 * counter bumped on the way past would need something that knows an arrival
 * happened to put it back. The path already says.
 *
 * AT MOST ONE LEG PER WORD, which is where "one word answers one port" lives -
 * see the note in FLWGPN.h. The loop only ever asks whether this word satisfies
 * the rule that is due, so a word in two sets settles the earlier one. */
static int rulesSatisfied(struct GameComponentsFLWGPN* gc){
	int reached = 0;
	struct intList* w = currentPath(gc);

	for(w = w->next; w != NULL && reached < gc->numLegs; w = w->next){
		if(checkIfUsed_WordSet(w->data, gc->goals[reached])){
			reached++;
		}
	}
	return reached;
}

/* The nearest word the given rule admits, from `from`.
 *
 * A plain breadth first walk of the graph, stopping at the first member it
 * meets. Written here rather than reached for because the searches that exist
 * answer a different question - chooseGoalBFS_FLWP picks a word in a distance
 * BAND and knows nothing about sets, and the FLWC path search wants a whole
 * StartWordParameters to ask.
 *
 * `atLeast` is how far away it has to be, which is how the dealer uses this to
 * find a port rather than the nearest one: 0 means "wherever it is". -1 when
 * the rule admits nothing that can be reached at all.
 *
 * It measures through the WHOLE dictionary on purpose. The player may type
 * anything, so the distance shown on the board has to be the distance they can
 * actually walk, not the distance through the words this board would deal.
 */
static int nearestInSet(int from, struct WordSet* set, int atLeast, int atMost,
	int* howFar, struct DataStructures* data){
	int total = data->I2W->numWords;
	int* seen = calloc(total, sizeof(int));
	int* depth = malloc(sizeof(int) * total);
	int* queue = malloc(sizeof(int) * total);
	if(seen == NULL || depth == NULL || queue == NULL){
		free(seen); free(depth); free(queue);
		return -1;
	}

	int head = 0, tail = 0, found = -1;
	seen[from] = 1;
	depth[from] = 0;
	queue[tail++] = from;

	if(atLeast <= 0 && checkIfUsed_WordSet(from, set) && !isTooObscure(from, data)
		&& !isOffLimits(from, data)){
		found = from;
	}

	while(head < tail && found == -1){
		int curr = queue[head++];
		if(atMost > 0 && depth[curr] >= atMost){
			continue;
		}
		struct intList* c = getConnections(curr, data->I2W);
		for(c = c->next; c != NULL && found == -1; c = c->next){
			int next = c->data;
			if(seen[next]){
				continue;
			}
			seen[next] = 1;
			depth[next] = depth[curr] + 1;
			/* IN THE LEVEL'S OWN VOCABULARY, both what is walked through and
			   what is arrived at. The rule admits every word in the dictionary
			   that fits it - that is what the PLAYER is judged against and it
			   never narrows - but what the board aims at, measures itself by
			   and hints towards has to be a word this level would deal. A board
			   that says "get to a word with OO" and means ZOOT is asking for
			   something outside its own reach. */
			if(isTooObscure(next, data) || isOffLimits(next, data)){
				continue;
			}
			if(depth[next] >= atLeast && checkIfUsed_WordSet(next, set)){
				found = next;
				break;
			}
			queue[tail++] = next;
		}
	}

	if(found != -1 && howFar != NULL){
		*howFar = depth[found];
	}

	free(seen); free(depth); free(queue);
	return found;
}

/* A word this leg's rule admits that sits in the distance band, or -1.
 *
 * The band is asked for from the near end: the first member at or past
 * minDistance, refused if it is past maxDistance. That gives the SHORTEST leg
 * the rule allows, which is the honest one to measure the board by - a player
 * may always find a nearer member than the dealer did, and cannot find a nearer
 * one than this. */
static int portForLeg(int from, struct WordSet* set, int minDistance, int maxDistance,
	int* howFar, struct DataStructures* data){
	int at = -1;
	int found = nearestInSet(from, set, minDistance, maxDistance, &at, data);
	if(found == -1){
		return -1;
	}
	if(maxDistance > 0 && at > maxDistance){
		return -1;
	}
	if(howFar != NULL){
		*howFar = at;
	}
	return found;
}

/*Defined below; named here because taking a move uses it before then*/
static void reAimTheGoal(struct GameComponentsFLWGPN* gc, struct DataStructures* data);

/*Hand back every set built so far, and the array holding them*/
static void freeTheSets(struct WordSet** goals, int built){
	int i;
	if(goals == NULL){
		return;
	}
	for(i = 0; i < built; i++){
		free_WordSet(goals[i]);
	}
	free(goals);
}

struct GameComponentsFLWGPN* initiateFLWGPN(int minAdjacenciesToStart,
	int maxAdjacenciesToStart, char*** goalSets, int numLegs, int minDistance,
	int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal,
	struct DataStructures* data){
	/* nothing to work with */
	if(data == NULL || goalSets == NULL || numLegs < 1){
		return NULL;
	}

	/* The sets are built once and kept for the life of the board: they are the
	   rules, and they do not change as it is played. */
	struct WordSet** goals = malloc(sizeof(struct WordSet*) * numLegs);
	if(goals == NULL){
		return NULL;
	}

	int leg;
	for(leg = 0; leg < numLegs; leg++){
		if(goalSets[leg] == NULL){
			freeTheSets(goals, leg);
			return NULL;
		}
		goals[leg] = convertCharPtrPtrToWordSet(goalSets[leg], data);
		if(goals[leg] == NULL){
			freeTheSets(goals, leg);
			return NULL;
		}
	}

	/* A WHOLE BOARD AT A TIME, rather than a leg at a time with retries.
	 *
	 * Whether a leg can be dealt depends on where the leg before it ended, and
	 * that on where the start landed, so a chain that fails on its last leg
	 * fails because of a choice made at the beginning. Backtracking one leg
	 * would spend its tries re-drawing the one choice that was not the problem.
	 *
	 * initiateFLWP RATHER THAN findFLWPStartAndGoal, for the reason the chain
	 * gives at length: the dealer fills in four fields and leaves the rest of
	 * the struct as whatever malloc returned. FinishGameComponents is what
	 * makes the undo storage and the word list real, and initiateFLWP is what
	 * calls it.
	 */
	int attempt;
	for(attempt = 0; attempt < TRIES_PER_BOARD; attempt++){
		struct GameComponents* walk = initiateFLWP(minAdjacenciesToStart,
			maxAdjacenciesToStart, minDistance, maxDistance, minAdjacenciesToGoal,
			maxAdjacenciesToGoal, data);
		if(walk == NULL || walk->start == -1){
			if(walk != NULL){
				freeGameComponentsFLWP(walk, data);
			}
			continue;
		}

		/* A START THAT ALREADY SATISFIES THE FIRST RULE IS NOT A BOARD.
		   The count is read off the path, and the path begins at the word the
		   board opened on, so such a board would open with its first leg
		   already finished - "get to a word with OO" answered before the player
		   had done anything. Dealt again instead. */
		if(checkIfUsed_WordSet(walk->start, goals[0])){
			freeGameComponentsFLWP(walk, data);
			continue;
		}

		/* The walk chose its own goal, which is a word rather than a rule. It
		   is replaced leg by leg below; what was worth having is the start and
		   everything FinishGameComponents built under it. */
		int from = walk->start, total = 0, ok = 1;
		for(leg = 0; leg < numLegs && ok; leg++){
			int howFar = 0;
			int port = portForLeg(from, goals[leg], minDistance, maxDistance,
				&howFar, data);
			if(port == -1){
				ok = 0;
				break;
			}
			total += howFar;
			from = port;
		}

		if(!ok){
			freeGameComponentsFLWP(walk, data);
			continue;
		}

		struct GameComponentsFLWGPN* gc = malloc(sizeof(struct GameComponentsFLWGPN));
		if(gc == NULL){
			freeGameComponentsFLWP(walk, data);
			freeTheSets(goals, numLegs);
			return NULL;
		}

		gc->walk = walk;
		gc->goals = goals;
		gc->numLegs = numLegs;
		gc->legsDone = 0;
		gc->shortestRoute = total;

		reAimTheGoal(gc, data);
		return gc;
	}

	freeTheSets(goals, numLegs);
	return NULL;
}

struct GameComponents* getFLWPComponentsFLWGPN(struct GameComponentsFLWGPN* gc){
	/* nothing to work with */
	if(gc == NULL){
		return NULL;
	}

	return gc->walk;
}

int isStartValidFLWGPN(struct GameComponentsFLWGPN* gc){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return 0;
	}

	return gc->walk->start != -1 && gc->numLegs > 0;
}

int legsDoneFLWGPN(struct GameComponentsFLWGPN* gc){
	return gc == NULL ? 0 : gc->legsDone;
}

int legsTotalFLWGPN(struct GameComponentsFLWGPN* gc){
	return gc == NULL ? 0 : gc->numLegs;
}

int shortestRouteFLWGPN(struct GameComponentsFLWGPN* gc){
	return gc == NULL ? 0 : gc->shortestRoute;
}

int currentGoalFLWGPN(struct GameComponentsFLWGPN* gc){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL || gc->legsDone >= gc->numLegs){
		return -1;
	}

	return gc->walk->goal;
}

int satisfiesCurrentRuleFLWGPN(int id, struct GameComponentsFLWGPN* gc){
	/* nothing to work with */
	if(gc == NULL || id < 0 || gc->legsDone >= gc->numLegs){
		return 0;
	}

	return checkIfUsed_WordSet(id, gc->goals[gc->legsDone]) != 0;
}

/* Put the aim wherever the path says it should be, and work the route out
   again. Called after anything that can change the path.

   RE-AIMED RATHER THAN SET ONCE: the goal is the nearest word the due rule
   admits FROM WHERE THE PLAYER IS, so the distance the board shows is the
   distance they actually have left. */
static void reAimTheGoal(struct GameComponentsFLWGPN* gc, struct DataStructures* data){
	gc->legsDone = rulesSatisfied(gc);
	if(gc->legsDone >= gc->numLegs){
		return;
	}

	/* A MOVE AWAY AT LEAST, never the word already under your feet.
	 *
	 * The word you are standing on can satisfy the rule that is due and still
	 * not count, and that is not an edge case - it is what "one word answers
	 * one port" MEANS. Reach a word with OO when the next rule is also about OO
	 * and you have answered the first rule with it; the second still wants a
	 * word, and it cannot be that one. Aimed at where you already are, the
	 * board would show a goal of nought moves that finishing never reached, and
	 * a route hint with nothing in it. */
	int at = gc->walk->prevInput >= 0 ? gc->walk->prevInput : gc->walk->start;
	int aim = nearestInSet(at, gc->goals[gc->legsDone], 1, 0, NULL, data);
	if(aim == -1){
		return;
	}

	gc->walk->goal = aim;
	getSolution_FLWP(at, aim, gc->walk, data);
}

int userEntersWordFLWGPN(char* userInput, struct GameComponentsFLWGPN* gc,
	struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return -1;
	}

	/* Nothing is refused that the walk itself would take. A rule that is not
	   due yet fences off nothing - see the note in FLWGPN.h. */
	int result = userEntersWord_FLWP(userInput, gc->walk, data);
	if(result != VALID){
		return result;
	}

	reAimTheGoal(gc, data);
	return VALID;
}

void undoMoveFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return;
	}

	undoMoveFLWP(gc->walk, data);
	reAimTheGoal(gc, data);
}

void redoMoveFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return;
	}

	redoMoveFLWP(gc->walk, data);
	reAimTheGoal(gc, data);
}

int isGameWonFLWGPN(struct GameComponentsFLWGPN* gc){
	/* nothing to work with */
	if(gc == NULL){
		return 0;
	}

	return gc->legsDone >= gc->numLegs;
}

void resetFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || gc->walk == NULL){
		return;
	}

	ResetFLWP(gc->walk, data);
	gc->legsDone = 0;
	reAimTheGoal(gc, data);
}

void freeGameComponentsFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL){
		return;
	}

	freeGameComponentsFLWP(gc->walk, data);
	freeTheSets(gc->goals, gc->numLegs);
	free(gc);
}
