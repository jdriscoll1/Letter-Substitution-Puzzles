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
 * A PORT WALKED THROUGH EARLY IS STILL THERE LATER, and it is worth saying why
 * it needs no help to be. It looks as though it should: a walk refuses a word
 * it has already had, so passing through a later port on your way to an earlier
 * one ought to spend it and leave the board unfinishable.
 *
 * It does not, because a walk does not actually spend the words you play.
 * AddWord_Struct records the move and moves you on; the only word ever marked
 * used is the one the board STARTED on, in FinishGameComponents. Everything
 * else may be revisited freely. Checked rather than assumed - a probe played a
 * word, played away from it and played it again, and the walk took it.
 *
 * So the one thing FLWPN has to watch is that no port IS the start word, which
 * is the single word a walk will not let you return to. initiateFLWPN refuses
 * such a port, and there is a test that every port differs from the start.
 * There is also a test that walks through a later port early and then finishes
 * the board, so if a walk ever does start spending words this mode fails loudly
 * rather than dealing boards that cannot be won.
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

/* Back to the first port, with the whole route given up. */
void resetFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data);

void freeGameComponentsFLWPN(struct GameComponentsFLWPN* gc, struct DataStructures* data);

#endif
