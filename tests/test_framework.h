/*
Title: test_framework.h
Description: A dependency-free assertion harness for the unit tests. Every check
records itself so the run ends with a count rather than dying on the first failure.
*/
#ifndef seenTestFramework
#define seenTestFramework

#include <stdio.h>
#include <string.h>

extern int tests_run;
extern int tests_failed;
extern int checks_run;
extern int checks_failed;

/*Starts a named test -- called for you by RUN_TEST*/
void test_start(const char* name);

/*Closes out the current test, printing whether it passed*/
void test_end(void);

/*Records a failed check against the test that is currently running*/
void test_fail(const char* file, int line, const char* what);

/*Prints the totals. Returns the process exit code: 0 when everything passed*/
int test_summary(void);

/*Run one test, or only the ones whose name contains the filter given on the
command line. The whole suite is eleven thousand checks against a four thousand
word dictionary, which is a slow way to ask about one test while fixing it:
	./flwo_tests traps_the_player
Without an argument everything runs, which is what CI and `make test` do.*/
extern const char* test_filter;

#define RUN_TEST(fn) do { 	if(test_filter == NULL || strstr(#fn, test_filter) != NULL){ 		test_start(#fn); fn(); test_end(); 	} } while(0)

#define CHECK(cond) do { \
	checks_run++; \
	if(!(cond)){ \
		checks_failed++; \
		test_fail(__FILE__, __LINE__, #cond); \
	} \
} while(0)

#define CHECK_INT(actual, expected) do { \
	long _a = (long)(actual); \
	long _e = (long)(expected); \
	checks_run++; \
	if(_a != _e){ \
		char _buf[512]; \
		checks_failed++; \
		snprintf(_buf, sizeof(_buf), "%s: got %ld, expected %ld", #actual, _a, _e); \
		test_fail(__FILE__, __LINE__, _buf); \
	} \
} while(0)

#define CHECK_STR(actual, expected) do { \
	const char* _a = (actual); \
	const char* _e = (expected); \
	checks_run++; \
	if(_a == NULL || _e == NULL || strcmp(_a, _e) != 0){ \
		char _buf[512]; \
		checks_failed++; \
		snprintf(_buf, sizeof(_buf), "%s: got \"%s\", expected \"%s\"", #actual, \
			(_a == NULL) ? "(null)" : _a, (_e == NULL) ? "(null)" : _e); \
		test_fail(__FILE__, __LINE__, _buf); \
	} \
} while(0)

#define CHECK_NULL(ptr) do { \
	checks_run++; \
	if((ptr) != NULL){ \
		checks_failed++; \
		test_fail(__FILE__, __LINE__, #ptr " should be NULL"); \
	} \
} while(0)

#define CHECK_NOT_NULL(ptr) do { \
	checks_run++; \
	if((ptr) == NULL){ \
		checks_failed++; \
		test_fail(__FILE__, __LINE__, #ptr " should not be NULL"); \
	} \
} while(0)

/*The suites, each defined in its own file*/
void suite_structs(void);
void suite_hashmap(void);
void suite_games(void);
void suite_regressions(void);
void suite_mcts(void);
void suite_modes(void);
void suite_debug(void);

/*The suites pick start words with rand(), so the seed is pinned to keep a
failing run reproducible*/
#define TEST_SEED 20260912

/*Shared helpers (test_helpers.c)*/
struct DataStructures;
/*Opens a dictionary and builds the data structures. The descriptor is closed
here, which also asserts that initDataStructures does not take it over*/
struct DataStructures* open_dictionary(const char* path, int numLetters);
/*Number of positions at which two equal-length words differ*/
int letters_that_differ(const char* a, const char* b, int numLetters);
/*The count on the first line of a dictionary file. Tests ask the file how many
words it holds rather than carrying the number themselves - the word list grows,
and a test that hardcodes 1952 fails for the one reason that is not a fault.*/
int declared_word_count(const char* path);

#endif
