/*
Title: test_helpers.c
Description: The harness itself plus the helpers the suites share.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"

int tests_run = 0;
int tests_failed = 0;
int checks_run = 0;
int checks_failed = 0;

static const char* current_test = NULL;
static int current_test_failures = 0;

void test_start(const char* name){
	current_test = name;
	current_test_failures = 0;
	tests_run++;
}

void test_end(void){
	if(current_test_failures == 0){
		printf("  [PASS] %s\n", current_test);
	}
	else{
		printf("  [FAIL] %s (%d failed check(s))\n", current_test, current_test_failures);
		tests_failed++;
	}
	current_test = NULL;
}

void test_fail(const char* file, int line, const char* what){
	current_test_failures++;
	printf("    %s:%d: %s\n", file, line, what);
}

int test_summary(void){
	printf("\n=====================================================\n");
	printf("%d test(s), %d failed | %d check(s), %d failed\n",
		tests_run, tests_failed, checks_run, checks_failed);
	printf("%s\n", (tests_failed == 0) ? "ALL TESTS PASSED" : "TESTS FAILED");
	printf("=====================================================\n");
	return (tests_failed == 0) ? 0 : 1;
}

struct DataStructures* open_dictionary(const char* path, int numLetters){
	int fd = open(path, O_RDONLY);
	if(fd == -1){
		printf("\nCould not open %s -- run the tests from the repository root\n", path);
		exit(1);
	}
	struct DataStructures* data = initDataStructures(fd, numLetters);
	/*The caller owns the descriptor. Whether that actually holds is asserted by
	test_init_leaves_the_caller_owning_the_fd, so the result is ignored here*/
	close(fd);
	/*initDataStructures calls srand(time(NULL)) itself, so the seed has to be
	put back afterwards or the tests would differ from run to run*/
	srand(TEST_SEED);
	return data;
}

int letters_that_differ(const char* a, const char* b, int numLetters){
	int differences = 0;
	int i;
	for(i = 0; i < numLetters; i++){
		if(a[i] != b[i]){
			differences++;
		}
	}
	return differences;
}
