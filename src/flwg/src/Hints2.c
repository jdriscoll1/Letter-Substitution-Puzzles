#include "../includes/Hints2.h"
#include "../../structs/includes/ArrayList.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../includes/FLWGGame.h"

int directAdjacencyHint(int wordId, struct DataStructures* data){
	// 1) Convert the integer to a word 
	if(isTrapped(wordId, data)){
		return -1; 
	}
	// 2) Look at all of the valid words
	struct intList* conn = getConnections(wordId, data->I2W);
	/* A game that never got a start word carries an id of -1, and the
	   accessor answers that with nothing rather than reading array[-1]. */
	if(conn == NULL){
		return -1;
	}
	int n = getNumAdjacencies(wordId, data);
	struct arrayList* alist = init_ArrayList(n, 5, NUM); 
	while(conn->next != NULL){
		conn = conn->next; 
		int id = conn->data; 
		if(checkIfUsed_WordSet(id, data->wordSet) == 0){
			add_ArrayList((void*)&id, alist, NUM); 
		}
	}
	int chosenArrayListId = rand() % alist->currPrecision; 	
	int result = ((int*)alist->list)[chosenArrayListId]; 
	free_ArrayList(alist); 
	return result; 

}


// Whether a word already carries a letter.
static int wordHasLetter(const char* word, int numLetters, char letter){
	for(int i = 0; i < numLetters; i++){
		if(word[i] == letter){
			return 1;
		}
	}
	return 0;
}

/* A letter worth trying: the one that turns this word into some word the
 * player has not used yet.
 *
 * Two things this has to get right that taking the first unused neighbour did
 * not.
 *
 * It prefers a letter the word does not already carry. Any unused neighbour
 * names a letter that works, but being told to try a T while a T sits in front
 * of you is not a hint - and for about one word in twenty of the three-letter
 * dictionary the first neighbour was exactly that. Where every neighbour
 * reuses a letter the word already has it still answers with one: a letter
 * already on the board beats no hint at all, and the hint has been paid for.
 *
 * And it answers when there is nothing to say. The id stayed -1 when every
 * neighbour had been used, and Convert_IntToWord hands back NULL for a
 * negative id - so the comparison read through a null pointer. That is
 * reachable: nine words in the three-letter dictionary have no neighbours at
 * all, and in FLWG the bot can leave the player on a word whose neighbours are
 * every one of them spent. '?' is this function's own "nothing to offer", and
 * the callers already expect it.
 */
char letterToConsiderHint(int id, struct DataStructures* data){
	char hint = '?';
	char* currWord = Convert_IntToWord(id, data->I2W);
	int numLetters = data->I2W->numLetters;
	if(currWord == NULL){
		return hint;
	}

	struct intList* c = getConnections(id, data->I2W);
	if(c == NULL){
		return hint;
	}
	c = c->next;
	// Loop through the num adjacenceis
	while(c != NULL){
		int c_id = c->data;
		if(checkIfUsed_WordSet(c_id, data->wordSet) == 0){
			char* nextWord = Convert_IntToWord(c_id, data->I2W);
			if(nextWord != NULL){
				for(int i = 0; i < numLetters; i++){
					if(nextWord[i] != currWord[i]){
						// A neighbour differs in exactly one place, so this
						// first difference is the substitution itself.
						if(hint == '?'){
							// Held in case nothing better turns up. A word is
							// letters, so '?' cannot be one of them.
							hint = nextWord[i];
						}
						if(!wordHasLetter(currWord, numLetters, nextWord[i])){
							return nextWord[i];
						}
						break;
					}
				}
			}
		}
		c = c->next;

	}
	return hint;

}


int numOptionsHint(int id, struct DataStructures* data){
	// Take the start word ID
	int numOptions = 0;
	struct intList* c = getConnections(id, data->I2W);
	if(c == NULL){
		return 0;
	}
	c = c->next;
	// Loop through the num adjacenceis
	while(c != NULL){
		int c_id = c->data; 
		if(checkIfUsed_WordSet(c_id, data->wordSet) == 0){
			// if the word is not used, then you can set it to the hint word id and Break 	
			numOptions++;
		}
		c = c->next; 
	}
	return numOptions; 
}
