#include "../includes/Hints2.h"
#include "../../structs/includes/ArrayList.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../includes/FLWGGame.h"

/* A word to play from here, and the commonest one there is.
 *
 * It used to hand back a RANDOM unused neighbour, which on a four letter board
 * meant a one in eleven chance of naming the most obscure word in reach. A hint
 * is bought, usually while stuck and usually against a clock, and the worst
 * thing it can do is name something the player has never met - they cannot act
 * on it, cannot check it, and have paid for the privilege.
 *
 * Not capped, ordered. See Sort_ByObscurity: a cap on top of an ordering can
 * only ever force the answer the ordering already gave, and being wrong about
 * one word costs it a place in the queue rather than its existence. So TARE is
 * offered when TARE is the best thing here, and never while anything commoner
 * is available.
 *
 * Deterministic now rather than random, which is the right trade: this answers
 * "the best move I can show you", and the same board twice deserves the same
 * answer. A taken hint spends the word, so asking again moves on by itself.
 */
int directAdjacencyHint(int wordId, struct DataStructures* data){
	if(isTrapped(wordId, data)){
		return -1; 
	}
	struct intList* conn = getConnections(wordId, data->I2W);
	/* A game that never got a start word carries an id of -1, and the
	   accessor answers that with nothing rather than reading array[-1]. */
	if(conn == NULL){
		return -1;
	}

	int best = -1;
	while(conn->next != NULL){
		conn = conn->next; 
		int id = conn->data; 
		if(checkIfUsed_WordSet(id, data->wordSet) == 0
			&& (best == -1 || getObscurity(id, data) < getObscurity(best, data))){
			best = id;
		}
	}

	/* -1 rather than rand() % 0. isTrapped is meant to have caught this, but
	   the old line divided by the count without checking it. */
	return best;
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

	/* Commonest first, so the letter named is the one that opens the word the
	   player is likeliest to know. The letter is the same either way when the
	   neighbours agree on it; where they do not, this is the difference between
	   pointing at MIRE and pointing at DAWS. */
	int n = getNumAdjacencies(id, data);
	if(n <= 0){
		return hint;
	}
	int neighbours[n];
	int count = Neighbours_ByObscurity(id, neighbours, n, data->I2W);

	for(int at = 0; at < count; at++){
		int c_id = neighbours[at];
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
	}
	return hint;

}


/* How many ways out there are, counting every one of them.
 *
 * Deliberately NOT narrowed to the words the board would deal. This is a claim
 * about the player's position and the player may type anything in the
 * dictionary, so the true number of legal moves is the whole count. Reporting
 * five ways out to somebody who can see six is how a hint stops being believed,
 * and a number they can check themselves has to be the one they would get.
 */
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
