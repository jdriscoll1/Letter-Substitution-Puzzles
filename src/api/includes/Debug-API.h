/*************************
Name: Debug-API
Purpose: Start any game mode on a word you name, instead of one it picked.

Every mode normally chooses its own start word: it filters the dictionary by
adjacency count and distance, shuffles the survivors and takes the first that
passes. That is what you want in a game and the opposite of what you want when
you are working on one, because the position changes underneath you every run.

These entry points seat the word you ask for and set up everything else exactly
as the ordinary init does - the same components, freed by the same
freeGameComponents* - so a debug game and a real one differ only in how the
first word was arrived at.

They deliberately do NOT apply the parameter ranges the pickers use. If you ask
to start on a word with two adjacencies in a game that would normally demand
six, you get it: naming a word is the whole point. Validity is still reported
the usual way, through isStartValid*, and is false only when the request cannot
be honoured at all - the word is not in the dictionary, or, for a path game, no
goal can be reached from it.
************************/
#ifndef seenDebugAPI
#define seenDebugAPI

#include "FLWG-API.h"
#include "FLWC-API.h"
#include "FLWT-API.h"

/*FLWG - the two-player substitution game. Play passes until somebody cannot
move. isStartValidFLWG is false if the word is not in the dictionary.*/
struct GameData* initFLWGAtStart(char* startWord, struct DataStructures* data);

/*FLWP - the pathfinder, starting on the named word with a goal searched for
under the usual distance and adjacency limits. isStartValid_FLWP is false if the
word is unknown or nothing in range can be reached from it.*/
struct GameComponents* initFLWPAtStart(char* startWord, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data);

/*FLWP with both ends named. The solution is the shortest path between them.
isStartValid_FLWP is false if either word is unknown or no path joins them.*/
struct GameComponents* initFLWPBetween(char* startWord, char* goalWord, struct DataStructures* data);
/*A pathfinder board with no route at all: a start with room to move and a
goal on an island it cannot reach. solution is left NULL.*/
struct GameComponents* initFLWPUnreachable(int minAdjacencies, int maxAdjacencies, struct DataStructures* data);

/*FLWC - reach any goal word, or (with an empty goal list) avoid every avoid
word. Both lists are NULL-terminated, as initFLWC takes them.*/
struct GameComponentsFLWC* initFLWCAtStart(char* startWord, char** goalWords, char** avoidWords, struct DataStructures* data);

/*FLWGP - the generalized path game, starting on the named word. The goal is a
rule rather than a word, so the route is searched for from the named start the
way it would be from a picked one. isStartValid_FLWGP is false if the word is
unknown.*/
struct GameComponentsFLWGP* initFLWGPAtStart(char* startWord, char** goalWords, char** avoidWords, int minGoalDistance, struct DataStructures* data);

/*FLWT - the tutorial: find numTurns distinct adjacencies of the named word.
There is no maximum to pass: the word's own adjacency count is the ceiling.*/
struct GameComponentsFLWT* initFLWTAtStart(char* startWord, int numTurns, struct DataStructures* data);

#endif
