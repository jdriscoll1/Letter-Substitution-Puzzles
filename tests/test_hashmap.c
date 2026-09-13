/*
Title: test_hashmap.c
Description: Tests for dictionary loading, the word/id conversions every other
module is built on, and the adjacency data the dictionary files carry.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/structs/includes/IntLinkedList.h"

static void test_dictionaries_load_every_word(void){
	struct DataStructures* two = open_dictionary("docs/2.txt", 2);
	struct DataStructures* four = open_dictionary("docs/4.txt", 4);
	int i;

	/*The count on the first line of each file*/
	CHECK_INT(two->I2W->numWords, 29);
	CHECK_INT(two->I2W->numLetters, 2);
	CHECK_INT(four->I2W->numWords, 1952);
	CHECK_INT(four->I2W->numLetters, 4);

	/*Every slot is filled in, and every word is the advertised length*/
	for(i = 0; i < four->I2W->numWords; i++){
		CHECK_NOT_NULL(four->I2W->array[i]);
		if(four->I2W->array[i] != NULL){
			CHECK_INT((int)strlen(four->I2W->array[i]->word), 4);
		}
	}

	freeDataStructures(two);
	freeDataStructures(four);
}

static void test_word_and_id_round_trip(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int id;
	int mismatches = 0;

	/*word -> id -> word has to be the identity for every entry*/
	for(id = 0; id < data->I2W->numWords; id++){
		char* word = Convert_IntToWord(id, data->I2W);
		if(word == NULL || Convert_WordToInt(word, data) != id){
			mismatches++;
		}
	}
	CHECK_INT(mismatches, 0);

	/*Spot check a word that is known to be in the four letter dictionary*/
	CHECK_STR(Convert_IntToWord(Convert_WordToInt("ware", data), data->I2W), "ware");

	freeDataStructures(data);
}

static void test_lookups_reject_bad_input(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	/*Not in the dictionary*/
	CHECK_INT(Convert_WordToInt("zzzz", data), -1);
	/*Wrong length, either way*/
	CHECK_INT(Convert_WordToInt("war", data), -1);
	CHECK_INT(Convert_WordToInt("wares", data), -1);
	/*No word at all*/
	CHECK_INT(Convert_WordToInt(NULL, data), -1);

	/*Ids outside the array give NULL rather than reading past the end*/
	CHECK_NULL(Convert_IntToWord(-1, data->I2W));
	CHECK_NULL(Convert_IntToWord(data->I2W->numWords, data->I2W));
	CHECK_NULL(Convert_IntToWord(data->I2W->numWords + 500, data->I2W));
	CHECK_NOT_NULL(Convert_IntToWord(data->I2W->numWords - 1, data->I2W));

	freeDataStructures(data);
}

static void test_adjacencies_are_single_substitutions(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int id;
	int badDistance = 0;
	int countMismatch = 0;

	for(id = 0; id < data->I2W->numWords; id++){
		struct intList* adjacency = getConnections(id, data->I2W);
		char* word = Convert_IntToWord(id, data->I2W);
		int counted = 0;

		adjacency = adjacency->next;
		while(adjacency != NULL){
			char* neighbour = Convert_IntToWord(adjacency->data, data->I2W);
			if(neighbour == NULL || letters_that_differ(word, neighbour, 4) != 1){
				badDistance++;
			}
			counted++;
			adjacency = adjacency->next;
		}

		/*numConnections is cached while the list is built, so the two must agree*/
		if(counted != data->I2W->array[id]->numConnections){
			countMismatch++;
		}
		if(counted != getNumAdjacencies(id, data)){
			countMismatch++;
		}
	}

	CHECK_INT(badDistance, 0);
	CHECK_INT(countMismatch, 0);

	freeDataStructures(data);
}

static void test_adjacencies_are_symmetric(void){
	struct DataStructures* data = open_dictionary("docs/2.txt", 2);
	int id;
	int asymmetric = 0;

	/*The graph is undirected, so b in adj(a) implies a in adj(b)*/
	for(id = 0; id < data->I2W->numWords; id++){
		struct intList* adjacency = getConnections(id, data->I2W)->next;
		while(adjacency != NULL){
			struct intList* back = getConnections(adjacency->data, data->I2W)->next;
			int found = 0;
			while(back != NULL){
				if(back->data == id){
					found = 1;
					break;
				}
				back = back->next;
			}
			if(!found){
				asymmetric++;
			}
			adjacency = adjacency->next;
		}
	}
	CHECK_INT(asymmetric, 0);

	freeDataStructures(data);
}

static void test_num_options_tracks_used_words(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int id = Convert_WordToInt("ware", data);
	int total = getNumAdjacencies(id, data);
	struct intList* adjacency;

	CHECK(total > 0);
	/*Nothing is used yet, so every adjacency is still an option*/
	CHECK_INT(getNumOptions(id, data), total);

	/*Using one neighbour removes exactly one option*/
	adjacency = getConnections(id, data->I2W)->next;
	markUsed_WordSet(adjacency->data, data->wordSet);
	CHECK_INT(getNumOptions(id, data), total - 1);

	reset_WordSet(data->wordSet);
	CHECK_INT(getNumOptions(id, data), total);

	freeDataStructures(data);
}

void suite_hashmap(void){
	printf("\n-- dictionary and conversions --\n");
	RUN_TEST(test_dictionaries_load_every_word);
	RUN_TEST(test_word_and_id_round_trip);
	RUN_TEST(test_lookups_reject_bad_input);
	RUN_TEST(test_adjacencies_are_single_substitutions);
	RUN_TEST(test_adjacencies_are_symmetric);
	RUN_TEST(test_num_options_tracks_used_words);
}
