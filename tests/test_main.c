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

int main(void){
	/*Fixed seed: the game APIs pick start words with rand()*/
	srand(TEST_SEED);

	printf("=====================================================\n");
	printf("Letter Substitution Puzzles -- unit tests\n");
	printf("=====================================================\n");

	suite_structs();
	suite_hashmap();
	suite_games();
	suite_mcts();
	suite_regressions();

	return test_summary();
}
