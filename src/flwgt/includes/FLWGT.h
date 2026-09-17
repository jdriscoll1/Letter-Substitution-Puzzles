/*************************
Name: Jordan D.
Purpose: The generalized turns game - name words at a given distance from a set.

************************/
#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/WordSet.h"
#include "../../api/includes/FLWG-API.h"

#ifndef seenFLWGT
#define seenFLWGT

/* FLWGT: name N words that sit a given distance from a set of words.
 *
 * Every other mode in this game is a WALK. A board is dealt, the player stands
 * on a word, and every move has to be one letter off the last one. This is not
 * that. Nothing is dealt, the player stands nowhere, and the words they type
 * have nothing to do with each other - each one is judged on its own, against
 * the rule the level states.
 *
 *   "Name 3 words exactly 2 from TEST"
 *   "Name a word exactly 3 from both PIES and TEST"
 *   "Name 3 words exactly 2 from any word with OO in it"
 *
 * Those look like three modes and are one. What differs is only WHICH WORDS
 * THE DISTANCE IS MEASURED FROM - one word, several, or every word matching a
 * rule - and the engine never has to know which of those it was given. It is
 * handed a set of words and measures from all of them at once. Rules stay in
 * JS, where FLWC already turns a predicate over the whole dictionary into an
 * array of words.
 *
 * So the whole mode is four numbers and a set: the seed words, how near, how
 * far, and how many answers the level wants.
 */
struct GameComponentsFLWGT {
	/* How far every word in the dictionary is from the seed set, or -1 for one
	   the seeds cannot reach at all. Worked out once, when the board is built.

	   IT IS A FACT ABOUT THE GRAPH AND NOTHING ELSE. The words the player has
	   already named are held in the word set, separately, so that naming a word
	   can never move another word further away. A distance that changed as the
	   game was played would make the rule the level states stop being true
	   halfway through it. */
	int* distance;
	int numWords;

	/* The rule, as a band. "Exactly two" is 2 and 2; "within two" is 0 and 2.
	   A band rather than a single number because this codebase already asks its
	   distance questions that way, and because it costs nothing to let a level
	   say either. */
	int minDistance;
	int maxDistance;

	/* How many the level wants, and how many have been named. */
	int wordsWanted;
	int wordsFound;

	/* How many words satisfy the rule at all, out of the whole dictionary, and
	   how many of those the game would be willing to deal.

	   The second number is the one that says whether a level is fair. A board
	   asking for three words when the only three answers are ZOUK, WABS and
	   GIBS is not a hard board, it is a broken one - see isTooObscure. The
	   player may still type any of them; this is about whether the level should
	   have been built. */
	int answersTotal;
	int answersWorthAsking;
};

/* Distance from every word to the nearest of the seeds, breadth first from all
of them at once. -1 where the seeds cannot reach. The caller owns the array. */
int* distancesFromSeeds(const int* seeds, int numSeeds, struct DataStructures* data);

/* Build a board. Seeds are word ids; anything not in the dictionary is ignored,
and a board with no usable seed at all comes back NULL. */
struct GameComponentsFLWGT* initFLWGT(const int* seeds, int numSeeds, int minDistance,
	int maxDistance, int wordsWanted, struct DataStructures* data);

/* Whether this board can be finished with words the game would be prepared to
use itself. A level that fails this should not be dealt. */
int isSolvableFLWGT(struct GameComponentsFLWGT* gc);

/* How far a word is from the seed set: 0 for a seed, -1 for a word nothing can
reach, and -2 for a word that is not in the dictionary. It is what the board
says out loud when an answer is refused - "BEST is 1 away, not 2" teaches the
rule, where "wrong" only teaches that you were wrong. */
int distanceOfWordFLWGT(const char* word, struct GameComponentsFLWGT* gc,
	struct DataStructures* data);

/* Take an answer. VALID when it counted, and an ERROR_CODE when it did not.
WRONG_DISTANCE is the one this mode adds; the others mean what they always do. */
int userEntersWordFLWGT(const char* word, struct GameComponentsFLWGT* gc,
	struct DataStructures* data);

int isGameWonFLWGT(struct GameComponentsFLWGT* gc);
int wordsFoundFLWGT(struct GameComponentsFLWGT* gc);
int wordsWantedFLWGT(struct GameComponentsFLWGT* gc);

/* How many answers are still out there, of the ones the game would deal. It is
the count hint, and it is also how a level knows it has not become impossible
because the player spent the only three answers on their first three goes. */
int answersLeftFLWGT(struct GameComponentsFLWGT* gc, struct DataStructures* data);

/* One answer that has not been named yet, commonest first, or -1. The cap
applies here because this is the engine choosing a word to put in front of
somebody - the same reason every other hint sorts this way. */
int anAnswerFLWGT(struct GameComponentsFLWGT* gc, struct DataStructures* data);

void freeGameComponentsFLWGT(struct GameComponentsFLWGT* gc);

#endif
