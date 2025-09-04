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
}; 

struct WordSet* convertCharPtrPtrToWordSet(char** words, struct DataStructures* data);
int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data);
// put a given word and all of its adjacencies (up to distance) into word set
void getSetOfSurroundingWords(int id, int distance, struct WordSet* wordSet, struct DataStructures* data); 
int isWordValid_StartWordParameters(int i, struct StartWordParametersFLWC p, struct DataStructures* data); 

#endif
