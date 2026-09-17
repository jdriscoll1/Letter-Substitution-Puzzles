#include <string.h>
#include <stdlib.h>

#include "../includes/Challenges.h"
#include "../../algs/includes/BreadthFirstSearch.h"
#include "../../algs/includes/TreeStorageNode.h"
#include "../../flwp/includes/GameFunctions.h"
#include "../../structs/includes/Queue.h"
#include "../../shared/includes/Log.h"
#include "../../shared/includes/Relax.h"

void swapAvoidGoal(struct StartWordParametersFLWC* p);

int all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(int id, int minDistance, int maxDistance, struct WordSet* goalWords, struct WordSet *avoidWords, struct DataStructures* data);

int is_game_winnable_FLWC(
	// the current id being searched	
	
	int id, 
	// this is the current depth 
	int depth, 
	// is it currently the player or bot's perspective	
	int isPlayerPerspective, 
	// the set of goal words	
	struct WordSet* goalWords, 
	// the set of avoid words
	struct WordSet* avoidWords, 
	// the words must not be too close
	struct DataStructures* data, 
	// alpha --> Initialized to -100
	int alpha, 
	// beta  --> Initialized to +100
	int beta);

/* A word to open a constraint game on.
 *
 * Six checks, and they are not all the same kind of thing. Two of them say
 * whether this is a game at all: a start that is already a goal or a forbidden
 * word is a board that is over before it begins, and a start the player cannot
 * force a win from is a board they cannot win. The other four - how well
 * connected the word is, how far it sits from the goal and avoid sets - are
 * what make the board easy or hard, and the dictionary may simply have nothing
 * that far out.
 *
 * It used to treat all six the same and answer -1 when any of them failed,
 * which is how Hold the line on calm water came to deal a board, throw it
 * away and deal another for as long as anybody watched: the four preferences
 * described a word that does not exist, and asking again produced the same
 * four preferences.
 *
 * So the preferences are asked for and then given up, a little at a time, and
 * the two real conditions are not. What comes back is the board that was asked
 * for when the dictionary has one, the closest board it does have when it does
 * not, and -1 only when no word in the dictionary can open a winnable game -
 * which is a rule with nothing behind it rather than an unlucky draw.
 */
int chooseStartWord_FLWCGeneral(struct StartWordParametersFLWC p, struct GameComponentsFLWC* flwcComponents, struct DataStructures* data){

	struct Band adjacencyAsked = { p.minAdjacencies, p.maxAdjacencies };
	struct Band goalAsked = { p.minGoalDistance, p.maxGoalDistance };
	struct Band avoidAsked = { p.minAvoidDistance, p.maxAvoidDistance };

	// The words that pass the two cheap checks, which is as far as most words get
	int* candidates = malloc(sizeof(int) * data->I2W->numWords);

	/* One round past the last, and it is the one that used to be the last.
	 *
	 * Every round including the give-up round now keeps a floor under how many
	 * ways out the board has, because a board with one move is not a looser
	 * version of the board that was asked for - it is not a choice. This extra
	 * round drops even that, and exists only so that nothing this function
	 * could answer before it can fail to answer now: if the dictionary really
	 * has no playable word with eight ways out under these rules, the old
	 * behaviour is still there underneath rather than a -1 and a mode that
	 * asks again for ever. */
	for(int round = 0; round <= RELAXATION_ROUNDS; round++){
		int takeAnythingPlayable = (round == RELAXATION_ROUNDS);
		struct Band adjacency = loosen(adjacencyAsked, round);

		if(!takeAnythingPlayable && adjacency.min < FEWEST_WAYS_OUT){
			adjacency.min = FEWEST_WAYS_OUT;
		}

		/* A distance band nobody is asking about is written 0,0 here, and the
		   check reads that as "do not look" rather than as "look everywhere".
		   So giving up on distance means saying 0,0, not saying 0 to INT_MAX -
		   which would be a demand that a goal word exist somewhere in the
		   graph, and a far stronger one than was originally made. */
		struct Band goal = isAnything(round) ? (struct Band){ 0, 0 } : loosen(goalAsked, round);
		struct Band avoid = isAnything(round) ? (struct Band){ 0, 0 } : loosen(avoidAsked, round);

		int numCandidates = 0;

		// for wordId in allWords
		for(int i = 0; i < data->I2W->numWords; i++){
			// CHECK #0: The Current Word Is Not in the goal word nor avoid word set
			// Never relaxed: a board that opens on the goal is already over.
			if(checkIfUsed_WordSet(i, p.goalWords) || checkIfUsed_WordSet(i, p.avoidWords)){
				continue;
			}

			// CHECK #1: Does the word have a # of adjacencies in Range
			int n = data->I2W->array[i]->numConnections;
			if(n < adjacency.min || n > adjacency.max){
				continue;
			}

			/* CHECK #1b: and is it a word this board is allowed to use at all.
			Refused on every round but the last-resort one, like the ways-out
			floor above it: a board opening on a word nobody has heard of is not
			a looser version of the board that was asked for. */
			if(!takeAnythingPlayable && isTooObscure(i, data)){
				continue;
			}

			/*OFF LIMITS IS NOT PART OF THE CAP AND IS NOT RELAXED WITH IT. The
			cap is dropped on the give-up round so that a board can always be
			dealt; this is not, because a board that can only be built on one of
			these is a board that does not get built.*/
			if(isOffLimits(i, data)){
				continue;
			}

			candidates[numCandidates++] = i;
		}

		// The remaining checks each cost a search of their own, so the candidates are
		// walked in random order and the first word that passes them all is taken. That
		// is the same uniform choice the full scan made, without paying for a distance
		// search and a game search on every word in the dictionary.
		Shuffle_IntArray(candidates, numCandidates);

		for(int c = 0; c < numCandidates; c++){
			int i = candidates[c];

			/* CHECK #2: Nothing the rule admits sits nearer than two moves.
			 *
			 * Never relaxed, unlike the band below it. The band is a
			 * description of the board somebody wants and the dictionary may
			 * not have one; this is the difference between a board and no
			 * board at all, so giving up on it hands back something that
			 * cannot be played rather than something that was not asked for.
			 *
			 * It is also the only one of these that asks about the nearest
			 * winning word rather than about the goal word the board is dealt
			 * against, which is the distinction the band below silently got
			 * wrong for as long as it existed. */
			if(!nearestGoalIsFarEnough(i, NEAREST_GOAL_ALLOWED, p.goalWords, p.avoidWords, data)){
				continue;
			}

			// CHECK #3: If there exists a goal word < the minimum distance, continue
			// CHECK #4: If there are no goal words < the maximum distance, continue
			if(!all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(i, goal.min, goal.max, p.goalWords, p.avoidWords, data)){
				continue;

			}

			// CHECK #5: If there exists an avoid word < the minimum distance, continue
			// CHECK #6: If there are no avoid words < the maximum distance, continue
			if(!all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(i, avoid.min, avoid.max, p.avoidWords, p.goalWords, data)){
				continue;
			}


			// CHECK #7: If the user cannot force a win, continue
			// Never relaxed either: a board the player cannot win is not a board.
			// num turns does not apply to FLWGP therefore
			if(p.numTurns != -1){
				// The search only ever scores 0 or 1, so that is the window. From
				// -100 to 100 alpha never caught up with beta and the pruning inside
				// never fired once -- every sibling was searched to the bottom
				if(!is_game_winnable_FLWC(i, p.numTurns, 1, p.goalWords, p.avoidWords, data, 0, 1)){
					continue;
				}
			}

			if(round > 0){
				FLWG_LOG("No word fit the board as asked; opened on one %d round(s) looser\n", round);
			}
			free(candidates);
			return i;
		}
	}

	/* Every preference given up and still nothing, so the rules themselves have
	   no board behind them - no word in the dictionary opens a winnable game
	   under them. Asking again would produce this same answer. */
	free(candidates);
	FLWG_LOG("There are no valid words!!!\n");
	return -1;
}








/* See Challenges.h. Breadth first from the start, and the moment anything the
 * rule admits turns up inside the floor the board is thrown back.
 *
 * Forbidden words are not walked through, matching the search below it: a route
 * that runs through a word the player must not make is not a route they have.
 */
int nearestGoalIsFarEnough(int id, int least, struct WordSet* goalWords, struct WordSet* avoidWords, struct DataStructures* data){

	/* Nothing asked, or a mode with no rule to reach - the avoid boards hand
	   in an empty goal set and mean it. */
	if(least <= 0 || goalWords == NULL){
		return 1;
	}

	struct Queue* q = init_Queue();
	struct WordSet* seen = init_WordSet(data->I2W->numWords);
	markUsed_WordSet(id, seen);
	enqueue(id, 0, NULL, q);

	int farEnough = 1;

	while(!isEmpty_Queue(q)){
		struct QueueNode* parent = dequeue(q);
		int distance = parent->data->distance;
		int currId = parent->data->id;

		/* Breadth first hands nodes back in order of distance, so once the
		   floor is reached everything still to come is at or beyond it and
		   there is nothing left to find. */
		if(distance >= least){
			break;
		}

		/* The start itself is somebody else's check - a board that opens on a
		   goal is rejected before this is ever asked. */
		if(distance > 0 && checkIfUsed_WordSet(currId, goalWords)){
			farEnough = 0;
			break;
		}

		struct intList* conn = getConnections(currId, data->I2W);
		while(conn->next != NULL){
			conn = conn->next;
			int next = conn->data;
			if(checkIfUsed_WordSet(next, seen) || checkIfUsed_WordSet(next, avoidWords)){
				continue;
			}
			enqueue(next, distance + 1, parent, q);
			markUsed_WordSet(next, seen);
		}
	}

	free_Queue(q);
	free_WordSet(seen);
	return farEnough;
}

/* Two questions, and they are deliberately not asked of the same graph.
 *
 * "Nothing the rule admits sits nearer than minDistance" is a guard - the board
 * is spoiled if something is too close. "Something the rule admits sits within
 * maxDistance" is a promise - the board is only playable if it can be reached.
 *
 * Narrowing a graph only ever makes distances longer, so the strict reading of
 * each one is a different graph:
 *
 *   The floor is measured over the WHOLE DICTIONARY, because the player may
 *   type anything in it. Measuring it among the board's own words would report
 *   a goal as three moves off when somebody who knows one odd word can be there
 *   in one, and hand back a board that is over before it starts.
 *
 *   The ceiling is measured over THE WORDS THIS BOARD DEALS, because a goal
 *   that can only be got at through words nobody has heard of is not within
 *   reach in any sense the player would recognise.
 *
 * Both are the conservative reading. The two searches together cost less than
 * the winnability search this sits in front of, and the first one usually stops
 * within a move or two.
 */
int nothingInSetIsNearerThan(int id, int least, struct WordSet* set, struct WordSet* forbidden, struct DataStructures* data){
	if(least <= 0 || set == NULL){
		return 1;
	}

	struct Queue* q = init_Queue();
	struct WordSet* seen = init_WordSet(data->I2W->numWords);
	markUsed_WordSet(id, seen);
	enqueue(id, 0, NULL, q);

	int farEnough = 1;

	while(!isEmpty_Queue(q)){
		struct QueueNode* parent = dequeue(q);
		int distance = parent->data->distance;
		int currId = parent->data->id;

		/* Breadth first hands nodes back in order of distance, so once the
		   floor is reached there is nothing nearer left to find. */
		if(distance >= least){
			break;
		}
		if(checkIfUsed_WordSet(currId, set)){
			farEnough = 0;
			break;
		}

		struct intList* conn = getConnections(currId, data->I2W);
		while(conn->next != NULL){
			conn = conn->next;
			int next = conn->data;
			if(checkIfUsed_WordSet(next, seen) || checkIfUsed_WordSet(next, forbidden)){
				continue;
			}
			enqueue(next, distance + 1, parent, q);
			markUsed_WordSet(next, seen);
		}
	}

	free_Queue(q);
	free_WordSet(seen);
	return farEnough;
}

int somethingInSetIsWithin(int id, int most, struct WordSet* set, struct WordSet* forbidden, struct DataStructures* data){
	if(set == NULL){
		return 0;
	}

	struct Queue* q = init_Queue();
	struct WordSet* seen = init_WordSet(data->I2W->numWords);
	markUsed_WordSet(id, seen);
	enqueue(id, 0, NULL, q);

	int found = 0;

	while(!isEmpty_Queue(q)){
		struct QueueNode* parent = dequeue(q);
		int distance = parent->data->distance;
		int currId = parent->data->id;

		/* THE DESTINATION AS WELL AS THE ROAD.
		 *
		 * This used to take any word the rule admits, however obscure, and only
		 * the words on the way had to be ones the board deals. That made the
		 * promise this check exists to make - "the board can be finished" -
		 * true of a board whose only finish was a word nobody says. The level
		 * would then be asking for something outside its own vocabulary while
		 * every word it dealt was inside it.
		 *
		 * It stays the strict reading: taking words out of the graph only ever
		 * makes distances longer, so a ceiling measured this way can only get
		 * harder to satisfy, never easier. And the floor below it is measured
		 * over the whole dictionary still, because that one asks what the
		 * PLAYER could stumble into and they may type anything.
		 *
		 * Said twice on purpose. The queueing rule below no longer lets an
		 * obscure goal word into the search at all, so this arrival check is
		 * already true by the time it is reached - it is here to say the rule
		 * out loud at the place a reader looks for it, and to still hold if
		 * somebody ever lets goal words back past the queue. */
		if(checkIfUsed_WordSet(currId, set) && !isTooObscure(currId, data)
			&& !isOffLimits(currId, data)){
			found = 1;
			break;
		}
		if(distance >= most){
			continue;
		}

		struct intList* conn = getConnections(currId, data->I2W);
		while(conn->next != NULL){
			conn = conn->next;
			int next = conn->data;
			if(checkIfUsed_WordSet(next, seen) || checkIfUsed_WordSet(next, forbidden)){
				continue;
			}
			/* The road AND the destination, both in the words this board
			   deals - see the note at the arrival above. A goal word that is
			   too obscure is no longer worth walking to, so it is no longer
			   worth queueing either. */
			if(isTooObscure(next, data) || isOffLimits(next, data)){
				continue;
			}
			enqueue(next, distance + 1, parent, q);
			markUsed_WordSet(next, seen);
		}
	}

	free_Queue(q);
	free_WordSet(seen);
	return found;
}

/* The board's distance rule: nothing the rule admits is nearer than the floor,
   and something it admits is within the ceiling. The two are measured on
   different graphs - see the comment above the pair of searches. */
int all_words_are_greater_than_min_distance_and_there_exists_a_word_less_than_max_distance(int id, int minDistance, int maxDistance, struct WordSet* goalWords, struct WordSet *avoidWords, struct DataStructures* data){

	// if the min distance and max distance are both 0, return false
	if(minDistance == 0 && maxDistance == 0){
		return 1;
	}

	if(!nothingInSetIsNearerThan(id, minDistance, goalWords, avoidWords, data)){
		return 0;
	}

	return somethingInSetIsWithin(id, maxDistance, goalWords, avoidWords, data);
}




// Get IS GAME WINNABLE FLWC 
// 0 --> TIE
// -1 --> NOT WINNABLE
// 1 --> WINNABLE
int max(int a, int b) {
	return (a > b) ? a : b; 
}
int min(int a, int b){
	return (a < b) ? a : b; 
}
int is_game_winnable_FLWC(
	// the current id being searched	
	
	int id, 
	// this is the current depth 
	int depth, 
	// is it currently the player or bot's perspective	
	int isPlayerPerspective, 
	// the set of goal words	
	struct WordSet* goalWords, 
	// the set of avoid words
	struct WordSet* avoidWords, 
	// the words must not be too close
	struct DataStructures* data, 
	// alpha --> Initialized to -100
	int alpha, 
	// beta  --> Initialized to +100
	int beta){


	/*SECTION #1: CHECK TO SEE IF CURRENT WORD IS A LEAF NODE*/
	int isGoalWord = goalWords && checkIfUsed_WordSet(id, goalWords); 
	int isAvoidWord = avoidWords && checkIfUsed_WordSet(id, avoidWords); 
	// WIN if goal word reached
	if(isGoalWord){
		return 1; 
	}
	// LOSS if avoid word reached - if it's not in the avoid range it doesn't count :( 
	if(isAvoidWord){
		return 0; 
	}

	// WIN if loss avoided, but LOSS if goal word missed
	if(depth <= 0) {
		// if there are goal words, we did not reach them and lost
		return !goalWords; 
	}
	
	/****SECTION 2: CHECK WORDS LEAVES TO SEE IF ANY ARE AVOID/GOAL WORDS***/
	
	markUsed_WordSet(id, data->wordSet); 

	// Get the Current Words Adjacencies
	struct intList* options = getConnections(id, data->I2W); 
	options = options->next; 

	// Start of by doing max scores. These are sentinels, not scores anybody can
	// earn -- the first option that gets looked at replaces them
	int result = (isPlayerPerspective) ?  -100 : 100;

	// Did anybody actually have a move to make from here?
	int hasMove = 0;

	while(options != NULL){
	
		int optionId = options->data; 
		// if the curr option is used, skip 
		if(checkIfUsed_WordSet(optionId, data->wordSet)){
			options = options->next; 
			continue; 
		}
		hasMove = 1;
		int option_score = is_game_winnable_FLWC(
			optionId,
			depth - 1, 
			!isPlayerPerspective, 
			goalWords, 	
			avoidWords, 	
			data, 
			alpha, 
			beta
		);

		// Fold the option into the running result: the best the player can
		// force, or the worst the opponent will allow

		if(isPlayerPerspective){
			result = max(option_score, result); 
			alpha = max(alpha, result); 
		}
		else{
			result = min(option_score, result); 
			beta = min(beta, result); 
		}
		if(alpha >= beta){
			break; 
		}
		// Alpha Beta Pruning??
		options = options->next; 
	}
	markUnused_WordSet(id, data->wordSet);

	// Whoever is on turn has nowhere to go, and the game scores that against the
	// side that is stuck: botTakesTurnFLWC reports -1 when the bot runs out of
	// moves, which it calls a loss for the bot, and -2 when the player is
	// trapped, which it calls a win for the bot. So this follows who cannot
	// move rather than the goal set -- it is not the same ending as the turns
	// running out, where nobody is stuck and the goal simply went unreached.
	// Falling out of the loop instead returned the sentinel, so a trapped player
	// scored -100, and every caller reads this as a truthy int: being trapped
	// came back as a win
	if(!hasMove){
		return !isPlayerPerspective;
	}
	return result;
	
}

		/*
		if(p.minGoalDistance != 1 || p.maxGoalDistance != 1){
			isValid_Goal = isWordValid_StartWordParameters(i, p, data); 
		}
		if(p.minAvoidDistance != 1 || p.maxAvoidDistance != 1){
			swapAvoidGoal(&p); 
			isValid_Avoid = isWordValid_StartWordParameters(i, p, data); 
			swapAvoidGoal(&p); 
		}
		*/ 
/*
	struct WordSet *goalWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strstr(w1, "oo") != NULL){
			markUsed_WordSet(i, goalWordSet); 
		}
	}
	flwcComponents->goalWords = goalWordSet; 
	p.goalWords = goalWordSet; 

	// Avoid Words
	struct WordSet *avoidWordSet = init_WordSet(data->I2W->numWords);	
	for(int i = 0; i < data->I2W->numWords; i++){
		char* w1 = Convert_IntToWord(i, data->I2W); 
		if(strchr(w1, 'd') != NULL){
			markUsed_WordSet(i, avoidWordSet); 
		}
	}
	flwcComponents->avoidWords = avoidWordSet; 
	p.avoidWords = avoidWordSet; 
	*/
	
struct WordSet* convertCharPtrPtrToWordSet(char** words, struct DataStructures* data){

	struct WordSet* wordSet = init_WordSet(data->I2W->numWords);
	int i = 0; 
	int wordId; 
	while(words[i] != NULL){
		
		wordId = convertWordToInt(words[i], data);
	 	markUsed_WordSet(wordId, wordSet); 	
		i++; 
	}
	return wordSet; 
}

int isWordValid_StartWordParameters(int i, struct StartWordParametersFLWC p, struct DataStructures* data){
	struct arrayList* pathToNearestWord = getPathToNearestWordInWordSet(i, p, data);  
	int distanceFromGoal = pathToNearestWord->currPrecision; 
	free_ArrayList(pathToNearestWord); 
	return distanceFromGoal >= p.minGoalDistance && distanceFromGoal <= p.maxGoalDistance; 

}
void swapAvoidGoal(struct StartWordParametersFLWC* p) {
    int a = p->minAvoidDistance;
    int b = p->maxAvoidDistance;
    struct WordSet* c = p->avoidWords;
    
    p->minAvoidDistance = p->minGoalDistance;
    p->maxAvoidDistance = p->maxGoalDistance;
    p->avoidWords = p->goalWords;
    
    p->minGoalDistance = a;
    p->maxGoalDistance = b;
    p->goalWords = c;
}

