/*
Title: test_flwpn.c
Description: The walk with more than one port of call.

Most of what this mode does is the ordinary walk, and the ordinary walk is
already tested. What is new is three things:

  - the goal moves on when you arrive rather than the game ending
  - a run is marked against ALL the legs, not the first one
  - ONCE A WORD IS TOUCHED IT IS SPENT

The third is the rule the mode is played under, and the walk underneath does not
enforce it. AddWord_Struct marks nothing used; the only word a walk ever spends
is the one it STARTED on, which a probe confirmed by playing a word, playing
away from it, and playing it again. So FLWPN spends the path itself.

Everything awkward follows from that rule rather than from the walk:

  A D B E C is a walk. A D B D C is not - D was already had.

  A later port cannot be touched early either, because touching it would spend
  it and no route could ever end on it again. The board would look perfectly
  healthy and be dead. So it is refused, with PORT_NOT_DUE.

  Which means a chain is only worth dealing when every leg can be walked without
  touching a later port or a word already spent. The graph does not know that -
  distance is measured through the open dictionary - so a chain can look fine
  and have a leg whose only routes run through a port that is not due.

And the spent words and the leg count are both DERIVED from the path after every
change rather than tracked through it, because undo has to undo both and the
pathfinder's undo has never heard of either.
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
static int routeAvoidingSpentWords(int from, int to, struct GameComponentsFLWPN* game,
	struct DataStructures* data, int* route, int max){
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
			/*nor a port whose turn has not come - the board refuses it, so a
			  route through one is not a route. Without this the walker plans a
			  way the rule forbids and then reports the refusal as a board that
			  could not be finished, which is a test blaming the thing it is
			  testing for its own bad planning.*/
			if(next != to){
				int j, laterPort = 0;
				for(j = legsDoneFLWPN(game); j < legsTotalFLWPN(game); j++){
					if(game->goals[j] == next){ laterPort = 1; break; }
				}
				if(laterPort){
					continue;
				}
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
	int length = routeAvoidingSpentWords(walk->prevInput, walk->goal, game, data, route, 64);
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

/* A port that has not had its turn is never spent, because it can never be
   touched - see the refusal test. This is the same statement from the other
   side: walking normally, nothing ahead of you is used up. */
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

/* THE RULE, in the words it was given in: A D B E C is a walk, A D B D C is not.
 *
 * Played on a real board rather than described: walk to the first port, then
 * try to play a word already on the path. The walk underneath would take it -
 * that is what the probe showed - so what refuses it is this mode.
 */
void test_flwpn_will_not_touch_a_word_twice(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(7);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 4, 1, 30, 3, data);
	CHECK(game != NULL);

	if(game != NULL){
		struct GameComponents* walk = getFLWPComponentsFLWPN(game);
		int startedOn = walk->start;

		CHECK(walkToTheNextPort(game, data) > 0);

		/*every word walked over is spent, the start included*/
		CHECK_INT((int)checkIfUsed_WordSet(startedOn, data->wordSet), 1);
		CHECK_INT((int)checkIfUsed_WordSet(game->goals[0], data->wordSet), 1);

		/*and playing one again is refused, wherever on the path it was*/
		struct intList* w = walk->userConnections;
		int refusals = 0, tried = 0;
		for(w = w->next; w != NULL; w = w->next){
			int again = userEntersWordFLWPN(convertIntToWord(w->data, data), game, data);
			tried++;
			if(again != VALID){
				refusals++;
			}
		}
		CHECK(tried >= 2);
		CHECK_INT(refusals, tried);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* A port that is not due is refused rather than allowed to kill the board.
 *
 * Under the rule above, touching C on the way to B would spend C and no route
 * could ever end on it again. The board would still look playable. So the move
 * does not happen, and it says which port is actually due.
 */
void test_flwpn_refuses_a_port_that_is_not_due(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	struct GameComponentsFLWPN* game = NULL;
	int seed;

	/*a board whose second port is a neighbour of the start, so it CAN be tried*/
	for(seed = 1; seed <= 60 && game == NULL; seed++){
		seedGameRandom(seed);
		struct GameComponentsFLWPN* candidate = initiateFLWPN(8, 30, 1, 2, 1, 30, 3, data);
		if(candidate == NULL){
			continue;
		}
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

		/*refused, and for the right reason*/
		CHECK_INT(userEntersWordFLWPN(convertIntToWord(second, data), game, data), PORT_NOT_DUE);

		/*nothing happened: not spent, not a leg, not a move*/
		CHECK_INT((int)checkIfUsed_WordSet(second, data->wordSet), 0);
		CHECK_INT(legsDoneFLWPN(game), 0);

		/*and the first port can still be reached, which is all this test is
		  about - whether a whole chain survives greedy routing is the subject
		  of test_flwpn_every_board_it_deals_can_be_finished, and on ports this
		  close together it is a question about the ROUTE the player picks
		  rather than about the board. See the note there.*/
		CHECK(walkToTheNextPort(game, data) >= 0);
		CHECK_INT(legsDoneFLWPN(game), 1);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* Undo puts back the word AND the leg.
 *
 * This is why both are read off the path instead of counted. An undo that
 * stepped back over an arrival would otherwise leave the port called at and the
 * word spent, and the board would be one port short for ever.
 */
void test_flwpn_undo_gives_back_the_word_and_the_port(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(11);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 3, 1, 30, 2, data);
	CHECK(game != NULL);

	if(game != NULL){
		int first = game->goals[0];

		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 1);
		CHECK_INT((int)checkIfUsed_WordSet(first, data->wordSet), 1);
		CHECK_INT(currentGoalFLWPN(game), game->goals[1]);

		/*step back over the arrival*/
		undoMoveFLWPN(game, data);

		CHECK_INT(legsDoneFLWPN(game), 0);
		CHECK_INT((int)checkIfUsed_WordSet(first, data->wordSet), 0);
		CHECK_INT(currentGoalFLWPN(game), first);
		CHECK_INT(isGameWonFLWPN(game), 0);

		/*and it can be walked again from there*/
		CHECK(walkToTheNextPort(game, data) >= 0);
		CHECK_INT(legsDoneFLWPN(game), 1);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

void test_flwpn_redo_takes_the_port_back(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	seedGameRandom(11);

	struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 3, 1, 30, 2, data);
	CHECK(game != NULL);

	if(game != NULL){
		CHECK(walkToTheNextPort(game, data) > 0);
		CHECK_INT(legsDoneFLWPN(game), 1);

		undoMoveFLWPN(game, data);
		CHECK_INT(legsDoneFLWPN(game), 0);

		redoMoveFLWPN(game, data);
		CHECK_INT(legsDoneFLWPN(game), 1);
		CHECK_INT((int)checkIfUsed_WordSet(game->goals[0], data->wordSet), 1);

		freeGameComponentsFLWPN(game, data);
	}

	freeDataStructures(data);
}

/* Every board dealt can actually be finished under the rule.
 *
 * Not "is connected in the graph" - walked, leg by leg, going around every word
 * already spent and never touching a port before its turn. That is a stricter
 * question than the dealer asks, and it is the one that matters.
 *
 * WHAT THIS DOES NOT PROMISE, and it is worth being straight about: the walker
 * here takes the shortest way to each port in turn, and taking the shortest way
 * to one port can spend words a later leg needed. The board is still winnable -
 * initiateFLWPN proved a way through before dealing it - but not by that
 * particular greedy route. That is the rule doing what the rule does: a player
 * can spend their way into a corner, and on ports one move apart they easily
 * can. It is a real property of the mode rather than a bug, and the place it
 * has to be decided is the tier tables, by keeping the ports far enough apart
 * that a reasonable route is not a trap.
 */
void test_flwpn_every_board_it_deals_can_be_finished(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int dealt = 0, finished = 0, seed;

	for(seed = 1; seed <= 25; seed++){
		seedGameRandom(seed);
		struct GameComponentsFLWPN* game = initiateFLWPN(8, 30, 2, 3, 1, 30, 3, data);
		if(game == NULL){
			resetWordSet(data);
			continue;
		}
		dealt++;

		int leg, ok = 1;
		for(leg = 0; leg < 3 && ok; leg++){
			if(walkToTheNextPort(game, data) < 0){
				ok = 0;
			}
		}
		if(ok && isGameWonFLWPN(game)){
			finished++;
		}

		freeGameComponentsFLWPN(game, data);
		resetWordSet(data);
	}

	CHECK(dealt > 15);
	CHECK_INT(finished, dealt);

	freeDataStructures(data);
}

void suite_flwpn(void){
	printf("\n-- the walk with more than one port --\n");
	RUN_TEST(test_flwpn_deals_as_many_ports_as_it_was_asked_for);
	RUN_TEST(test_flwpn_moves_the_goal_on_when_you_arrive);
	RUN_TEST(test_flwpn_keeps_every_port_that_is_still_to_come);
	RUN_TEST(test_flwpn_measures_the_whole_chain_not_the_first_leg);
	RUN_TEST(test_flwpn_reset_goes_back_to_the_first_port);
	RUN_TEST(test_flwpn_with_one_port_is_an_ordinary_walk);
	RUN_TEST(test_flwpn_refuses_a_chain_it_cannot_deal);
	RUN_TEST(test_flwpn_will_not_touch_a_word_twice);
	RUN_TEST(test_flwpn_refuses_a_port_that_is_not_due);
	RUN_TEST(test_flwpn_undo_gives_back_the_word_and_the_port);
	RUN_TEST(test_flwpn_redo_takes_the_port_back);
	RUN_TEST(test_flwpn_every_board_it_deals_can_be_finished);
	RUN_TEST(test_flwpn_survives_being_handed_nothing);
}
