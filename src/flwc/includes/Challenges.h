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
