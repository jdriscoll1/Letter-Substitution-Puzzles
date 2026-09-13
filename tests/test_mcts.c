/*
Title: test_mcts.c
Description: Tests for the Monte Carlo tree search. The search is random, so these
assert the invariants that have to hold on every run -- the move is legal, the
shared word set is left exactly as it was found, and the pieces (rollout, node
expansion, the UCT score) behave as their comments describe -- rather than pinning
down which word comes back.

docs/2.txt is used throughout: it is 29 words, which keeps a search fast enough
for a unit test.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/algs/includes/MontyCarlosTreeSearch.h"
/*chooseRandom, the rollout policy, lives here*/
#include "../src/algs/includes/MinimaxTests.h"
#include "../src/flwg/includes/FLWGGame.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"
#include "../src/structs/includes/WordSet.h"

/*Is every word marked exactly as it was before the search ran?*/
static int word_sets_match(struct WordSet* a, struct WordSet* b, int numWords){
	int id;
	for(id = 0; id < numWords; id++){
		if((checkIfUsed_WordSet(id, a) != 0) != (checkIfUsed_WordSet(id, b) != 0)){
			return 0;
		}
	}
	return 1;
}

/*Is the word a direct, unused neighbour of id?*/
static int is_legal_move(int id, int move, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	while(adjacency != NULL){
		if(adjacency->data == move){
			return 1;
		}
		adjacency = adjacency->next;
	}
	return 0;
}

static void test_mcts_returns_a_legal_move(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	int start = 0;
	int move;

	markUsed_WordSet(start, data->wordSet);
	move = montyCarlosTreeSearch(start, data->wordSet, data->I2W);

	/*It has to hand back a word one substitution away that nobody has played*/
	CHECK(move != -1);
	CHECK_INT(is_legal_move(start, move, data), 1);
	CHECK_INT(letters_that_differ(Convert_IntToWord(start, data->I2W),
		Convert_IntToWord(move, data->I2W), 2), 1);

	freeDataStructures(data);
}

static void test_mcts_leaves_the_word_set_untouched(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	struct WordSet* before;
	int start = 0;

	/*Play a couple of words so the set is not trivially empty*/
	markUsed_WordSet(start, data->wordSet);
	markUsed_WordSet(3, data->wordSet);
	before = copyWordSetDeep(data->wordSet);

	montyCarlosTreeSearch(start, data->wordSet, data->I2W);

	/*The search marks words as it descends and unmarks them on the way back up.
	Anything left behind would silently remove words from the real game*/
	CHECK_INT(word_sets_match(before, data->wordSet, data->I2W->numWords), 1);

	free_WordSet(before);
	freeDataStructures(data);
}

static void test_mcts_gives_up_when_there_is_no_move(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	int start = 0;
	int i;

	/*Every word claimed, so the search has nothing to pick*/
	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}
	CHECK_INT(montyCarlosTreeSearch(start, data->wordSet, data->I2W), -1);

	freeDataStructures(data);
}

static void test_mcts_takes_the_only_option(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	int start = 0;
	struct intList* adjacency = getConnections(start, data->I2W)->next;
	int keep = adjacency->data;
	int move;

	/*Claim every neighbour but one, so there is exactly one legal answer*/
	markUsed_WordSet(start, data->wordSet);
	adjacency = adjacency->next;
	while(adjacency != NULL){
		markUsed_WordSet(adjacency->data, data->wordSet);
		adjacency = adjacency->next;
	}

	move = montyCarlosTreeSearch(start, data->wordSet, data->I2W);
	CHECK_INT(move, keep);

	freeDataStructures(data);
}

static void test_visit_expands_only_unused_neighbours(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	int start = 0;
	struct mctsStruct root;
	struct intList* adjacency;
	int expected = 0;
	int i;
	int usedChildren = 0;
	int notNeighbours = 0;
	/*Nodes are carved out of a pool rather than malloc'd one at a time, so the
	tree is torn down by closing the pool -- there is no free for a single node*/
	struct mctsPool* pool = create_mctsPool();

	/*Claim the start and one of its neighbours*/
	markUsed_WordSet(start, data->wordSet);
	adjacency = getConnections(start, data->I2W)->next;
	markUsed_WordSet(adjacency->data, data->wordSet);

	/*How many neighbours are left*/
	adjacency = getConnections(start, data->I2W)->next;
	while(adjacency != NULL){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			expected++;
		}
		adjacency = adjacency->next;
	}
	CHECK(expected > 0);

	root.player = 0;
	root.wordID = start;
	root.score = 0;
	root.visits = 0;
	root.numChildren = 0;
	root.parent = NULL;
	root.numWins = 0;
	root.children = NULL;

	visit_mctsStruct(start, &root, 2, pool, data->wordSet, data->I2W);

	/*A node is expanded into exactly the moves that are still available*/
	CHECK_INT(root.numChildren, expected);
	CHECK_INT(root.visits, 1);

	for(i = 0; i < root.numChildren; i++){
		int childId = root.children[i]->wordID;
		if(checkIfUsed_WordSet(childId, data->wordSet)){
			usedChildren++;
		}
		if(!is_legal_move(start, childId, data)){
			notNeighbours++;
		}
		CHECK_INT(root.children[i]->parent == &root, 1);
		/*The child is one ply on, so the turn has passed to the other side*/
		CHECK_INT(root.children[i]->player, 1);
	}
	/*A word that has already been played is not a move*/
	CHECK_INT(usedChildren, 0);
	CHECK_INT(notNeighbours, 0);

	free_mctsPool(pool);
	freeDataStructures(data);
}

static void test_rollout_scores_a_finished_game(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	struct WordSet* before;
	int start = 0;
	int i;
	int result;

	/*Every word used: whoever has to move has already lost*/
	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}
	/*A playout that cannot move names whoever was on turn as the loser, and that
	is whoever the caller said it was -- the seat, not a fixed "the opponent"*/
	CHECK_INT(rollout(start, 10, 0, 2, data->wordSet, data->I2W), 0);
	CHECK_INT(rollout(start, 10, 1, 2, data->wordSet, data->I2W), 1);
	CHECK_INT(rollout(start, 10, 2, 3, data->wordSet, data->I2W), 2);

	/*A playout from an open position has to leave the set as it found it*/
	reset_WordSet(data->wordSet);
	markUsed_WordSet(start, data->wordSet);
	before = copyWordSetDeep(data->wordSet);
	result = rollout(start, 1000, 0, 2, data->wordSet, data->I2W);
	CHECK(result == 0 || result == 1);
	CHECK_INT(word_sets_match(before, data->wordSet, data->I2W->numWords), 1);

	/*No depth left means no verdict*/
	CHECK_INT(rollout(start, 0, 0, 2, data->wordSet, data->I2W), -1);

	free_WordSet(before);
	freeDataStructures(data);
}

static void test_rollout_policy_is_uniform(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int word = Convert_WordToInt("ware", data);
	int total = getNumAdjacencies(word, data);
	int* ids = malloc(sizeof(int) * total);
	struct intList* adjacency;
	int first, last, i;
	int firstCount = 0;
	int lastCount = 0;
	int other = 0;
	const int draws = 20000;

	i = 0;
	for(adjacency = getConnections(word, data->I2W)->next; adjacency != NULL; adjacency = adjacency->next){
		ids[i++] = adjacency->data;
	}
	CHECK(total >= 4);

	/*Leave the first and last adjacency free and play everything between them.
	The old policy drew an index and probed forward for the first free word, which
	handed the whole run's chances to the word after it -- it picked the last one
	95.85% of the time here. Both should come up about half the time.*/
	for(i = 1; i < total - 1; i++){
		markUsed_WordSet(ids[i], data->wordSet);
	}
	first = ids[0];
	last = ids[total - 1];

	for(i = 0; i < draws; i++){
		int pick = chooseRandom(word, data->I2W, data->wordSet);
		if(pick == first){ firstCount++; }
		else if(pick == last){ lastCount++; }
		else{ other++; }
	}

	/*Never a word that was already played*/
	CHECK_INT(other, 0);
	/*A fair coin over 20000 draws lands far inside this band; the old policy was
	nowhere near it*/
	CHECK(firstCount > draws * 0.4);
	CHECK(firstCount < draws * 0.6);
	CHECK(lastCount > draws * 0.4);
	CHECK(lastCount < draws * 0.6);

	free(ids);
	freeDataStructures(data);
}

/*------------------------------------------------------------------------------
Three player FLWG: one MCTS, one bot that plays at random, one that always moves
to the neighbour with the fewest adjacencies left. Ordinary FLWG rules -- a player
who cannot move loses and the other two win -- so a player with no skill at all
loses one game in three. The claim under test is that the MCTS loses far fewer.

The three player game is what separates the search from the two player one: what a
play-out proves depends on how many seats sit between this move and the one that
strands somebody. Told there are two players when there are three, the search
credits most of its play-outs to the wrong player, and it was measurably the worst
of the three bots here -- 35 losses out of 59 on docs/3.txt where 20 is chance.
------------------------------------------------------------------------------*/
#define BOT_MCTS 0
#define BOT_RANDOM 1
#define BOT_MIN_ADJACENCIES 2
#define NUM_BOTS 3

/*How many of a word's adjacencies nobody has played yet*/
static int unused_adjacencies(int id, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	int available = 0;
	while(adjacency != NULL){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			available++;
		}
		adjacency = adjacency->next;
	}
	return available;
}

/*The opposite of botPly_MaxAdjacencies: take the move that leaves the next player
the least room. A greedy way to play FLWG, and a real opponent rather than a foil*/
static int choose_min_adjacencies(int id, struct DataStructures* data){
	struct intList* adjacency = getConnections(id, data->I2W)->next;
	int best = -1;
	int fewest = 0;
	while(adjacency != NULL){
		if(!checkIfUsed_WordSet(adjacency->data, data->wordSet)){
			int available = unused_adjacencies(adjacency->data, data);
			if(best == -1 || available < fewest){
				best = adjacency->data;
				fewest = available;
			}
		}
		adjacency = adjacency->next;
	}
	return best;
}

/*Plays one turn, marking the word it plays. -1 means this bot is stuck and lost*/
static int three_player_turn(int bot, int word, struct DataStructures* data){
	int move;
	if(bot == BOT_MCTS){
		/*The search always calls itself player 0; the other two follow it round*/
		move = montyCarlosTreeSearch_Multiplayer(word, NUM_BOTS, data->wordSet, data->I2W);
	}
	else if(bot == BOT_RANDOM){
		move = chooseRandom(word, data->I2W, data->wordSet);
	}
	else{
		move = choose_min_adjacencies(word, data);
	}
	if(move != -1){
		markUsed_WordSet(move, data->wordSet);
	}
	return move;
}

/*Plays a game out from start and returns the bot that ran out of moves*/
static int play_three_player_game(int start, int* seats, struct DataStructures* data){
	int word = start;
	int seat = 0;
	reset_WordSet(data->wordSet);
	markUsed_WordSet(start, data->wordSet);
	for(;;){
		int move = three_player_turn(seats[seat], word, data);
		if(move == -1){
			return seats[seat];
		}
		word = move;
		seat = (seat + 1) % NUM_BOTS;
	}
}

static void test_mcts_wins_a_three_player_game(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	/*Every seating, so nobody keeps whatever edge moving first or last is worth*/
	int seatings[6][NUM_BOTS] = {
		{BOT_MCTS, BOT_RANDOM, BOT_MIN_ADJACENCIES},
		{BOT_MCTS, BOT_MIN_ADJACENCIES, BOT_RANDOM},
		{BOT_RANDOM, BOT_MCTS, BOT_MIN_ADJACENCIES},
		{BOT_MIN_ADJACENCIES, BOT_MCTS, BOT_RANDOM},
		{BOT_RANDOM, BOT_MIN_ADJACENCIES, BOT_MCTS},
		{BOT_MIN_ADJACENCIES, BOT_RANDOM, BOT_MCTS}
	};
	const int games = 60;
	int losses[NUM_BOTS] = {0, 0, 0};
	int played = 0;
	int game;

	for(game = 0; game < games; game++){
		/*A different start each time, stepped by a stride coprime to the count*/
		int start = (game * 7) % data->I2W->numWords;
		if(getNumAdjacencies(start, data) == 0){
			continue;
		}
		losses[play_three_player_game(start, seatings[game % 6], data)]++;
		played++;
	}

	printf("    %d games: mcts lost %d, random lost %d, min adjacencies lost %d\n",
		played, losses[BOT_MCTS], losses[BOT_RANDOM], losses[BOT_MIN_ADJACENCIES]);

	CHECK(played > 0);
	/*Every game ends with exactly one bot stuck*/
	CHECK_INT(losses[BOT_MCTS] + losses[BOT_RANDOM] + losses[BOT_MIN_ADJACENCIES], played);
	/*It has to beat both opponents, not just the weaker one*/
	CHECK(losses[BOT_MCTS] < losses[BOT_RANDOM]);
	CHECK(losses[BOT_MCTS] < losses[BOT_MIN_ADJACENCIES]);
	/*And beat the one-in-three a bot with no skill would lose, with room to
	spare -- a third of the games is the bar for playing no worse than chance*/
	CHECK(losses[BOT_MCTS] * 3 < played);

	freeDataStructures(data);
}

static void test_uct_score_matches_the_formula(void){
	struct mctsStruct node;
	double expected;
	double actual;

	/*S_i = x_i + c * sqrt(ln(t) / n_i), with c = 5*/
	node.numWins = 3;
	node.visits = 4;
	expected = (3.0 / 4.0) + 5.0 * sqrt(log(100.0) / 4.0);
	actual = calculate_mctsScore(&node, 100);
	CHECK(fabs(actual - expected) < 1e-9);

	/*More wins over the same visits is worth more*/
	node.numWins = 1;
	CHECK(calculate_mctsScore(&node, 100) < actual);

	/*A node visited less often gets a bigger exploration bonus*/
	node.numWins = 1;
	node.visits = 1;
	CHECK(calculate_mctsScore(&node, 100) > calculate_mctsScore(&node, 100) - 1);
	{
		struct mctsStruct rare = node;
		struct mctsStruct common = node;
		rare.visits = 2;
		rare.numWins = 1;
		common.visits = 50;
		common.numWins = 25;
		/*Same win rate, so the less explored node has to score higher*/
		CHECK(calculate_mctsScore(&rare, 100) > calculate_mctsScore(&common, 100));
	}
}

void suite_mcts(void){
	printf("\n-- monte carlo tree search --\n");
	RUN_TEST(test_mcts_returns_a_legal_move);
	RUN_TEST(test_mcts_leaves_the_word_set_untouched);
	RUN_TEST(test_mcts_gives_up_when_there_is_no_move);
	RUN_TEST(test_mcts_takes_the_only_option);
	RUN_TEST(test_visit_expands_only_unused_neighbours);
	RUN_TEST(test_rollout_scores_a_finished_game);
	RUN_TEST(test_rollout_policy_is_uniform);
	RUN_TEST(test_uct_score_matches_the_formula);
	RUN_TEST(test_mcts_wins_a_three_player_game);
}
