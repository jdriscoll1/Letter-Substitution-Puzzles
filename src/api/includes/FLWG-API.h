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

/*Whether the engine may choose this word at all.
 *
 * A DIFFERENT QUESTION FROM THE CAP, and it is asked in the same places for the
 * opposite kind of reason. The cap keeps out words nobody knows; this keeps out
 * a handful everybody does - see docs/offLimits.txt. And the cap is a
 * preference that is dropped when a board cannot otherwise be dealt, where this
 * is never dropped at all: a board that can only be built on one of these is a
 * board that does not get built.
 *
 * Never asked about a word the PLAYER typed. They may play every one.*/
int isOffLimits(int id, struct DataStructures* data);

/*How obscure a word is, by the word. The opposite errand to isTooObscure: this
is for judging a word the PLAYER has already played, so the app can tell them
they found a good one. A word the dictionary does not know answers
OBSCURITY_UNKNOWN, which is what the rest of the engine already means by
"nothing has ever ranked this".*/
int obscurityOfWord(char* word, struct DataStructures* data);
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

/*Whether there is a move to take back, and whether there is one to put back.

Asked so the two buttons can be greyed out rather than sit there looking
pressable and do nothing. Undo and Redo both already refuse quietly when there
is nothing to do, which is correct and is also invisible: a control that looks
alive and answers a press with no change reads as the game being broken.

They are asked rather than counted in JS because the second one cannot be
counted there. Taking a move back is just "have you made one", but putting one
back depends on a redo stack that AddWord_Struct and RemoveWord_Struct both
silently clear - so a copy of it kept on the other side of the bridge would be
right until the player typed a different word after an undo, and wrong from
then on with nothing to say so.*/
int canUndoFLWP(struct GameComponents *gameComponents);
int canRedoFLWP(struct GameComponents *gameComponents);
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
/*How far the nearest word the rule admits is from where the player is standing,
around the words they have spent and the words they may not stand on.

THE COMPOSED BOARD COULD NOT ANSWER THIS, and the plain pathfinder's answer is
not a substitute for two reasons. It measures to ONE word, and this board has a
set; and it is asked of the walk, which on a composed board is borrowed and
carries no goal at all - so it answered -1 for every position of every such
board. A level that hangs a move limit off the distance was therefore told the
goal was unreachable from the opening word.

hintGetMinAdjacenciesFLWGP is not it either: that is the route the board was
DEALT with, measured from the first word, and the player has usually moved.*/
int distanceToGoalFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 
char* hintWordTowardsGoalFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 
char* hintGetValidGoalWordFLWGP(struct GameComponentsFLWGP* flwgpComponents, struct DataStructures* data); 

char hintLetterToConsiderFLWG(struct GameData* flwgComponents, struct DataStructures* data);
int hintNumOptionsFLWG(struct GameData* flwgComponents, struct DataStructures* data);

/*How deep the safe-move hint looks when the board's own bot does not search.

A board whose bot is random or picks the widest word has no plan to see, so
there is no depth to match. Advice worked out against an opponent that does
plan is only ever too careful, never wrong, and being too careful for three
plies costs nothing a player would notice.*/
#define FLWG_HINT_LEAST_DEPTH 3

/*A move that does not walk into the bot, as a word, or NULL when there is no
move left at all.

The other two FLWG hints answer about the position: which letter leads
somewhere, how many ways out there are. Both are about what is LEGAL, and on a
board with an opponent legal is not the question - the move that loses you the
game is legal right up until it is played. This is the one hint on this mode
that knows there is a bot: it runs the same search the bot runs, from the seat
the player is sitting in, and hands back what it would do.

Two things about that are worth knowing before changing it.

It is ADVICE, NOT A TURN. botPly plays the move it finds - it marks the word
spent - so this puts it straight back. The player still has to type it, and may
well type something else.

The obscurity cap swaps sides, and that is correct. Inside the search the cap
falls on whoever is to move, which is normally the bot; here it is the player
being advised, so what comes back is never a word the board would not have
dealt. The BOT's replies are then searched over the whole graph, which reads it
as able to answer with anything - so the advice is a little pessimistic about
what it can do. That is the right way round: the hint picks the move that is
hardest to punish rather than one that is only safe while the bot stays
inside its vocabulary.*/
char* hintSafeMoveFLWG(struct GameData* flwgComponents, int botType, struct DataStructures* data);

// Test Functionality  
char* convertIntToWord(int wordId, struct DataStructures* dataStructures); 
int convertWordToInt(char* word, struct DataStructures* dataStructures); 



#endif 
