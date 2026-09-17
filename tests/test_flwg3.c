/*
Title: test_flwg3.c
Description: The adversarial game with three at the table.

WHAT IS UNDER TEST IS THE SEATING, because the rule is not new and the search is
not new. A word is dealt, everyone in turn changes one letter, nothing is said
twice, and the first player who cannot move loses. That is FLWG's rule verbatim.
What FLWG never had to answer is WHOSE MOVE IT IS, WHICH SEATS THE ENGINE PLAYS,
and WHO IT WAS THAT RAN AGROUND - and those three questions are the mode.

AND THE SEARCH HAD NEVER BEEN ASKED TO OBEY THE CAP. MaxN and Hypermax have
taken numPlayers since 2021 and were never called from a live board, so neither
of them had ever been told that there are words the engine may not say. A three
handed bot that reached for a word past the level's vocabulary would be doing
the thing the two handed bot has been forbidden from doing since the ranks
shipped, and two of the tests below are only about that.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/flwg3/includes/FLWG3.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/algs/includes/MaxN.h"
#include "../src/structs/includes/IntLinkedList.h"

#define LIST "docs/offLimits.txt"

/*Three at the table, seats one and two played by the engine.*/
#define THE_ENGINE_HAS_THE_OTHER_TWO ((1 << 1) | (1 << 2))

static struct DataStructures* threeLetterBoard(void){
	struct DataStructures* data = open_dictionary("docs/3.txt", 3);
	Load_Obscurity(data->I2W, "docs/3ranks.txt");
	Load_OffLimits(data->I2W, LIST, data);
	return data;
}

/*The word the game is standing on, as a string owned by the dictionary.*/
static char* here(struct GameComponentsFLWG3* game, struct DataStructures* data){
	return getCurrWordFLWG3(game, data);
}

/* --------------------------------------------------------------- dealing */

void test_flwg3_deals_a_board_with_everyone_seated(void){
	struct DataStructures* data = threeLetterBoard();

	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		10, 30, data);

	CHECK(game != NULL);
	CHECK(isStartValidFLWG3(game));

	/*The opening word is inside the band it was asked for, and it is spent -
	  nobody may play it back.*/
	CHECK(getNumAdjacencies(game->currWordId, data) >= 10);
	CHECK(getNumAdjacencies(game->currWordId, data) <= 30);
	CHECK(checkIfUsed_WordSet(game->currWordId, data->wordSet) != 0);

	/*SEAT ZERO OPENS, always. A board where the engine moved first would show
	  the player a word they were never given a turn on.*/
	CHECK_INT(whoseTurnFLWG3(game), 0);
	CHECK_INT(strandedSeatFLWG3(game), -1);
	CHECK_INT(isGameWonFLWG3(game), 0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

void test_flwg3_can_be_dealt_a_named_start(void){
	struct DataStructures* data = threeLetterBoard();

	struct GameComponentsFLWG3* game = initFLWG3AtStart("cat", 3,
		THE_ENGINE_HAS_THE_OTHER_TWO, 2, data);

	CHECK(isStartValidFLWG3(game));
	CHECK_INT(strcmp(here(game, data), "cat"), 0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

/* ----------------------------------------------------------- taking turns */

void test_flwg3_the_turn_goes_round_the_table(void){
	struct DataStructures* data = threeLetterBoard();
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	CHECK_INT(whoseTurnFLWG3(game), 0);

	/*The player plays a neighbour of the opening word.*/
	struct intList* option = getConnections(game->currWordId, data->I2W);
	char* neighbour = Convert_IntToWord(option->next->data, data->I2W);
	CHECK_INT(userEntersWordFLWG3(neighbour, game, data), VALID);
	CHECK_INT(whoseTurnFLWG3(game), 1);

	CHECK(botTakesTurnFLWG3(game, data) != -1);
	CHECK_INT(whoseTurnFLWG3(game), 2);

	CHECK(botTakesTurnFLWG3(game, data) != -1);
	/*And back round to the player.*/
	CHECK_INT(whoseTurnFLWG3(game), 0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

void test_flwg3_refuses_a_word_played_out_of_turn(void){
	struct DataStructures* data = threeLetterBoard();
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	struct intList* option = getConnections(game->currWordId, data->I2W);
	char* neighbour = Convert_IntToWord(option->next->data, data->I2W);
	CHECK_INT(userEntersWordFLWG3(neighbour, game, data), VALID);

	/*It is seat one's move now, and seat one belongs to the engine. Whatever
	  the player types is refused for being somebody else's turn rather than for
	  being a bad word - and the board does not move.*/
	int wasHere = game->currWordId;
	struct intList* next = getConnections(game->currWordId, data->I2W);
	char* legalWord = Convert_IntToWord(next->next->data, data->I2W);

	CHECK_INT(userEntersWordFLWG3(legalWord, game, data), NOT_YOUR_TURN);
	CHECK_INT(game->currWordId, wasHere);
	CHECK_INT(whoseTurnFLWG3(game), 1);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

void test_flwg3_a_refused_word_does_not_pass_the_turn(void){
	struct DataStructures* data = threeLetterBoard();
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	/*The word already on the board has been spent.*/
	CHECK_INT(userEntersWordFLWG3(here(game, data), game, data), WORD_USED);
	CHECK_INT(whoseTurnFLWG3(game), 0);

	/*And a word that is not a word at all.*/
	CHECK(userEntersWordFLWG3("zzz", game, data) != VALID);
	CHECK_INT(whoseTurnFLWG3(game), 0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

void test_flwg3_the_bot_plays_a_legal_move(void){
	struct DataStructures* data = threeLetterBoard();
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	struct intList* option = getConnections(game->currWordId, data->I2W);
	CHECK_INT(userEntersWordFLWG3(Convert_IntToWord(option->next->data, data->I2W), game, data), VALID);

	char before[8];
	strncpy(before, here(game, data), 7);
	before[7] = '\0';

	int played = botTakesTurnFLWG3(game, data);
	CHECK(played != -1);

	char* now = here(game, data);
	/*One letter different, and it is the word the call said it played.*/
	CHECK_INT(letters_that_differ(before, now, 3), 1);
	CHECK_INT(played, game->currWordId);
	/*And claimed, so nobody may say it again.*/
	CHECK(checkIfUsed_WordSet(played, data->wordSet) != 0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

/* ------------------------------------------- the words the engine may say */

/*THE CAP IS THE POINT OF THIS ONE. Every word the engine plays, on either of
  its two seats, has to sit inside the vocabulary the level was given - which
  the three handed searches had never been told about, because nothing had ever
  called them from a board.*/
void test_flwg3_the_bots_stay_inside_the_vocabulary(void){
	struct DataStructures* data = threeLetterBoard();
	setObscurityCap(data, 20000);

	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	int plies = 0;
	/*How often the engine had a word past the cap sitting right next to it and
	  did not take it. Counted because a test that only checks what was played
	  would pass just as well against a search that was never tempted - on a
	  board where every neighbour is inside the cap, "it stayed inside the cap"
	  is a fact about the board and not about the search.*/
	int tempted = 0;
	int turn;

	for(turn = 0; turn < 40 && isGameWonFLWG3(game) == 0; turn++){
		struct intList* option;
		int moved = 0;

		if(whoseTurnFLWG3(game) == 0){
			/*The player walks wherever the graph lets them, cap or no cap - and
			  deliberately out of it where they can, to push the engine towards
			  the obscure end of the dictionary.*/
			for(option = getConnections(game->currWordId, data->I2W)->next;
			    option != NULL; option = option->next){
				if(checkIfUsed_WordSet(option->data, data->wordSet)){
					continue;
				}
				if(userEntersWordFLWG3(Convert_IntToWord(option->data, data->I2W), game, data) == VALID){
					moved = 1;
					break;
				}
			}
			if(!moved){
				break;
			}
			continue;
		}

		for(option = getConnections(game->currWordId, data->I2W)->next;
		    option != NULL; option = option->next){
			if(checkIfUsed_WordSet(option->data, data->wordSet) == 0
			   && isTooObscure(option->data, data)){
				tempted++;
			}
		}

		int played = botTakesTurnFLWG3(game, data);
		if(played == -1){
			break;
		}
		plies++;
		CHECK_INT(isTooObscure(played, data), 0);
		CHECK_INT(isOffLimits(played, data), 0);
	}

	/*The run has to have actually happened, and the engine has to have had
	  something to refuse, or both checks above passed by never being asked.

	  EVEN SO THIS ONE IS THE WEAKER OF THE TWO. It watches a whole game and
	  reports what was played, and a search with no cap at all would pass it on
	  most boards - an obscure word is not usually the best move, so not taking
	  one proves very little. Removing the gate from Hypermax and running this
	  test was how that was found out: it still passed. The test below is the
	  one that bites, and this one is here to say the mode as a whole behaves
	  over a real game rather than in a set-up position.*/
	CHECK(plies > 0);
	CHECK(tempted > 0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

/*THE SEARCH, CORNERED. This is the test that bites.
 *
 * A position is built where every way out of the word the engine is standing on
 * is past the cap: the in-cap neighbours are spent, the obscure ones are not. A
 * search that respects the cap has nowhere to go and says so. A search that does
 * not will happily take one, and there is no move it could prefer instead, so
 * the two answers cannot be confused.
 *
 * ASKED OF HYPERMAX DIRECTLY rather than through a board, because the board
 * decides "stranded" for itself before it searches - so going through the board
 * would test the board's gate twice and the search's not at all.
 */
void test_flwg3_the_search_will_not_reach_past_the_cap(void){
	struct DataStructures* data = threeLetterBoard();
	setObscurityCap(data, 2000);

	/*A word with both kinds of neighbour, found rather than named so the test
	  does not go stale when the ranks or the dictionary change.*/
	int here = -1;
	int i;
	for(i = 0; i < data->I2W->numWords && here == -1; i++){
		int inCap = 0;
		int past = 0;
		struct intList* option;
		for(option = getConnections(i, data->I2W)->next; option != NULL; option = option->next){
			if(isTooObscureForGraph(option->data, data->I2W)){
				past++;
			}
			else{
				inCap++;
			}
		}
		if(past > 0 && inCap > 0){
			here = i;
		}
	}
	CHECK(here != -1);

	/*Spend every way out that the engine would have been allowed, leaving only
	  the ones it may not say.*/
	reset_WordSet(data->wordSet);
	markUsed_WordSet(here, data->wordSet);
	int left = 0;
	struct intList* option;
	for(option = getConnections(here, data->I2W)->next; option != NULL; option = option->next){
		if(isTooObscureForGraph(option->data, data->I2W)){
			left++;
		}
		else{
			markUsed_WordSet(option->data, data->wordSet);
		}
	}
	CHECK(left > 0);

	/*Told the seat is its own, it has nowhere to go.*/
	CHECK_INT(Hypermax(here, 1, 3, THE_ENGINE_HAS_THE_OTHER_TWO, 2, data->I2W, data->wordSet), -1);

	/*Told it is playing nobody - which is what every caller before the three
	  handed game passed, and what the search did for its whole life - it takes
	  one of the very words it just refused.*/
	int taken = Hypermax(here, 1, 3, 0, 2, data->I2W, data->wordSet);
	CHECK(taken != -1);
	CHECK_INT(isTooObscureForGraph(taken, data->I2W), 1);

	freeDataStructures(data);
}

/*And the rule the search applies, asked on its own.*/
void test_flwg3_the_search_only_holds_its_own_seats_to_the_cap(void){
	struct DataStructures* data = threeLetterBoard();
	setObscurityCap(data, 800);

	/*A word past the cap, found rather than named, so the test does not go
	  stale when the ranks change.*/
	int obscure = -1;
	int i;
	for(i = 0; i < data->I2W->numWords && obscure == -1; i++){
		if(isTooObscureForGraph(i, data->I2W)){
			obscure = i;
		}
	}
	CHECK(obscure != -1);

	int engineHasSeatsOneAndTwo = THE_ENGINE_HAS_THE_OTHER_TWO;

	/*On the engine's seats it is a word the engine would not say.*/
	CHECK_INT(engineWouldNotSay(obscure, 1, engineHasSeatsOneAndTwo, data->I2W), 1);
	CHECK_INT(engineWouldNotSay(obscure, 2, engineHasSeatsOneAndTwo, data->I2W), 1);

	/*ON THE PLAYER'S SEAT IT IS NOT, and that is not an oversight. The player
	  may type anything in the dictionary, so a search that refused to model
	  them doing it would be planning against an opponent who does not exist.*/
	CHECK_INT(engineWouldNotSay(obscure, 0, engineHasSeatsOneAndTwo, data->I2W), 0);

	/*And a search told it plays nobody is the search as it was before any of
	  this, which is what every caller that came before passes.*/
	CHECK_INT(engineWouldNotSay(obscure, 1, 0, data->I2W), 0);

	freeDataStructures(data);
}

/* ------------------------------------------------------- running aground */

/*THE WHOLE POINT OF A THIRD SEAT, in one test. With two at the table "somebody
  is stuck" and "I won" are the same fact said twice. With three they are not:
  the seat that runs aground loses and BOTH of the others win, so the board has
  to say which seat it was and not merely that the game is over.*/
void test_flwg3_the_seat_that_runs_aground_is_the_one_that_loses(void){
	struct DataStructures* data = threeLetterBoard();
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	/*Wall the board in: every word but the one being stood on is spent, so
	  whichever seat is asked next has nowhere to go.*/
	int i;
	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}

	CHECK_INT(seatIsStrandedFLWG3(game, data), 1);

	/*Seat zero is to move and seat zero is the player, so the player loses.*/
	CHECK_INT(whoseTurnFLWG3(game), 0);
	CHECK_INT(isGameWonFLWG3(game), 0);   /*not until somebody is actually asked*/

	/*Hand the move to seat one instead and it is seat one that is stranded -
	  and seat zero has won without playing a word.*/
	game->whoseTurn = 1;
	CHECK_INT(botTakesTurnFLWG3(game, data), -1);
	CHECK_INT(strandedSeatFLWG3(game), 1);
	CHECK_INT(isGameWonFLWG3(game), 1);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

void test_flwg3_the_player_running_aground_is_a_loss(void){
	struct DataStructures* data = threeLetterBoard();
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	int i;
	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}

	/*The screen asks before it offers the keyboard, and a stranded seat zero is
	  the player losing rather than the player being asked for a word they do
	  not have.*/
	CHECK_INT(whoseTurnFLWG3(game), 0);
	CHECK_INT(seatIsStrandedFLWG3(game, data), 1);

	game->strandedSeat = 0;
	CHECK_INT(isGameWonFLWG3(game), 2);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

/*A whole game, played out. Exactly one seat runs aground and the board agrees
  with itself about which.*/
void test_flwg3_a_game_ends_on_exactly_one_stranded_seat(void){
	struct DataStructures* data = threeLetterBoard();
	setObscurityCap(data, 5000);
	struct GameComponentsFLWG3* game = initiateFLWG3(3, THE_ENGINE_HAS_THE_OTHER_TWO, 2,
		14, 30, data);

	int moves = 0;
	while(isGameWonFLWG3(game) == 0 && moves < 300){
		if(seatIsStrandedFLWG3(game, data)){
			game->strandedSeat = whoseTurnFLWG3(game);
			break;
		}
		if(whoseTurnFLWG3(game) == 0){
			struct intList* option = getConnections(game->currWordId, data->I2W);
			for(option = option->next; option != NULL; option = option->next){
				if(checkIfUsed_WordSet(option->data, data->wordSet)){
					continue;
				}
				if(userEntersWordFLWG3(Convert_IntToWord(option->data, data->I2W), game, data) == VALID){
					break;
				}
			}
		}
		else{
			botTakesTurnFLWG3(game, data);
		}
		moves++;
	}

	CHECK(moves > 0);
	CHECK(moves < 300);
	int stranded = strandedSeatFLWG3(game);
	CHECK(stranded >= 0);
	CHECK(stranded < 3);
	/*And the answer the screen reads matches the seat.*/
	CHECK_INT(isGameWonFLWG3(game), stranded == 0 ? 2 : 1);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
}

/* --------------------------------------------------------- handed nothing */

void test_flwg3_survives_being_handed_nothing(void){
	struct DataStructures* data = threeLetterBoard();

	CHECK(initiateFLWG3(3, 6, 2, 10, 30, NULL) == NULL);
	CHECK(initFLWG3AtStart(NULL, 3, 6, 2, data) == NULL);
	CHECK(initFLWG3AtStart("cat", 3, 6, 2, NULL) == NULL);
	/*A table of one is not a game.*/
	CHECK(initiateFLWG3(1, 0, 2, 10, 30, data) == NULL);

	CHECK_INT(isStartValidFLWG3(NULL), 0);
	CHECK(getCurrWordFLWG3(NULL, data) == NULL);
	CHECK_INT(whoseTurnFLWG3(NULL), -1);
	CHECK_INT(strandedSeatFLWG3(NULL), -1);
	CHECK_INT(seatIsStrandedFLWG3(NULL, data), 1);
	CHECK_INT(userEntersWordFLWG3("cat", NULL, data), UNKNOWN_ERROR);
	CHECK_INT(botTakesTurnFLWG3(NULL, data), -1);
	CHECK_INT(isGameWonFLWG3(NULL), 0);
	freeGameComponentsFLWG3(NULL);

	freeDataStructures(data);
}

void suite_flwg3(void){
	printf("\n-- three at the table --\n");
	RUN_TEST(test_flwg3_deals_a_board_with_everyone_seated);
	RUN_TEST(test_flwg3_can_be_dealt_a_named_start);
	RUN_TEST(test_flwg3_the_turn_goes_round_the_table);
	RUN_TEST(test_flwg3_refuses_a_word_played_out_of_turn);
	RUN_TEST(test_flwg3_a_refused_word_does_not_pass_the_turn);
	RUN_TEST(test_flwg3_the_bot_plays_a_legal_move);
	RUN_TEST(test_flwg3_the_bots_stay_inside_the_vocabulary);
	RUN_TEST(test_flwg3_the_search_will_not_reach_past_the_cap);
	RUN_TEST(test_flwg3_the_search_only_holds_its_own_seats_to_the_cap);
	RUN_TEST(test_flwg3_the_seat_that_runs_aground_is_the_one_that_loses);
	RUN_TEST(test_flwg3_the_player_running_aground_is_a_loss);
	RUN_TEST(test_flwg3_a_game_ends_on_exactly_one_stranded_seat);
	RUN_TEST(test_flwg3_survives_being_handed_nothing);
}
