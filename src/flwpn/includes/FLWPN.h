/*************************
Name: Jordan D.
Purpose: The walk with more than one port of call.

************************/
#include "../../structs/includes/HashMap.h"
#include "../../api/includes/FLWG-API.h"
#include "../../flwp/includes/PathGameComponents.h"

#ifndef seenFLWPN
#define seenFLWPN

/* FLWPN: get from x to y to z, rather than from x to y.
 *
 * It is the ordinary walk with a list of goals instead of one, and that is not
 * a figure of speech - it HOLDS an ordinary walk and hands it out. Everything
 * the pathfinder already does keeps working on it untouched: undo and redo, the
 * route on the board, the double-tap rewind, how far the goal is, every hint.
 * The only thing FLWPN adds is what happens when you arrive - the goal moves on
 * rather than the game ending.
 *
 * ONCE A WORD IS TOUCHED IT IS SPENT. A D B E C is a walk; A D B D C is not,
 * because D was already had. That is the rule this mode is played under, and
 * three things follow from it that are not obvious.
 *
 * THE WALK DOES NOT ENFORCE IT ON ITS OWN. AddWord_Struct records a move and
 * moves you on; the only word ever marked used is the one the board STARTED on,
 * in FinishGameComponents. Checked rather than assumed - a probe played a word,
 * played away from it, and played it again, and the walk took it. So FLWPN
 * marks the path itself.
 *
 * IT IS DERIVED, NEVER TRACKED. After anything that changes the path - a move,
 * an undo, a redo, a reset - the word set is rebuilt from the path as it now
 * stands, and how many ports have been called at is counted off that same path.
 * Tracking either incrementally would mean undo had to put back exactly what
 * the move took, in the word set and in the leg count, and the pathfinder's undo
 * knows nothing about either. Deriving both costs one walk of a list a few words
 * long and cannot drift.
 *
 * A LATER PORT CANNOT BE TOUCHED EARLY. Under this rule, landing on C while
 * sailing for B would spend C, and the board could never be finished - it would
 * look perfectly healthy and be dead. So the move is refused, with PORT_NOT_DUE,
 * and the board says which port is actually due. That is a real restriction on
 * an otherwise free walk, and it is the only one: it buys a board that cannot
 * quietly become unwinnable.
 *
 * Which means a chain is only worth dealing if every leg can be walked without
 * touching a later port or a word already spent. initiateFLWPN walks the whole
 * chain that way before handing it over, and deals another when it cannot.
 */
struct GameComponentsFLWPN {
	/* An ordinary walk, with its goal set to whichever port is next. */
	struct GameComponents* walk;

	/* Every port, in the order they must be called at. */
	int* goals;
	int numLegs;
	int legsDone;

	/* The shortest route through all of them, leg by leg, added up. It is what
	   a run is marked against - marking a three-legged walk against one leg's
	   distance would make three stars unreachable, which is the bug the
	   pathfinder already had once against distanceToGoal. */
	int shortestRoute;
};

/* Deal a board. Every leg is dealt to the same distance band, from wherever the
last one ended. NULL when no chain that long could be built. */
struct GameComponentsFLWPN* initiateFLWPN(int minAdjacenciesToStart, int maxAdjacenciesToStart,
	int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal,
	int numLegs, struct DataStructures* data);

/* The walk inside, for everything the pathfinder already knows how to do. The
caller does not own it - freeGameComponentsFLWPN frees it. */
struct GameComponents* getFLWPComponentsFLWPN(struct GameComponentsFLWPN* gc);

int isStartValidFLWPN(struct GameComponentsFLWPN* gc);

/* Which port is being sailed for now, and how far through the chain this is. */
int currentGoalFLWPN(struct GameComponentsFLWPN* gc);
int legsDoneFLWPN(struct GameComponentsFLWPN* gc);
int legsTotalFLWPN(struct GameComponentsFLWPN* gc);
int shortestRouteFLWPN(struct GameComponentsFLWPN* gc);

/* Take a move. The ordinary walk's codes, and the ordinary walk's rules - with
one addition: arriving at the current port moves the goal on to the next one
rather than winning, until there is no next one. */
int userEntersWordFLWPN(char* userInput, struct GameComponentsFLWPN* gc,
	struct DataStructures* data);

int isGameWonFLWPN(struct GameComponentsFLWPN* gc);

/* Take a move back, or put it back. The pathfinder's own undo moves the path;
these make the spent words and the leg count agree with it again, which is why
this mode cannot simply use the walk's undo through getFLWPComponentsFLWPN. */
void undoMoveFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data);
void redoMoveFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data);

/* Back to the first port, with the whole route given up. */
void resetFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data);

void freeGameComponentsFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data);

#endif
