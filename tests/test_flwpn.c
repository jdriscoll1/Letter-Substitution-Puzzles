/*
Title: test_flwpn.c
Description: The walk with more than one port of call.

Most of what this mode does is the ordinary walk, and the ordinary walk is
already tested. What is new is three things:

  - the goal moves on when you arrive rather than the game ending
  - a run is marked against ALL the legs, not the first one
  - a port walked through early is still there when its turn comes

The third was built as a trap and turned out not to be one, which is worth
recording because the reasoning was so tidy. A walk refuses a word it has
already had, so passing through a later port ought to spend it and leave the
board unwinnable - and a whole mechanism was written to put such ports back.
Then a probe played a word, played away from it, and played it again, and the
walk took it: AddWord_Struct does not mark anything used. The only word a walk
ever spends is the one it STARTED on.

So the mechanism came out and the test stayed, because the property still
matters - it is just delivered by the walk rather than by FLWPN. If a walk ever
does start spending words, this file goes red instead of the game quietly
dealing boards that cannot be won.

And the one real hazard is the opposite of the imagined one: a port that IS the
start word would be a port nobody could reach, because the start is exactly the
word a walk will not take back.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/flwpn/includes/FLWPN.h"
#include "../src/flwp/includes/PathGameComponents.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"
#include "../src/structs/includes/WordSet.h"

struct DataStructures* open_dictionary(const char* path, int numLetters);

/* Get from where the board is now to whatever port is next, and say how many
   moves it took - or -1 when it could not, which is the failure this file is
   really about.

   IT FINDS ITS OWN WAY RATHER THAN READING gc->solution. The engine works the
   route out when a leg begins and then leaves it alone, because what reads it
   is the route hint; the moment a player wanders off it - which is exactly the
   case being tested here - it describes a journey from somewhere they are no
   longer standing. Following it then replays a move already made and the walk
   refuses it as a word used, which reads precisely like a broken engine and is
   not one.

   It also has to go AROUND spent words, which the engine's own solver does not:
   solve_FLWP searches the whole graph. A route through a word already played
   would be rejected move by move. */
static int routeAvoidingSpentWords(int from, int to, struct DataStructures* data,
	int* route, int max){
	int total = data->I2W->numWords;
	int* cameFrom = malloc(sizeof(int) * total);
	int* queue = malloc(sizeof(int) * total);
	int i, head = 0, tail = 0, found = 0;

	for(i = 0; i < total; i++){
		cameFrom[i] = -2;
	}
	cameFrom[from] = -1;
	queue[tail++] = from;

	while(head < tail && !found){
		int curr = queue[head++];
		struct intList* c = getConnections(curr, data->I2W);
		for(c = c->next; c != NULL; c = c->next){
			int next = c->data;
			if(cameFrom[next] != -2){
				continue;
			}
			/*a word already played cannot be walked through again*/
			if(next != to && checkIfUsed_WordSet(next, data->wordSet)){
				continue;
			}
			cameFrom[next] = curr;
			if(next == to){
				found = 1;
				break;
			}
			queue[tail++] = next;
		}
	}

	int length = 0;
	if(found){
		int backwards[128];
		int at = to, steps = 0;
		while(at != -1 && steps < 128){
			backwards[steps++] = at;
			at = cameFrom[at];
		}
		for(i = steps - 1; i >= 0 && length < max; i--){
			route[length++] = backwards[i];
		}
	}

	free(cameFrom);
	free(queue);
	return found ? length : 0;
}

static int walkToTheNextPort(struct GameComponentsFLWPN* game, struct DataStructures* data){
	struct GameComponents* walk = getFLWPComponentsFLWPN(game);

	if(walk->prevInput == walk->goal){
		return 0;
	}

	int route[64];
	int length = routeAvoidingSpentWords(walk->prevInput, walk->goal, data, route, 64);
	if(length < 2){
		return -1;
	}

	int moves = 0, i;
	for(i = 1; i < length; i++){
		if(userEntersWordFLWPN(convertIntToWord(route[i], data), game, data) != VALID){
			return -1;
		}
		moves++;
	}

	return walk->prevInput == route[length - 1] ? moves : -1;
}

void test_flwpn_deals_as_many_ports_as_it_was_asked_for(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(7);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 4, 1, 30, 3, data);
	CHECK(game != NULL);

	if(game != NULL){
		CHECK_INT(legsTotalFLWPN(game), 3);
		CHECK_INT(legsDoneFLWPN(game), 0);
		CHECK_INT(isStartValidFLWPN(game), 1);
		CHECK_INT(isGameWonFLWPN(game), 0);

		/*every port is a different word, and none of them is where it began*/
		struct GameComponents* walk = getFLWPComponentsFLWPN(game);
		CHECK(game->goals[0] != game->goals[1]);
		CHECK(game->goals[1] != game->goals[2]);
		CHECK(game->goals[0] != game->goals[2]);
		CHECK(game->goals[0] != walk->start);
		CHECK(game->goals[1] != walk->start);
		CHECK(game->goals[2] != walk->start);

		/*and the first one is what the board is sailing for*/
		CHECK_INT(currentGoalFLWPN(game), game->goals[0]);
		CHECK_INT(walk->goal, game->goals[0]);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* Arriving moves the goal on rather than ending the game. */
void test_flwpn_moves_the_goal_on_when_you_arrive(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(11);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 4, 1, 30, 3, data);
	CHECK(game != NULL);

	if(game != NULL){
		int first = game->goals[0];

		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 1);
		CHECK_INT(isGameWonFLWPN(game), 0);
		CHECK_INT(currentGoalFLWPN(game), game->goals[1]);
		/*standing on the port just called at*/
		CHECK_INT(getFLWPComponentsFLWPN(game)->prevInput, first);

		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 2);
		CHECK_INT(isGameWonFLWPN(game), 0);

		/*and only the last one wins it*/
		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 3);
		CHECK_INT(isGameWonFLWPN(game), 1);
		CHECK_INT(currentGoalFLWPN(game), -1);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* A player actually walking through a later port, and then finishing the board.
 *
 * It needs a board whose second port is a neighbour of the start, so the deal
 * is searched for one rather than hoped for. If no seed in sixty produces one
 * this FAILS - a scenario that cannot be set up any more is a scenario nobody
 * is testing, and finding that out from a red test beats finding it out from a
 * player on a dead board. The first version of this shrugged when the deal did
 * not oblige, which is a passing test that tested nothing.
 */
void test_flwpn_a_port_walked_through_early_is_still_there_later(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	struct GameComponentsFLWPN* game = NULL;
	int seed;

	for(seed = 1; seed <= 60 && game == NULL; seed++){
		seedGameRandom(seed);
		struct GameComponentsFLWPN* candidate = initiateFLWPN(8, 30, 1, 2, 1, 30, 3, data);
		if(candidate == NULL){
			continue;
		}

		/*is the SECOND port a neighbour of the start, so it can be played first?*/
		struct GameComponents* walk = getFLWPComponentsFLWPN(candidate);
		int reachable = 0;
		struct intList* c = getConnections(walk->start, data->I2W);
		for(c = c->next; c != NULL && !reachable; c = c->next){
			if(c->data == candidate->goals[1]){
				reachable = 1;
			}
		}

		if(reachable){
			game = candidate;
		}
		else {
			freeGameComponentsFLWPN(candidate, data);
			resetWordSet(data);
		}
	}

	CHECK(game != NULL);

	if(game != NULL){
		int second = game->goals[1];

		/*play the second port while the first is still what is being sailed for*/
		CHECK_INT(userEntersWordFLWPN(convertIntToWord(second, data), game, data), VALID);

		/*it was a move, and it was NOT an arrival at a port that is not due*/
		CHECK_INT(legsDoneFLWPN(game), 0);

		/*and it is not spent, which is the whole point*/
		CHECK_INT((int)checkIfUsed_WordSet(second, data->wordSet), 0);

		/*so the board can still be finished, all the way through*/
		CHECK(walkToTheNextPort(game, data) >= 0);
		CHECK_INT(legsDoneFLWPN(game), 1);
		CHECK(walkToTheNextPort(game, data) >= 0);
		CHECK_INT(legsDoneFLWPN(game), 2);
		CHECK(walkToTheNextPort(game, data) >= 0);
		CHECK_INT(isGameWonFLWPN(game), 1);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* Every port is kept reachable, not just the next one - a three-legged board
   walked through its LAST port on move one must still be finishable. */
void test_flwpn_keeps_every_port_that_is_still_to_come(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(5);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 3, 1, 30, 3, data);
	CHECK(game != NULL);

	if(game != NULL){
		int i;
		/*nothing still to come is spent, before a move is made*/
		for(i = 0; i < 3; i++){
			CHECK_INT((int)checkIfUsed_WordSet(game->goals[i], data->wordSet), 0);
		}

		CHECK(walkToTheNextPort(game, data) > 0);

		/*the one just called at may be spent; the ones still to come may not*/
		for(i = 1; i < 3; i++){
			CHECK_INT((int)checkIfUsed_WordSet(game->goals[i], data->wordSet), 0);
		}

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* A run is marked against the whole chain.
 *
 * The pathfinder already had this bug once, against distanceToGoal: a perfect
 * run scored three moves against a target of two and took two stars, and three
 * stars was unreachable on seven of the ten path levels. A three-legged walk
 * marked against one leg would be that again, three times over.
 */
void test_flwpn_measures_the_whole_chain_not_the_first_leg(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(13);

	struct GameComponentsFLWPN* one = initiateFLWPN(8, 30, 3, 3, 1, 30, 1, data);
	CHECK(one != NULL);
	int oneLeg = one != NULL ? shortestRouteFLWPN(one) : 0;
	if(one != NULL){ freeGameComponentsFLWPN(one, data); }

	seedGameRandom(13);
	struct GameComponentsFLWPN* three = initiateFLWPN(8, 30, 3, 3, 1, 30, 3, data);
	CHECK(three != NULL);
	int threeLegs = three != NULL ? shortestRouteFLWPN(three) : 0;

	CHECK(oneLeg >= 3);
	/*three legs of about three moves each, so comfortably more than one leg*/
	CHECK(threeLegs > oneLeg);
	CHECK(threeLegs >= 3 * 3);

	if(three != NULL){ freeGameComponentsFLWPN(three, data); }
	freeDataStructures(data);
}

/* Reset puts the whole chain back, not just the current leg. */
void test_flwpn_reset_goes_back_to_the_first_port(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(17);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 3, 1, 30, 2, data);
	CHECK(game != NULL);

	if(game != NULL){
		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 1);

		resetFLWPN(game, data);

		CHECK_INT(legsDoneFLWPN(game), 0);
		CHECK_INT(isGameWonFLWPN(game), 0);
		CHECK_INT(currentGoalFLWPN(game), game->goals[0]);
		CHECK_INT(getFLWPComponentsFLWPN(game)->prevInput, getFLWPComponentsFLWPN(game)->start);

		/*and it is walkable again from the top*/
		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 1);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* One leg is exactly the ordinary walk, which is the cheapest check that the
   wrapper has not changed what it wraps. */
void test_flwpn_with_one_port_is_an_ordinary_walk(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(23);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 4, 1, 30, 1, data);
	CHECK(game != NULL);

	if(game != NULL){
		CHECK_INT(legsTotalFLWPN(game), 1);
		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(isGameWonFLWPN(game), 1);
		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

void test_flwpn_refuses_a_chain_it_cannot_deal(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	/*nothing is a hundred moves from anything in this dictionary*/
	CHECK(initiateFLWPN(8, 30, 100, 100, 1, 30, 2, data) == NULL);
	/*and no ports at all is not a board*/
	CHECK(initiateFLWPN(8, 30, 2, 3, 1, 30, 0, data) == NULL);

	freeDataStructures(data);
}

void test_flwpn_survives_being_handed_nothing(void){
	CHECK(initiateFLWPN(1, 30, 1, 2, 1, 30, 2, NULL) == NULL);
	CHECK(getFLWPComponentsFLWPN(NULL) == NULL);
	CHECK_INT(isStartValidFLWPN(NULL), 0);
	CHECK_INT(currentGoalFLWPN(NULL), -1);
	CHECK_INT(legsDoneFLWPN(NULL), 0);
	CHECK_INT(legsTotalFLWPN(NULL), 0);
	CHECK_INT(shortestRouteFLWPN(NULL), 0);
	CHECK_INT(isGameWonFLWPN(NULL), 0);
	CHECK_INT(userEntersWordFLWPN("cart", NULL, NULL), -1);
	resetFLWPN(NULL, NULL);
	freeGameComponentsFLWPN(NULL, NULL);
}

void suite_flwpn(void){
	printf("\n-- the walk with more than one port --\n");
	RUN_TEST(test_flwpn_deals_as_many_ports_as_it_was_asked_for);
	RUN_TEST(test_flwpn_moves_the_goal_on_when_you_arrive);
	RUN_TEST(test_flwpn_a_port_walked_through_early_is_still_there_later);
	RUN_TEST(test_flwpn_keeps_every_port_that_is_still_to_come);
	RUN_TEST(test_flwpn_measures_the_whole_chain_not_the_first_leg);
	RUN_TEST(test_flwpn_reset_goes_back_to_the_first_port);
	RUN_TEST(test_flwpn_with_one_port_is_an_ordinary_walk);
	RUN_TEST(test_flwpn_refuses_a_chain_it_cannot_deal);
	RUN_TEST(test_flwpn_survives_being_handed_nothing);
}
