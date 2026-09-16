#ifndef seenFLWCChallenges
#define seenFLWCChallenges

#include "../../structs/includes/WordSet.h"
#include "../../api/includes/FLWG-API.h"
#include "../../api/includes/FLWC-API.h"

struct StartWordParametersFLWC{
	// This is the set of goal words 
	struct WordSet* goalWords; 
	// This is the set of words to be avoided 
	struct WordSet* avoidWords; 
	
	// For example, it shouldn't be it's neighbor neighbor (2)
	int minGoalDistance; 
	// For example, it shouldn't be 7 away from any goal words
	int maxGoalDistance; 

	// For example, it shouldn't be an immediate neighbor to a word to be avoided
	int minAvoidDistance; 
	// For example, it shoulnd't be so far from an avoid word it's not even relevant 
	int maxAvoidDistance; 

	// It should at least have one connection, if not more
	int minAdjacencies; 
	// If it has too many connections it may be too easy 
	int maxAdjacencies; 	
	int minGoalAdjacencies; 	
	int maxGoalAdjacencies; 	
	// Number of Turns in the FLWC Game
	int numTurns; 
}; 

/* How near the nearest winning word may ever be dealt.
 *
 * Two, because one is not a board. A constraint game is dealt by looking for a
 * goal word a given distance off and opening on something that far from it -
 * but a player does not win by reaching that word. They win by reaching any
 * word the rule admits, and when the rule is a common one there is very often
 * a different one much nearer. The board is then over in a single move
 * whatever distance it was dealt at.
 *
 * Not a hypothetical. Asking for a word containing J, Q, X or Z three to five
 * moves away dealt VEAL, which is one letter from ZEAL; a third of the
 * openings that rule allowed had a winner one move off. The campaign's own
 * level 28 - reach a word that starts and ends with the same letter - was a
 * one move board in thirty-seven per cent of its openings.
 */
#define NEAREST_GOAL_ALLOWED 2

/* The fewest ways out a board may ever be opened on.
 *
 * The adjacency band is a preference and is given up a little at a time when
 * the dictionary has nothing like it, which is right - but the last round used
 * to give it up altogether, and a word with one neighbour then satisfied it as
 * well as a word with twenty.
 *
 * That is not a rare accident, it is what the remaining checks select for. The
 * two that are never relaxed both get easier the fewer moves a word has: a
 * board cannot deal a winner within two moves if almost nothing is within two
 * moves, and the player cannot be forced onto a forbidden word if there is
 * nowhere to be forced. So once adjacency stopped counting, the dead ends of
 * the graph were not merely allowed, they were the best candidates in it.
 *
 * DEMO is the example. Its only neighbour in the whole four letter dictionary
 * is MEMO, and MEMO's only neighbour is DEMO - a two word island where nothing
 * the rule forbids can be reached at all. Every never-relaxed check passes,
 * and the board it opens has exactly one move in it.
 *
 * Eight, because it is the lowest floor any board in the game deliberately
 * asks for - the arcade's rough seas - so this never argues with a request
 * that could have been met. It leaves 953 of the 1952 four letter words and
 * 348 of the 495 three letter ones to choose from, which is a wide enough pool
 * that boards stop repeating as well.
 */
#define FEWEST_WAYS_OUT 8

/* Whether the nearest word the rule admits is at least `least` moves off.
 *
 * Asked of every candidate and never relaxed, because this belongs with "the
 * word it opens on is not already a goal" rather than with the preferences: a
 * board won in one move is not a looser version of the board that was asked
 * for, it is not a board. */
int nearestGoalIsFarEnough(int id, int least, struct WordSet* goalWords, struct WordSet* avoidWords, struct DataStructures* data);

struct WordSet* convertCharPtrPtrToWordSet(char** words, struct DataStructures* data);
int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data);
// put a given word and all of its adjacencies (up to distance) into word set
void getSetOfSurroundingWords(int id, int distance, struct WordSet* wordSet, struct DataStructures* data); 
int isWordValid_StartWordParameters(int i, struct StartWordParametersFLWC p, struct DataStructures* data); 

#endif
