/*
Title: test_flwg3_timing.c
Description: How long a seat at the table takes to think.

NOT A CORRECTNESS TEST. It is a bench, kept with the suite because the numbers
it prints are what the campaign's depths are chosen from and they go stale the
moment the search or the dictionary changes. A three handed board plays TWO
engine moves for every one of the player's, so a seat that takes a second costs
the player two, and the game's other bots answer instantly.

Run it by name: ./flwo_tests timing
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/flwg3/includes/FLWG3.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/IntLinkedList.h"

static double secondsSince(clock_t from){
	return (double)(clock() - from) / (double)CLOCKS_PER_SEC;
}

/*One board played out, reporting the slowest single ply any seat took.*/
static void bench(const char* dictionary, int numLetters, int seats, int depth){
	struct DataStructures* data = open_dictionary(dictionary, numLetters);
	Load_Obscurity(data->I2W, numLetters == 4 ? "docs/4ranks.txt"
		: numLetters == 3 ? "docs/3ranks.txt" : "docs/2ranks.txt");
	setObscurityCap(data, 50000);

	int enginePlays = 0;
	int seat;
	for(seat = 1; seat < seats; seat++){
		enginePlays |= (1 << seat);
	}

	struct GameComponentsFLWG3* game = initiateFLWG3(seats, enginePlays, depth,
		numLetters == 2 ? 12 : numLetters == 3 ? 19 : 16, 35, data);

	double worst = 0;
	double total = 0;
	int plies = 0;
	int moves;

	for(moves = 0; moves < 30 && isGameWonFLWG3(game) == 0; moves++){
		if(seatIsStrandedFLWG3(game, data)){
			break;
		}
		if(whoseTurnFLWG3(game) == 0){
			struct intList* option;
			for(option = getConnections(game->currWordId, data->I2W)->next;
			    option != NULL; option = option->next){
				if(checkIfUsed_WordSet(option->data, data->wordSet)){
					continue;
				}
				if(userEntersWordFLWG3(Convert_IntToWord(option->data, data->I2W),
					game, data) == VALID){
					break;
				}
			}
			continue;
		}

		clock_t began = clock();
		int played = botTakesTurnFLWG3(game, data);
		double took = secondsSince(began);
		if(took > worst){
			worst = took;
		}
		total += took;
		plies++;
		if(played == -1){
			break;
		}
	}

	printf("    %d letters, %d seats, depth %d:  %d plies, worst %.0f ms, mean %.0f ms\n",
		numLetters, seats, depth, plies, worst * 1000.0,
		plies > 0 ? (total / plies) * 1000.0 : 0.0);

	freeGameComponentsFLWG3(game);
	freeDataStructures(data);
	/*Nothing is asserted about the numbers - they are data, and the campaign is
	  what reads them. What IS asserted is that a board of this shape plays at
	  all, so a shape nobody can deal cannot sit here printing zeroes.*/
	CHECK(plies > 0);
}

void test_flwg3_timing_of_a_seat(void){
	printf("\n");
	bench("docs/2.txt", 2, 3, 2);
	bench("docs/2.txt", 2, 3, 4);
	bench("docs/3.txt", 3, 3, 1);
	bench("docs/3.txt", 3, 3, 2);
	bench("docs/3.txt", 3, 3, 3);
	bench("docs/3.txt", 3, 3, 4);
	bench("docs/4.txt", 4, 3, 1);
	bench("docs/4.txt", 4, 3, 2);
	bench("docs/4.txt", 4, 3, 3);
	bench("docs/4.txt", 4, 3, 4);
	bench("docs/4.txt", 4, 4, 2);
	bench("docs/4.txt", 4, 4, 3);
	bench("docs/4.txt", 4, 5, 2);
	bench("docs/3.txt", 3, 5, 3);
}

void suite_flwg3_timing(void){
	printf("\n-- how long a seat takes to think --\n");
	RUN_TEST(test_flwg3_timing_of_a_seat);
}
