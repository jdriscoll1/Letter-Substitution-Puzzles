/*************************
Name: Jordan D. Andrew B.
Date: Aug 8, 2024
Purpose: A library to encapsulate & organize the code into an API

************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/TreeSet.h"
#include "../../structs/includes/WordSet.h"

#ifndef seenFLWGAPI
#define seenFLWGAPI


// Some things that need to be done
// Convert Word To Int 
struct DataStructures {
	// The Word to Int Map 
	struct DummyHeadNode*** W2I; 
	// The Int to Word Map
	struct wordDataArray* I2W; 
	// The Word Set
	struct WordSet* wordSet;
	/* How obscure a word the engine may DEAL OR PLAY on this board - see
	wordData.obscurity. A word past this is still perfectly legal for the player
	to type; it is the game that must not use it, because being beaten by a word
	nobody has heard of is not being beaten.

	OBSCURITY_UNKNOWN means no restriction, which is what every caller gets
	until it says otherwise - so a dictionary loaded and left alone behaves
	exactly as it did before any of this existed. */
	int obscurityCap;
}; 

struct GameData {
	int currWordId; 
	int numPlayers; 
	int difficulty; 
	int goalWord; 
};

struct GameComponentsFLWGP{
	struct GameComponents* flwpComponents; 
	struct GameComponentsFLWC* flwcComponents; 

}; 

// Shared behavior
struct DataStructures* initDataStructures(int fd, int numLetters);

/*The hardest word the engine may use from here on. Set once as a board is
built, the way the adjacency bounds are. Anything at or below the cap may be
dealt and played; anything above it belongs to the player alone.*/
void setObscurityCap(struct DataStructures* data, int cap);

/*Whether a word is past that line. Asked wherever the engine is choosing a word
for itself - never where it is judging one the player typed.*/
int isTooObscure(int id, struct DataStructures* data);
/*Fix the sequence of random choices, so the same seed deals the same board.
initDataStructures seeds from the clock once at startup; this overrides that
from the point it is called, which is what lets a puzzle be the same puzzle for
everybody on a given day - the start word, the bot's replies and all.*/
void seedGameRandom(unsigned int seed); 

// flwg behavior
void freeDataStructures(struct DataStructures* dataStructures); 
struct GameData* initFLWG(struct DataStructures* dataStructures, int minAdjacencies, int maxAdjacencies);
char* getCurrWord(struct GameData* gameData, struct DataStructures* dataStructures); 
int botTakesTurn(struct GameData* gameData, struct DataStructures* data, int botType);
int userTakesTurn(char* userInput, struct GameData* gameData, struct DataStructures* dataStructures); 
void resetWordSet(struct DataStructures* dataStructures);
int startGameReturnFirstWord(struct DataStructures *dataStructures);
void freeGameComponentsFLWG(struct GameData* gameData);
int isStartValidFLWG(struct GameData* gameData);

// flwp behavior
struct GameComponents* initiateFLWP(int minAdjacenciesToStart, int maxAdjacenciesToStart, int minDistance, int maxDistance, int minAdjacenciesToGoal, int maxAdjacenciesToGoal, struct DataStructures* data);
void ResetFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures); 
int userEntersWord_FLWP(char* userInput, struct GameComponents *gameComponents, struct DataStructures* dataStructures); 
void removeWord_FLWP(char* word, struct GameComponents *gameComponents, struct DataStructures* dataStructures);
int isStartValid_FLWP(struct GameComponents *gameComponents); 
char* getStartWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
char* getGoalWordFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
int getPrevWordFLWP(struct GameComponents* gameComponents);
int isGameWonFLWP(struct GameComponents *gameComponents);
void undoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
void redoMoveFLWP(struct GameComponents *gameComponents, struct DataStructures* dataStructures);
struct arrayList *getCurrentWordsFLWP(struct GameComponents *gameComponents);
void freeGameComponentsFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures);
char* hintGetHeadAdjacencyFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures); 
char* hintGetTailAdjacencyFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures); 
int hintGetMinAdjacenciesFLWP(struct GameComponents* gameComponents, struct DataStructures* dataStructures); 
/*Distance from the word the player is on to the goal, around spent words; -1 if there is no way*/
int distanceToGoalFLWP(struct GameComponents* gameComponents, struct DataStructures* data);

/* The route still open from where the player stands to the goal, as one line
   of words, or NULL when there is none. Freed by the caller. */
char* routeToGoalFLWP(struct GameComponents* gameComponents, struct DataStructures* data);

// flwp-generalized additional functionality 
struct GameComponentsFLWGP* initiateFLWGP(int minAdjacenciesToStart,  int maxAdjacenciesToStart, char** goalWords, char** avoidWords, int minGoalDistance, int minAvoidDistance, int maxGoalDistance, int maxAvoidDistance, int minGoalAdjacencies, int maxGoalAdjacencies, struct DataStructures* dataStructures);
/*Everything below the start word, for a board whose first word is already
settled. Shared by initiateFLWGP and by the named-start entry point in
Debug-API, so both deal the same board from the same word.*/
struct GameComponentsFLWGP* buildFLWGPFromFLWC(struct GameComponentsFLWC* flwcComponents, int minGoalDistance, struct DataStructures* dataStructures);
struct GameComponents* getFLWPComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents);
struct GameComponentsFLWC* getFLWCComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents); 
void freeGameComponentsFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures); 
int userEntersWord_FLWGP(char* userInput, struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* dataStructures); 
void undoMoveFLWGP(struct GameComponentsFLWGP *flwgpComponents, struct DataStructures* dataStructures);
void redoMoveFLWGP(struct GameComponentsFLWGP *flwgpComponents, struct DataStructures* dataStructures);
int isStartValid_FLWGP(struct GameComponentsFLWGP* flwgpComponents); 
int hintGetMinAdjacenciesFLWGP(struct GameComponentsFLWGP* flwgpComponents); 
char* hintWordTowardsGoalFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 
char* hintGetValidGoalWordFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 

char hintLetterToConsiderFLWG(struct GameData* flwgComponents, struct DataStructures* data);
int hintNumOptionsFLWG(struct GameData* flwgComponents, struct DataStructures* data);

// Test Functionality  
char* convertIntToWord(int wordId, struct DataStructures* dataStructures); 
int convertWordToInt(char* word, struct DataStructures* dataStructures); 



#endif 
