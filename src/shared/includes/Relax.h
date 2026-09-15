#ifndef FLWG_RELAX_H
#define FLWG_RELAX_H

/* Asking for a board the dictionary might not have.
 *
 * Every mode is dealt a board by being handed numbers - how well connected the
 * word it opens on should be, how far from a goal, how far from something
 * forbidden - and then searching the dictionary for a word that satisfies all
 * of them at once. When no word does, the search used to return -1, the app
 * threw the board away and asked for another, and the numbers it was given the
 * second time were drawn from the same table as the first. A set of numbers
 * that nothing satisfies does not fail once. It fails every time.
 *
 * That is the wrong division of labour. The numbers are a description of the
 * board somebody wants; only the dictionary knows which boards exist. So the
 * numbers are treated here as a preference rather than a requirement: the
 * search asks for exactly what it was given, and if the dictionary has nothing
 * like that it asks again for something close, and again for something less
 * close, until it finds a board. The last round asks for anything at all.
 *
 * What is never relaxed is what makes a board a game rather than a board: the
 * word it opens on is not already a goal or a forbidden word, the goal can
 * actually be reached, and the player can force a win. Those are not
 * preferences and widening them would hand back a board that is unplayable
 * rather than merely not the one that was asked for.
 *
 * So a caller gets the board it described where one exists, the nearest thing
 * to it where one does not, and -1 only when no playable board exists at all -
 * which, with every preference given up, means the rules themselves are empty
 * rather than the request being unlucky.
 */

/* A range a search would like a number to fall in. */
struct Band {
	int min;
	int max;
};

/* How many times a band is asked for before it is given up entirely.
 *
 * Every round but the last widens by one on each side, so by the last real
 * round a band has grown by six in both directions - further than any of these
 * numbers ranges over the dictionaries the game ships, where the best
 * connected word has twenty four neighbours and nothing is more than a handful
 * of moves from anything else. The round after that asks for anything, which
 * is what makes the search terminate with an answer rather than with -1.
 */
#define RELAXATION_ROUNDS 8

/* The band to ask for on this round, counting from nought. */
struct Band loosen(struct Band asked, int round);

/* True once a round has given up on the band entirely, which is how a caller
   knows to stop applying it rather than to apply a very wide version of it -
   the difference matters where a search reads an empty band as "no constraint"
   and a wide one as "search the whole graph for something that far away". */
int isAnything(int round);

/* A band that admits every word, for a search that has no sentinel of its own
   and simply wants the test to pass. */
struct Band anyBand(void);

#endif
