/*************************
Name: FLWG3.h
Purpose: The adversarial game with more than two seats at the table.

************************/
#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/WordSet.h"
#include "../../api/includes/FLWG-API.h"

#ifndef seenFLWG3
#define seenFLWG3

/*The shallowest a hint may look, however shallow the board's own seats are.
Three is where the two handed game set the same floor, for the same reason: a
suggestion that cannot see the reply is not a suggestion.*/
#define FLWG3_HINT_LEAST_DEPTH 3

/* FLWG3: the same game as FLWG, round a bigger table.
 *
 * The rule does not change and does not need to. A word is dealt, everyone in
 * turn changes one letter of it to make another word, nothing may be said
 * twice, AND THE FIRST PLAYER WHO CANNOT MOVE LOSES. With two at the table that
 * is "trap the other one". With three it is a different game wearing the same
 * rule: you can be trapped by somebody who was not aiming at you, and the move
 * that traps the seat on your left hands the game to the seat on your right.
 *
 * WHAT IS ACTUALLY NEW HERE IS THE SEATING, and it is the only thing this file
 * adds to what the engine already had. The two handed API is stateless about
 * turns - the caller alternates by choosing which function to call - and that
 * works exactly as long as there are two of them and one is you. It does not
 * survive a third: "whose move is it", "which seats does the engine play" and
 * "who was it that got stuck" all become real questions, and none of them has
 * anywhere to live in struct GameData. So they live here.
 *
 * THE SEARCH WAS ALREADY WRITTEN. MaxN and Hypermax have taken numPlayers since
 * 2021 and score a vector rather than one number - the stranded seat scores -1
 * and every other seat +1, which is this rule already generalised. What they had
 * never had is a caller, a test, or the obscurity cap.
 */
struct GameComponentsFLWG3 {
	/* The word on the board. */
	int currWordId;

	/* How many seats. Three is what the campaign deals; the mode does not care,
	   and MaxN has never cared either. */
	int numPlayers;

	/* WHICH SEATS THE ENGINE PLAYS, one bit per seat.
	 *
	 * Seat 0 is the person holding the phone, so in practice this is every bit
	 * but the first. It is a mask rather than a count because the search has to
	 * know which plies are its own: the obscurity cap and the off-limits list
	 * apply to what the ENGINE says and never to what the player may type, and
	 * a search that forgot the difference would either walk into words the
	 * player can reach or refuse to model them reaching it. */
	int enginePlays;

	/* Whose move it is, 0 to numPlayers - 1. */
	int whoseTurn;

	/* How far the engine's seats look. The same number FLWG calls botType when
	   it is positive, and it means the same thing. */
	int depth;

	/* The seat that could not move, or -1 while the game is still on.
	 *
	 * ONE SEAT LOSES AND THE REST WIN, which is why this is a seat and not a
	 * winner. With two at the table the two are the same fact said twice; with
	 * three they are not, and the screen needs to say which of the two
	 * opponents ran aground. */
	int strandedSeat;
};

/*Deal a board. minAdjacencies/maxAdjacencies bound the opening word the same
way they do everywhere else. enginePlays is the seat mask; depth is how far the
engine's seats look.

Returns a game whose currWordId is -1 when the numbers admit no opening word -
isStartValidFLWG3 is the question to ask.*/
struct GameComponentsFLWG3* initiateFLWG3(int numPlayers, int enginePlays, int depth,
	int minAdjacencies, int maxAdjacencies, struct DataStructures* data);

/*The same board from a word chosen elsewhere, for the daily and for a test that
wants to know which board it is looking at.*/
struct GameComponentsFLWG3* initFLWG3AtStart(char* startWord, int numPlayers, int enginePlays,
	int depth, struct DataStructures* data);

int isStartValidFLWG3(struct GameComponentsFLWG3* game);

char* getCurrWordFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data);

/*Whose move it is. The screen draws the table from this.*/
int whoseTurnFLWG3(struct GameComponentsFLWG3* game);

/*WHETHER THE SEAT TO MOVE HAS ANYWHERE TO GO AT ALL.
 *
 * Asked BEFORE that seat is given the move, because running out is how this
 * game ends and the seat it ends on is the one that loses. A seat that is
 * stranded is not asked for a word it cannot supply.
 *
 * The answer depends on whose seat it is, which is the whole reason the mask
 * exists: an engine seat is stranded when every way out is used, past the cap,
 * or off limits; a player is stranded only when every way out is used, because
 * nothing has ever restricted what the player may type.*/
int seatIsStrandedFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data);

/*The player's move. Refused with NOT_YOUR_TURN if it is not their turn, and
otherwise with the same codes the two handed game uses.*/
int userEntersWordFLWG3(char* userInput, struct GameComponentsFLWG3* game, struct DataStructures* data);

/*An engine seat's move. Plays for whichever seat is to move, which must be one
of the engine's.

@return the word id it played, or -1 if that seat had nowhere to go - in which
case the seat is recorded as stranded and the game is over.*/
int botTakesTurnFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data);

/*How the board stands, from SEAT ZERO'S SIDE, because that is the seat the
screen is drawing for.

@return 0 the game is still on
        1 seat zero has won - somebody else ran aground
        2 seat zero has lost - seat zero ran aground*/
int isGameWonFLWG3(struct GameComponentsFLWG3* game);

/*Which seat ran aground, or -1 while the game is on.*/
int strandedSeatFLWG3(struct GameComponentsFLWG3* game);

/*One letter worth trying, or '?' when nothing leads anywhere. The same hint
the two handed board gives, asked of the word this one is standing on.*/
char hintLetterToConsiderFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data);

/*How many words are still within reach of the word on the board.

UNCAPPED, DELIBERATELY, like every other count in the game: it is a claim about
the PLAYER'S position, and a number they can check by hand has to be the number
they would get.*/
int hintNumOptionsFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data);

/*A MOVE THAT DOES NOT WALK INTO EITHER OF THEM.
 *
 * The two hints above answer about what is LEGAL, and on a board with opponents
 * legal is not the question - the move that loses you the game is legal right
 * up until it is played. This is the one hint here that knows there is anybody
 * else at the table, and with three of them it is worth more than it was with
 * one: a move that is safe from the seat on your left can be exactly the move
 * that hands you to the seat on your right.
 *
 * It searches from SEAT ZERO, and for the length of the search seat zero counts
 * as one of the engine's - so the word it offers is one the game would have
 * been willing to say itself, rather than something out of the far end of the
 * dictionary the player has never heard of.
 *
 * NULL when there is nothing safe, which on this board is a real answer.*/
char* hintSafeMoveFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data);

void freeGameComponentsFLWG3(struct GameComponentsFLWG3* game);

#endif
