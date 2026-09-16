/*
Title: test_main.c
Description: Entry point for the unit tests. Run from the repository root, since
the suites load the dictionaries from docs/ by relative path:

	make test

The seed is fixed so a failure can be reproduced. open_dictionary re-applies it
after every load, because initDataStructures reseeds from the clock itself.
*/
#include <stdio.h>
#include <stdlib.h>

#include "test_framework.h"

void suite_api_surface(void);
void suite_bridge(void);

int main(int argc, char** argv){
	/*An optional substring: only tests whose name contains it are run*/
	if(argc > 1){
		test_filter = argv[1];
	}
	/*Fixed seed: the game APIs pick start words with rand()*/
	srand(TEST_SEED);

	printf("=====================================================\n");
	printf("Letter Substitution Puzzles -- unit tests\n");
	printf("=====================================================\n");

	suite_structs();
	suite_hashmap();
	suite_games();
	suite_mcts();
	suite_modes();
	suite_debug();
	suite_regressions();
	suite_api_surface();
	suite_bridge();

	return test_summary();
}
