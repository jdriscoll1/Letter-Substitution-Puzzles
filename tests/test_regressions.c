/*
Title: test_regressions.c
Description: One test per bug fixed in the memory-leak pass. The leaks themselves
only show up under valgrind (make test-memcheck), so these pin down the behaviour
around them: the paths that used to leak still return what callers expect, and the
two memory-safety bugs are gone.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/flwc/includes/Challenges.h"
#include "../src/flwg/includes/FLWGGame.h"
#include "../src/flwp/includes/GameFunctions.h"
#include "../src/flwp/includes/UserInput.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"

/*initDataStructures used to hand its descriptor to fdopen and then fclose it,
so every close(fd) that followed was a stale double close on a number the next
open() could hand back to something else*/
static void test_init_leaves_the_caller_owning_the_fd(void){
	int fd = open("docs/2.txt", O_RDONLY);
	struct DataStructures* data;
	int closeResult;

	CHECK(fd != -1);
	data = initDataStructures(fd, 2);
	CHECK_NOT_NULL(data);

	/*The descriptor is still the caller's to close*/
	closeResult = close(fd);
	CHECK_INT(closeResult, 0);

	/*and closing it does not disturb the structures that were built from it*/
	CHECK_INT(data->I2W->numWords, 29);
	CHECK_NOT_NULL(Convert_IntToWord(0, data->I2W));

	freeDataStructures(data);
}

/*The hint guards tested solution->list, which malloc(0) makes non-NULL, so a
goal that could not be reached fell through to a read at index -1*/
static void test_flwc_hints_handle_an_unreachable_goal(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* noWords[] = {NULL};
	struct WordSet* empty = convertCharPtrPtrToWordSet(noWords, data);
	struct GameComponentsFLWC components;
	char* path;

	/*An empty goal set means the search can never succeed*/
	components.wordId = 0;
	components.goalWords = empty;
	components.avoidWords = empty;

	CHECK_NULL(hintGoalWordFLWC(&components, data));
	CHECK_NULL(hintAdjacencyTowardsGoalFLWC(&components, data));
	path = hintPathToGoalFLWC(&components, data);
	CHECK_NULL(path);
	free(path);
	CHECK_INT(hintMinDistanceToGoalFLWC(&components, data), -1);
	CHECK_INT(hintDistanceFromNearestAvoidWordFLWIC(&components, data), -1);

	free_WordSet(empty);
	freeDataStructures(data);
}

/*A goal word that is reachable still produces a usable hint, so the guard above
did not simply switch the hints off*/
static void test_flwc_hints_still_answer_a_reachable_goal(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goalWords[] = {"care", NULL};
	char* noWords[] = {NULL};
	struct GameComponentsFLWC components;
	char* path;

	components.wordId = Convert_WordToInt("ware", data);
	components.goalWords = convertCharPtrPtrToWordSet(goalWords, data);
	components.avoidWords = convertCharPtrPtrToWordSet(noWords, data);
	CHECK(components.wordId != -1);

	/*ware -> care is a single substitution*/
	CHECK_STR(hintGoalWordFLWC(&components, data), "care");
	CHECK_STR(hintAdjacencyTowardsGoalFLWC(&components, data), "care");
	CHECK_INT(hintMinDistanceToGoalFLWC(&components, data), 1);

	path = hintPathToGoalFLWC(&components, data);
	CHECK_NOT_NULL(path);
	if(path != NULL){
		CHECK(strstr(path, "ware") != NULL);
		CHECK(strstr(path, "care") != NULL);
	}
	free(path);

	free_WordSet(components.goalWords);
	free_WordSet(components.avoidWords);
	freeDataStructures(data);
}

/*Both start pickers leaked their array list on this path*/
static void test_choose_start_reports_no_match(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	/*No word has anywhere near this many adjacencies*/
	CHECK_INT(ChooseStart_Range(data->I2W, 999, 1000), -1);
	CHECK_INT(ChooseStart(data->I2W, 999), -1);

	/*A range that does match still returns a word inside it*/
	int chosen = ChooseStart_Range(data->I2W, 4, 4);
	CHECK(chosen != -1);
	if(chosen != -1){
		CHECK_INT(getNumAdjacencies(chosen, data), 4);
	}

	freeDataStructures(data);
}

/*botPly returned -1 before freeing its three minimax nodes*/
static void test_botply_gives_up_when_every_word_is_used(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int start = Convert_WordToInt("ware", data);
	int i;

	/*With the whole dictionary claimed there is nowhere to move*/
	for(i = 0; i < data->I2W->numWords; i++){
		markUsed_WordSet(i, data->wordSet);
	}
	CHECK_INT(isTrapped(start, data), 1);
	CHECK_INT(botPly(start, 4, data->I2W, data->wordSet), -1);

	/*With a clear board it finds a legal move again*/
	reset_WordSet(data->wordSet);
	markUsed_WordSet(start, data->wordSet);
	int played = botPly(start, 4, data->I2W, data->wordSet);
	CHECK(played != -1);
	if(played != -1){
		CHECK_INT(letters_that_differ(Convert_IntToWord(start, data->I2W),
			Convert_IntToWord(played, data->I2W), 4), 1);
	}

	freeDataStructures(data);
}

/*Fill_HashMaps dropped the wordStruct for every duplicate key. Reading a four
letter dictionary as three letters truncates words into duplicates, which is the
cheapest way to drive that path*/
static void test_duplicate_keys_do_not_break_the_map(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 3);
	int id;

	/*Every word is truncated to three letters*/
	CHECK_INT(data->I2W->numWords, 1952);
	CHECK_INT(data->I2W->numLetters, 3);
	CHECK_INT((int)strlen(Convert_IntToWord(0, data->I2W)), 3);
	CHECK_STR(Convert_IntToWord(0, data->I2W), "war");

	/*The first of the duplicates is the one the tree kept, and it still resolves*/
	id = Convert_WordToInt("war", data);
	CHECK(id != -1);
	if(id != -1){
		CHECK_STR(Convert_IntToWord(id, data->I2W), "war");
	}

	freeDataStructures(data);
}

/*removeWord_FLWP dropped the string RemoveWord_Struct hands back*/
static void test_remove_word_rewinds_the_path(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* gc = initiateFLWP(4, 16, 4, 8, 4, 16, data);
	int secondWord = gc->solution->next->next->data;
	char* secondWordText = Convert_IntToWord(secondWord, data->I2W);

	CHECK_INT(userEntersWord_FLWP(secondWordText, gc, data), VALID);
	CHECK_INT(getPrevWordFLWP(gc), secondWord);

	/*Removing the word puts the path back to the start word*/
	removeWord_FLWP(secondWordText, gc, data);
	CHECK_INT(getPrevWordFLWP(gc), gc->start);

	freeGameComponentsFLWP(gc, data);
	freeDataStructures(data);
}

void suite_regressions(void){
	printf("\n-- regressions --\n");
	RUN_TEST(test_init_leaves_the_caller_owning_the_fd);
	RUN_TEST(test_flwc_hints_handle_an_unreachable_goal);
	RUN_TEST(test_flwc_hints_still_answer_a_reachable_goal);
	RUN_TEST(test_choose_start_reports_no_match);
	RUN_TEST(test_botply_gives_up_when_every_word_is_used);
	RUN_TEST(test_duplicate_keys_do_not_break_the_map);
	RUN_TEST(test_remove_word_rewinds_the_path);
}
