/*
Title: test_structs.c
Description: Unit tests for the hand-rolled containers in src/structs.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/structs/includes/WordSet.h"
#include "../src/structs/includes/ArrayList.h"
#include "../src/structs/includes/IntLinkedList.h"

static void test_wordset_marks_and_clears(void){
	struct WordSet* set = init_WordSet(200);
	CHECK_NOT_NULL(set);
	CHECK_INT(set->totalWords, 200);

	/*Nothing is used to begin with, including across block boundaries*/
	CHECK_INT(checkIfUsed_WordSet(0, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(63, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(64, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(199, set) != 0, 0);

	/*A word is used once it is marked, and only that word*/
	markUsed_WordSet(64, set);
	CHECK_INT(checkIfUsed_WordSet(64, set) != 0, 1);
	CHECK_INT(checkIfUsed_WordSet(63, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(65, set) != 0, 0);

	/*Marking twice is not a toggle*/
	markUsed_WordSet(64, set);
	CHECK_INT(checkIfUsed_WordSet(64, set) != 0, 1);

	markUnused_WordSet(64, set);
	CHECK_INT(checkIfUsed_WordSet(64, set) != 0, 0);

	/*Reset clears every block, not just the first*/
	markUsed_WordSet(1, set);
	markUsed_WordSet(100, set);
	markUsed_WordSet(199, set);
	reset_WordSet(set);
	CHECK_INT(checkIfUsed_WordSet(1, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(100, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(199, set) != 0, 0);

	free_WordSet(set);
}

static void test_wordset_rejects_out_of_range(void){
	struct WordSet* set = init_WordSet(10);
	printf("    (the bounds warnings below are the behaviour under test)\n");

	/*Out of range ids are refused rather than corrupting a neighbouring block*/
	markUsed_WordSet(-1, set);
	markUsed_WordSet(10, set);
	markUsed_WordSet(99999, set);
	CHECK_INT(checkIfUsed_WordSet(0, set) != 0, 0);
	CHECK_INT(checkIfUsed_WordSet(9, set) != 0, 0);

	free_WordSet(set);
}

static void test_wordset_deep_copy_is_independent(void){
	struct WordSet* original = init_WordSet(128);
	struct WordSet* copy;

	markUsed_WordSet(3, original);
	markUsed_WordSet(127, original);

	copy = copyWordSetDeep(original);
	CHECK_NOT_NULL(copy);
	CHECK_INT(copy->totalWords, original->totalWords);
	CHECK_INT(checkIfUsed_WordSet(3, copy) != 0, 1);
	CHECK_INT(checkIfUsed_WordSet(127, copy) != 0, 1);

	/*Writing to one must not reach the other -- getPathToNearestWordInWordSet
	depends on this to explore without disturbing the live game*/
	markUsed_WordSet(50, copy);
	CHECK_INT(checkIfUsed_WordSet(50, original) != 0, 0);
	markUnused_WordSet(3, original);
	CHECK_INT(checkIfUsed_WordSet(3, copy) != 0, 1);

	free_WordSet(copy);
	free_WordSet(original);
}

static void test_arraylist_grows_past_initial_size(void){
	struct arrayList* aList = init_ArrayList(2, 2, NUM);
	int i;
	int value;

	CHECK_INT(aList->currPrecision, 0);

	/*Add well past the initial capacity so the realloc path is exercised*/
	for(i = 0; i < 50; i++){
		value = i * 3;
		add_ArrayList((void*)&value, aList, NUM);
	}
	CHECK_INT(aList->currPrecision, 50);
	CHECK(aList->currSize >= 50);

	/*Everything added is still readable and in order*/
	for(i = 0; i < 50; i++){
		CHECK_INT(((int*)(aList->list))[i], i * 3);
	}

	free_ArrayList(aList);
}

static void test_arraylist_builds_a_path_string(void){
	struct arrayList* aList = init_ArrayList(4, 4, STR);

	addString_ArrayList("pies", 4, aList);
	addString_ArrayList("->", 2, aList);
	addString_ArrayList("ties", 4, aList);

	CHECK_STR((char*)aList->list, "pies->ties");
	CHECK_INT(aList->currPrecision, 10);

	free_ArrayList(aList);
}

static void test_intll_add_find_and_remove(void){
	struct intList* list = init_IntLL();

	CHECK_INT(list->size, 0);

	AddToBack_IntLL(10, list);
	AddToBack_IntLL(20, list);
	AddToBack_IntLL(30, list);
	CHECK_INT(list->size, 3);
	CHECK_INT(list->next->data, 10);
	CHECK_INT(FindLast_IntLL(list), 30);

	AddToFront_IntLL(5, list);
	CHECK_INT(list->size, 4);
	CHECK_INT(list->next->data, 5);
	CHECK_INT(FindLast_IntLL(list), 30);

	/*RemoveFrom drops the match and everything behind it, which is what
	backs the "-word" command in FLWP*/
	RemoveFrom_IntLL(20, list);
	CHECK_INT(FindLast_IntLL(list), 10);
	CHECK_INT(list->next->data, 5);
	CHECK_INT(list->next->next->data, 10);
	CHECK_NULL(list->next->next->next);

	Free_IntLL(list);
}

static void test_intll_copy_appends_every_node(void){
	struct intList* source = init_IntLL();
	struct intList* destination = init_IntLL();
	struct intList* walk;
	int expected[] = {7, 8, 9};
	int i = 0;

	AddToBack_IntLL(7, source);
	AddToBack_IntLL(8, source);
	AddToBack_IntLL(9, source);

	Copy_IntLLToIntLL(destination, source);
	CHECK_INT(destination->size, 3);

	walk = destination->next;
	while(walk != NULL && i < 3){
		CHECK_INT(walk->data, expected[i]);
		walk = walk->next;
		i++;
	}
	CHECK_INT(i, 3);
	CHECK_NULL(walk);

	/*The copy is a separate list: freeing one leaves the other intact*/
	Free_IntLL(source);
	CHECK_INT(destination->size, 3);

	Free_IntLL(destination);
}

void suite_structs(void){
	printf("\n-- structs --\n");
	RUN_TEST(test_wordset_marks_and_clears);
	RUN_TEST(test_wordset_rejects_out_of_range);
	RUN_TEST(test_wordset_deep_copy_is_independent);
	RUN_TEST(test_arraylist_grows_past_initial_size);
	RUN_TEST(test_arraylist_builds_a_path_string);
	RUN_TEST(test_intll_add_find_and_remove);
	RUN_TEST(test_intll_copy_appends_every_node);
}
