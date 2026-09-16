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

	/*The count on the first line of each file - asked of the file, so this says
	"the loader read every word there was" rather than "the word list is still
	the size it was in 2021"*/
	CHECK_INT(two->I2W->numWords, declared_word_count("docs/2.txt"));
	CHECK_INT(two->I2W->numLetters, 2);
	CHECK_INT(four->I2W->numWords, declared_word_count("docs/4.txt"));
	CHECK_INT(four->I2W->numLetters, 4);
	CHECK(four->I2W->numWords > 1000);

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

static void test_flat_connections_match_the_list(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int id;
	int mismatches = 0;
	int missingArrays = 0;

	/*wordData carries the adjacencies twice: as the linked list everything walks
	and as a flat array for the searches that index into them at random. The two
	have to agree, entry for entry, or a search reads a word that is not adjacent*/
	for(id = 0; id < data->I2W->numWords; id++){
		struct wordData* word = data->I2W->array[id];
		struct intList* walk = word->connectionHeader->next;
		int i = 0;

		if(word->numConnections > 0 && word->connections == NULL){
			missingArrays++;
			continue;
		}
		while(walk != NULL && i < word->numConnections){
			if(word->connections[i] != walk->data){
				mismatches++;
			}
			walk = walk->next;
			i++;
		}
		/*Both have to run out at the same moment*/
		if(walk != NULL || i != word->numConnections){
			mismatches++;
		}
	}

	CHECK_INT(missingArrays, 0);
	CHECK_INT(mismatches, 0);

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

/*How obscure each word is, read from the ranks file that sits beside the word
 * list.
 *
 * The dictionary is the Scrabble word list now, which is twice the size it was
 * and full of words nobody says - ADZE, ZAX, QAT are all legal and all fair for
 * a player to type. What they must not be is what the game deals, or what a bot
 * answers with. The rank is how anything choosing a word tells those apart.
 *
 * The ranks live in their own file rather than on the end of a connections line
 * because every reader of those lines takes everything after the word as a
 * neighbour index, so a rank put there is silently read as a neighbour.
 */
static void test_obscurity_is_read_from_the_ranks_file(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int care, adze;

	care = Convert_WordToInt("care", data);
	adze = Convert_WordToInt("adze", data);
	CHECK(care != -1);
	CHECK(adze != -1);

	/*Nothing has been read yet, so nothing is ranked. A dictionary without a
	ranks file beside it still loads and still plays.*/
	CHECK_INT(getObscurity(care, data), OBSCURITY_UNKNOWN);

	Load_Obscurity(data->I2W, "docs/4ranks.txt");

	/*CARE is an ordinary english word and ADZE is not. The exact number is the
	word's place in a list of the commonest english words, so smaller is more
	common; what matters here is the gap between them.*/
	CHECK(getObscurity(care, data) < 5000);
	CHECK_INT(getObscurity(adze, data), OBSCURITY_UNKNOWN);
	CHECK(getObscurity(care, data) < getObscurity(adze, data));

	/*An id nothing knows about is not a lookup off the end of the array*/
	CHECK_INT(getObscurity(-1, data), OBSCURITY_UNKNOWN);
	CHECK_INT(getObscurity(data->I2W->numWords, data), OBSCURITY_UNKNOWN);

	freeDataStructures(data);
}

/*A ranks file that has drifted out of step with its word list is refused whole.
 * Applying it would give every word somebody else's rank, and a game that deals
 * by rank would then deal by nonsense - worse than having no ranks at all.*/
static void test_a_ranks_file_for_another_dictionary_is_refused(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	int care = Convert_WordToInt("care", data);

	Load_Obscurity(data->I2W, "docs/3ranks.txt");
	CHECK_INT(getObscurity(care, data), OBSCURITY_UNKNOWN);

	/*And a file that is not there at all leaves the dictionary usable*/
	Load_Obscurity(data->I2W, "docs/nosuchranks.txt");
	CHECK_INT(getObscurity(care, data), OBSCURITY_UNKNOWN);

	/*The right one still applies afterwards*/
	Load_Obscurity(data->I2W, "docs/4ranks.txt");
	CHECK(getObscurity(care, data) < 5000);

	freeDataStructures(data);
}

void suite_hashmap(void){	printf("\n-- dictionary and conversions --\n");
	RUN_TEST(test_dictionaries_load_every_word);
	RUN_TEST(test_word_and_id_round_trip);
	RUN_TEST(test_lookups_reject_bad_input);
	RUN_TEST(test_adjacencies_are_single_substitutions);
	RUN_TEST(test_adjacencies_are_symmetric);
	RUN_TEST(test_flat_connections_match_the_list);
	RUN_TEST(test_num_options_tracks_used_words);
	RUN_TEST(test_obscurity_is_read_from_the_ranks_file);
	RUN_TEST(test_a_ranks_file_for_another_dictionary_is_refused);
}
