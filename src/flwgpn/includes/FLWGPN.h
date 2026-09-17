/*************************
Name: Jordan D.
Purpose: The walk whose ports are rules rather than words.

************************/
#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/WordSet.h"
#include "../../api/includes/FLWG-API.h"
#include "../../flwp/includes/PathGameComponents.h"

#ifndef seenFLWGPN
#define seenFLWGPN

/* FLWGPN: get to a word with OO, and then to a word ending in H.
 *
 * It is the chain (FLWPN) with the ports described instead of named. A leg is
 * finished by reaching ANY word the rule admits, not one particular word the
 * engine picked - which is the whole difference, and it is what makes the board
 * a puzzle about the rule rather than a puzzle about a word you were told.
 *
 * Like the chain, it HOLDS an ordinary walk and hands it out, so undo, redo,
 * the route on the board, the double-tap rewind, how far the goal is and every
 * hint are the pathfinder's untouched.
 *
 * FOUR THINGS FOLLOW FROM PORTS BEING SETS, and three of them are things the
 * chain needs and this does not.
 *
 * NO WORD IS SPENT. The chain spends every word on the path because its ports
 * are single words: wander onto the third port while sailing for the first and
 * that port is gone, and the board is dead while looking perfectly healthy. A
 * SET survives having a member touched - there are others - so there is nothing
 * here to protect and the rule is dropped. This is an ordinary walk.
 *
 * AND NO PORT IS FORBIDDEN EARLY. PORT_NOT_DUE exists for the same reason and
 * goes for the same reason. Over a set it would also be perverse: "a word with
 * OO" can be two hundred words, and refusing every one of them until its leg
 * came due would fence off a quarter of the dictionary with no way for the
 * player to see the fence.
 *
 * ONE WORD ANSWERS ONE PORT. Sets overlap - OOZH is a word with OO and a word
 * ending in H - so a word could answer two legs at once. It does not: the count
 * walks the path and advances at most one leg per word, so a word settles the
 * EARLIEST port it can and the next leg starts from it. A player standing on a
 * word that would also answer the next rule still has to move.
 *
 * THE WALK STILL NEEDS A WORD TO AIM AT. distanceToGoal, the route hint and the
 * star marking all read walk->goal, and "a word with OO" is not a word. So the
 * goal is re-aimed after every move at the NEAREST member of the due set from
 * wherever the player now stands. That keeps the distance on the board honest -
 * pinning it to whichever member the dealer happened to pick would show a
 * player three moves from a rule they are one move from satisfying.
 *
 * WHAT IS MARKED AGAINST is the dealer's own chain, added up leg by leg, the
 * way the chain marks its own. A player who finishes on a different member of
 * the set than the dealer found may beat it; that is the right way round.
 */
struct GameComponentsFLWGPN {
	/* An ordinary walk, with its goal re-aimed at the nearest word the due
	   rule admits. */
	struct GameComponents* walk;

	/* One set per leg, in the order they must be satisfied. Owned. */
	struct WordSet** goals;
	int numLegs;
	int legsDone;

	/* The shortest route through all of them, leg by leg, added up - what a run
	   is marked against. Marking a two-legged walk against one leg's distance
	   would put three stars out of reach, which the pathfinder has already done
	   once against distanceToGoal. */
	int shortestRoute;
};

/* Deal a board. Each element of goalSets is its own NULL-terminated list of the
words that leg's rule admits; numLegs says how many there are. Every leg is
dealt to the same distance band, measured from wherever the last one ended.
NULL when no chain that long could be built. */
struct GameComponentsFLWGPN* initiateFLWGPN(int minAdjacenciesToStart,
	int maxAdjacenciesToStart, char*** goalSets, int numLegs, int minDistance,
	int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal,
	struct DataStructures* data);

/* The walk inside, for everything the pathfinder already knows how to do. The
caller does not own it - freeGameComponentsFLWGPN frees it. */
struct GameComponents* getFLWPComponentsFLWGPN(struct GameComponentsFLWGPN* gc);

int isStartValidFLWGPN(struct GameComponentsFLWGPN* gc);

/* How far through the chain this is, and how long the whole thing is. */
int legsDoneFLWGPN(struct GameComponentsFLWGPN* gc);
int legsTotalFLWGPN(struct GameComponentsFLWGPN* gc);
int shortestRouteFLWGPN(struct GameComponentsFLWGPN* gc);

/* A word the due rule admits, nearest first - what the board aims at and what
the route hint follows. -1 when the chain is finished or none can be reached. */
int currentGoalFLWGPN(struct GameComponentsFLWGPN* gc);

/* Whether a word satisfies the rule that is due now. The screen asks so it can
say "that is one of them" without knowing what the rule is. */
int satisfiesCurrentRuleFLWGPN(int id, struct GameComponentsFLWGPN* gc);

/* Take a move. The ordinary walk's codes and the ordinary walk's rules, with
one addition: a word the due rule admits moves the rule on to the next one
rather than winning, until there is no next one. */
int userEntersWordFLWGPN(char* userInput, struct GameComponentsFLWGPN* gc,
	struct DataStructures* data);

int isGameWonFLWGPN(struct GameComponentsFLWGPN* gc);

/* Take a move back, or put it back. The pathfinder's own undo moves the path;
these make the leg count and the aim agree with it again, which is why this mode
cannot simply use the walk's undo through getFLWPComponentsFLWGPN. */
void undoMoveFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data);
void redoMoveFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data);

/* Back to the first rule, with the whole route given up. */
void resetFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data);

void freeGameComponentsFLWGPN(struct GameComponentsFLWGPN* gc, struct DataStructures* data);

#endif
