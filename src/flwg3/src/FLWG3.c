/*The adversarial game with more than two seats at the table. See FLWG3.h.*/

#include <stdlib.h>
#include <stdio.h>

#include "../includes/FLWG3.h"

#include "../../algs/includes/Hypermax.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../flwg/includes/Hints2.h"
#include "../../flwp/includes/UserInput.h"
#include "../../structs/includes/IntLinkedList.h"

/*Whether a seat is one the engine plays.*/
static int engineHasTheSeat(struct GameComponentsFLWG3* game, int seat){
	if(game == NULL || seat < 0 || seat >= game->numPlayers){
		return 0;
	}
	return (game->enginePlays >> seat) & 1;
}

/*Everything below the opening word, shared by the two ways of dealing one.*/
static struct GameComponentsFLWG3* buildFLWG3(int startId, int numPlayers, int enginePlays,
	int depth, struct DataStructures* data){

	struct GameComponentsFLWG3* game = malloc(sizeof(struct GameComponentsFLWG3));
	if(game == NULL){
		return NULL;
	}

	game->currWordId = startId;
	game->numPlayers = numPlayers;
	game->enginePlays = enginePlays;
	game->depth = depth;
	/*Seat zero opens, which is the person holding the phone. A board where the
	  engine moved first would show the player a word they had not been given a
	  turn on.*/
	game->whoseTurn = 0;
	game->strandedSeat = -1;

	reset_WordSet(data->wordSet);
	if(startId != -1){
		markUsed_WordSet(startId, data->wordSet);
	}
	return game;
}

struct GameComponentsFLWG3* initiateFLWG3(int numPlayers, int enginePlays, int depth,
	int minAdjacencies, int maxAdjacencies, struct DataStructures* data){

	if(data == NULL || numPlayers < 2){
		return NULL;
	}
	return buildFLWG3(ChooseStart_Range(data->I2W, minAdjacencies, maxAdjacencies),
		numPlayers, enginePlays, depth, data);
}

struct GameComponentsFLWG3* initFLWG3AtStart(char* startWord, int numPlayers, int enginePlays,
	int depth, struct DataStructures* data){

	if(data == NULL || startWord == NULL || numPlayers < 2){
		return NULL;
	}
	return buildFLWG3(convertWordToInt(startWord, data), numPlayers, enginePlays, depth, data);
}

int isStartValidFLWG3(struct GameComponentsFLWG3* game){
	if(game == NULL){
		return 0;
	}
	return game->currWordId != -1;
}

char* getCurrWordFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL){
		return NULL;
	}
	return convertIntToWord(game->currWordId, data);
}

int whoseTurnFLWG3(struct GameComponentsFLWG3* game){
	if(game == NULL){
		return -1;
	}
	return game->whoseTurn;
}

int strandedSeatFLWG3(struct GameComponentsFLWG3* game){
	if(game == NULL){
		return -1;
	}
	return game->strandedSeat;
}

int seatIsStrandedFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL || data == NULL || game->currWordId < 0){
		return 1;
	}

	int enginesTurn = engineHasTheSeat(game, game->whoseTurn);
	struct intList* option = getConnections(game->currWordId, data->I2W);

	for(option = option->next; option != NULL; option = option->next){
		if(checkIfUsed_WordSet(option->data, data->wordSet)){
			continue;
		}
		/*THE CAP IS THE ENGINE'S, NOT THE TABLE'S. A word past it is one this
		  level does not deal, so an engine seat may not reach for it - but the
		  player may type anything in the dictionary, and a seat of theirs that
		  was called stranded over a word they could have played would be losing
		  a game they had not lost.*/
		if(enginesTurn && (isTooObscureForGraph(option->data, data->I2W)
			|| isOffLimitsForGraph(option->data, data->I2W))){
			continue;
		}
		return 0;
	}
	return 1;
}

int userEntersWordFLWG3(char* userInput, struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL || data == NULL){
		return UNKNOWN_ERROR;
	}
	/*A seat the engine plays is not one the player may play for.*/
	if(engineHasTheSeat(game, game->whoseTurn)){
		return NOT_YOUR_TURN;
	}
	if(game->strandedSeat != -1){
		return NOT_YOUR_TURN;
	}

	enum ERROR_CODE result;
	if((result = Check_Input(game->currWordId, (const char*)userInput, data)) != VALID){
		return result;
	}

	int wordId = convertWordToInt(userInput, data);
	if(checkIfUsed_WordSet(wordId, data->wordSet)){
		return WORD_USED;
	}

	markUsed_WordSet(wordId, data->wordSet);
	game->currWordId = wordId;
	game->whoseTurn = (game->whoseTurn + 1) % game->numPlayers;
	return VALID;
}

int botTakesTurnFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL || data == NULL){
		return -1;
	}
	if(!engineHasTheSeat(game, game->whoseTurn) || game->strandedSeat != -1){
		return -1;
	}

	/*ASKED BEFORE THE SEARCH RATHER THAN READ OUT OF IT. Hypermax answers -1
	  from a root with nothing to play, but it answers the same -1 from a root
	  whose every move is past the cap, and the two are the same thing to this
	  game - so the board is better off deciding it in one pass of the
	  adjacencies than in a whole search.*/
	if(seatIsStrandedFLWG3(game, data)){
		game->strandedSeat = game->whoseTurn;
		return -1;
	}

	int chosen = Hypermax(game->currWordId, game->whoseTurn, game->numPlayers,
		game->enginePlays, game->depth, data->I2W, data->wordSet);

	if(chosen == -1){
		game->strandedSeat = game->whoseTurn;
		return -1;
	}

	/*Hypermax leaves the root marked and does not claim what it chose, so the
	  word is spent here. Everything else the search touched it put back.*/
	markUsed_WordSet(chosen, data->wordSet);
	game->currWordId = chosen;
	game->whoseTurn = (game->whoseTurn + 1) % game->numPlayers;
	return chosen;
}

int isGameWonFLWG3(struct GameComponentsFLWG3* game){
	if(game == NULL || game->strandedSeat == -1){
		return 0;
	}
	/*ONE SEAT LOSES AND EVERY OTHER SEAT WINS. Seat zero is the one the screen
	  is drawing for, so the answer is about seat zero: it won unless it was the
	  seat that ran aground.*/
	return game->strandedSeat == 0 ? 2 : 1;
}

char hintLetterToConsiderFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL || data == NULL){
		return '?';
	}
	return letterToConsiderHint(game->currWordId, data);
}

int hintNumOptionsFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL || data == NULL){
		return -1;
	}
	return numOptionsHint(game->currWordId, data);
}

char* hintSafeMoveFLWG3(struct GameComponentsFLWG3* game, struct DataStructures* data){
	if(game == NULL || data == NULL){
		return NULL;
	}

	/*Never shallower than the board is, and never shallow enough to be useless:
	  a hint that looks one move further than the seat about to answer is the
	  least that can be called advice.*/
	int howFar = game->depth > FLWG3_HINT_LEAST_DEPTH ? game->depth : FLWG3_HINT_LEAST_DEPTH;

	/*Seat zero counts as one of the engine's for the length of this search, so
	  that what comes back is a word the game would have said itself. The board
	  does not keep the change - enginePlays is read here and not written.*/
	int asIfItWereOurs = game->enginePlays | 1;

	int suggestion = Hypermax(game->currWordId, 0, game->numPlayers, asIfItWereOurs,
		howFar, data->I2W, data->wordSet);

	if(suggestion == -1){
		return NULL;
	}
	/*Hypermax does not claim what it chooses - botTakesTurnFLWG3 is the thing
	  that spends a word - so there is nothing to give back here.*/
	return Convert_IntToWord(suggestion, data->I2W);
}

void freeGameComponentsFLWG3(struct GameComponentsFLWG3* game){
	if(game == NULL){
		return;
	}
	free(game);
}
