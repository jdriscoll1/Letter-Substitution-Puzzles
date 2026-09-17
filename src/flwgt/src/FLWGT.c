/*************************
Name: Jordan D.
Purpose: The generalized turns game - name words at a given distance from a set.

************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/FLWGT.h"
#include "../../flwp/includes/UserInput.h"
#include "../../structs/includes/IntLinkedList.h"

/* Breadth first from every seed at once.
 *
 * Multi-source rather than one search per seed and then a minimum, because
 * that is the same thing and this is one pass. Every seed starts at zero and
 * goes on the queue together, so the first time a word is reached it is reached
 * by whichever seed is nearest to it, which is the definition being asked for.
 *
 * The word set is deliberately not consulted. This measures the graph, and the
 * graph does not change when somebody names a word.
 */
int* distancesFromSeeds(const int* seeds, int numSeeds, struct DataStructures* data){
	/* nothing to work with */
	if(data == NULL || data->I2W == NULL || seeds == NULL || numSeeds <= 0){
		return NULL;
	}

	int total = data->I2W->numWords;
	int* distance = malloc(sizeof(int) * total);
	int* queue = malloc(sizeof(int) * total);
	if(distance == NULL || queue == NULL){
		free(distance);
		free(queue);
		return NULL;
	}

	int i;
	for(i = 0; i < total; i++){
		distance[i] = -1;
	}

	int head = 0, tail = 0;
	for(i = 0; i < numSeeds; i++){
		int seed = seeds[i];
		/* A seed the dictionary does not have is dropped rather than fatal: the
		   caller built this list out of a rule, and a rule that matches nothing
		   is a board with no answers, which isSolvableFLWGT will say. */
		if(seed < 0 || seed >= total || distance[seed] == 0){
			continue;
		}
		distance[seed] = 0;
		queue[tail++] = seed;
	}

	while(head < tail){
		int curr = queue[head++];
		struct intList* conn = getConnections(curr, data->I2W);
		if(conn == NULL){
			continue;
		}
		for(conn = conn->next; conn != NULL; conn = conn->next){
			int next = conn->data;
			if(next < 0 || next >= total || distance[next] != -1){
				continue;
			}
			distance[next] = distance[curr] + 1;
			queue[tail++] = next;
		}
	}

	free(queue);
	return distance;
}

/*Whether a word satisfies the band this board was built with*/
static int isAnAnswer(int id, struct GameComponentsFLWGT* gc){
	if(id < 0 || id >= gc->numWords){
		return 0;
	}
	int d = gc->distance[id];
	return d >= gc->minDistance && d <= gc->maxDistance;
}

struct GameComponentsFLWGT* initFLWGT(const int* seeds, int numSeeds, int minDistance,
	int maxDistance, int wordsWanted, struct DataStructures* data){
	/* nothing to work with */
	if(data == NULL || data->I2W == NULL){
		return NULL;
	}

	int* distance = distancesFromSeeds(seeds, numSeeds, data);
	if(distance == NULL){
		return NULL;
	}

	struct GameComponentsFLWGT* gc = malloc(sizeof(struct GameComponentsFLWGT));
	if(gc == NULL){
		free(distance);
		return NULL;
	}

	gc->distance = distance;
	gc->numWords = data->I2W->numWords;
	gc->minDistance = minDistance;
	gc->maxDistance = maxDistance;
	gc->wordsWanted = wordsWanted < 1 ? 1 : wordsWanted;
	gc->wordsFound = 0;
	gc->answersTotal = 0;
	gc->answersWorthAsking = 0;

	int i;
	for(i = 0; i < gc->numWords; i++){
		if(!isAnAnswer(i, gc)){
			continue;
		}
		gc->answersTotal++;
		/* The second count is the one that decides whether the board is fair.
		   The first is the truth about the dictionary. */
		if(!isTooObscure(i, data)){
			gc->answersWorthAsking++;
		}
	}

	return gc;
}

int isSolvableFLWGT(struct GameComponentsFLWGT* gc){
	/* nothing to work with */
	if(gc == NULL){
		return 0;
	}

	return gc->answersWorthAsking >= gc->wordsWanted;
}

int distanceOfWordFLWGT(const char* word, struct GameComponentsFLWGT* gc,
	struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || data == NULL || word == NULL){
		return -2;
	}

	int id = Convert_WordToInt((char*)word, data);
	if(id < 0 || id >= gc->numWords){
		return -2;
	}
	return gc->distance[id];
}

int userEntersWordFLWGT(const char* word, struct GameComponentsFLWGT* gc,
	struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || data == NULL || word == NULL){
		return UNKNOWN_ERROR;
	}

	int length = (int)strlen(word);
	if(length < data->I2W->numLetters){
		return TOO_SHORT;
	}
	if(length > data->I2W->numLetters){
		return TOO_LONG;
	}

	int id = Convert_WordToInt((char*)word, data);
	if(id < 0 || id >= gc->numWords){
		return WORD_DOES_NOT_EXIST;
	}

	/* Every mode keeps its answers; naming the same word twice is not two
	   answers. */
	if(checkIfUsed_WordSet(id, data->wordSet)){
		return WORD_USED;
	}

	/* And the only rule this mode actually has. Nothing about the obscurity of
	   the word is asked here - the cap says what the GAME may use, never what
	   the player may type, and a player who knows a rare word that fits has
	   answered the question. */
	if(!isAnAnswer(id, gc)){
		return WRONG_DISTANCE;
	}

	markUsed_WordSet(id, data->wordSet);
	gc->wordsFound++;
	return VALID;
}

int isGameWonFLWGT(struct GameComponentsFLWGT* gc){
	/* nothing to work with */
	if(gc == NULL){
		return 0;
	}

	return gc->wordsFound >= gc->wordsWanted;
}

int wordsFoundFLWGT(struct GameComponentsFLWGT* gc){
	return gc == NULL ? 0 : gc->wordsFound;
}

int wordsWantedFLWGT(struct GameComponentsFLWGT* gc){
	return gc == NULL ? 0 : gc->wordsWanted;
}

int answersLeftFLWGT(struct GameComponentsFLWGT* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || data == NULL){
		return 0;
	}

	int i, left = 0;
	for(i = 0; i < gc->numWords; i++){
		if(isAnAnswer(i, gc) && !checkIfUsed_WordSet(i, data->wordSet)){
			left++;
		}
	}
	return left;
}

int anAnswerFLWGT(struct GameComponentsFLWGT* gc, struct DataStructures* data){
	/* nothing to work with */
	if(gc == NULL || data == NULL){
		return -1;
	}

	/* Commonest first, and capped, because this is the engine choosing a word
	   to put in front of somebody rather than judging one they chose. Sorting
	   rather than filtering would be the rule everywhere else, but here there
	   is a floor underneath it: an answer past the cap is still better than no
	   answer, so the cap is only a preference and the last resort ignores it. */
	int i, best = -1, fallback = -1;
	for(i = 0; i < gc->numWords; i++){
		if(!isAnAnswer(i, gc) || checkIfUsed_WordSet(i, data->wordSet)){
			continue;
		}
		if(fallback == -1 || getObscurity(i, data) < getObscurity(fallback, data)){
			fallback = i;
		}
		if(isTooObscure(i, data)){
			continue;
		}
		if(best == -1 || getObscurity(i, data) < getObscurity(best, data)){
			best = i;
		}
	}

	return best != -1 ? best : fallback;
}

void freeGameComponentsFLWGT(struct GameComponentsFLWGT* gc){
	/* nothing to work with */
	if(gc == NULL){
		return;
	}

	free(gc->distance);
	free(gc);
}
