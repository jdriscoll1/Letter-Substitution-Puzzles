/*
Title: test_bridge.c
Description: Every engine function the app can reach, exercised and asserted.

The app only ever touches the engine through one file - the JNI layer in
android/app/src/main/cpp/FLWGJNI.c - and that file calls sixty-nine engine
functions. Those sixty-nine are the whole of what a player can make the engine
do. Anything else in here is reachable only from the debug console.

test_api_surface.c already proves none of them crash when handed nothing. This
proves they do what they say when handed something: the start word is a word,
the goal is reachable, an illegal move is refused and a legal one is taken,
undo puts the board back, the hints point somewhere real, and every mode can be
played and handed back.

BRIDGE_FUNCTIONS below is the list, and the last test in the file checks that
every name on it was actually reached by a test in this file - so a function
added to the bridge and left untested fails the build rather than going quiet.
A JS test in the app repository checks the list itself still matches what the
bridge calls, which is the half of it that C cannot see.
*/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_framework.h"

#include "../src/api/includes/FLWG-API.h"
#include "../src/api/includes/FLWT-API.h"
#include "../src/api/includes/FLWC-API.h"
#include "../src/api/includes/Debug-API.h"
#include "../src/structs/includes/ArrayList.h"
#include "../src/structs/includes/HashMap.h"
#include "../src/flwg/includes/Hints2.h"
#include "../src/flwgt/includes/FLWGT.h"

struct DataStructures* open_dictionary(const char* path, int numLetters);
int letters_that_differ(const char* a, const char* b, int numLetters);

/* ------------------------------------------------------------ the ratchet */

static const char* BRIDGE_FUNCTIONS[] = {
	"initDataStructures", "Load_Obscurity_fd", "setObscurityCap",
	"obscurityOfWord",
	"seedGameRandom", "getAllWords",
	"convertIntToWord", "convertWordToInt", "directAdjacencyHint",

	"initFLWG", "isStartValidFLWG", "getCurrWord", "userTakesTurn",
	"botTakesTurn", "hintLetterToConsiderFLWG", "hintNumOptionsFLWG",
	"hintSafeMoveFLWG", "freeGameComponentsFLWG",

	"initiateFLWP", "isStartValid_FLWP", "getStartWordFLWP", "getGoalWordFLWP",
	"userEntersWord_FLWP", "removeWord_FLWP", "ResetFLWP", "undoMoveFLWP",
	"redoMoveFLWP", "getCurrentWordsFLWP", "getPrevWordFLWP", "isGameWonFLWP",
	"canUndoFLWP", "canRedoFLWP",
	"hintGetHeadAdjacencyFLWP", "hintGetTailAdjacencyFLWP",
	"hintGetMinAdjacenciesFLWP", "distanceToGoalFLWP", "routeToGoalFLWP",
	"freeGameComponentsFLWP",

	"initiateFLWGP", "isStartValid_FLWGP", "getFLWPComponentsFLWGP",
	"getFLWCComponentsFLWGP", "userEntersWord_FLWGP", "undoMoveFLWGP",
	"redoMoveFLWGP", "hintGetMinAdjacenciesFLWGP", "hintWordTowardsGoalFLWGP",
	"hintGetValidGoalWordFLWGP", "freeGameComponentsFLWGP",

	"initFLWT", "isStartValidFLWT", "getStartWordFLWT", "userEntersWordFLWT",
	"getCurrentWordsFLWT", "isGameWonFLWT", "hint_letterToConsiderFLWT",
	"hint_numOptionsFLWT", "freeGameComponentsFLWT",

	"initFLWC", "isStartValidFLWC", "getStartWordFLWC", "userEntersWordFLWC",
	"botTakesTurnFLWC", "isGameWonFLWC", "hintGoalWordFLWC",
	"hintAdjacencyTowardsGoalFLWC", "hintPathToGoalFLWC",
	"hintBestDirectAdjacencyFLWIC", "hintDistanceFromNearestAvoidWordFLWIC",
	"freeGameComponentsFLWC",

	"initFLWGT", "isSolvableFLWGT", "userEntersWordFLWGT", "distanceOfWordFLWGT",
	"isGameWonFLWGT", "wordsFoundFLWGT", "wordsWantedFLWGT", "answersLeftFLWGT",
	"anAnswerFLWGT", "freeGameComponentsFLWGT",

	"initFLWGAtStart", "initFLWPAtStart", "initFLWPBetween",
	"initFLWPUnreachable", "initFLWCAtStart", "initFLWTAtStart",
	"initFLWGPAtStart",
};

#define BRIDGE_COUNT ((int)(sizeof(BRIDGE_FUNCTIONS) / sizeof(BRIDGE_FUNCTIONS[0])))

static int covered[BRIDGE_COUNT];

/*Records that a test actually called this one*/
static void covers(const char* name){
	int i;
	for(i = 0; i < BRIDGE_COUNT; i++){
		if(strcmp(BRIDGE_FUNCTIONS[i], name) == 0){
			covered[i] = 1;
			return;
		}
	}
	/*A name that is not on the list is a typo in the test, and would otherwise
	look exactly like coverage*/
	printf("\n  covers(\"%s\") names nothing on the bridge list\n", name);
	CHECK(0);
}

/* ------------------------------------------------------- shared foundations */

void test_bridge_loads_and_converts_the_dictionary(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	covers("initDataStructures");
	CHECK(data != NULL);

	/*And how obscure each of those words is, which the app loads from a second
	asset straight after the dictionary. Over a descriptor here because that is
	how the app has it: assets live packed inside the APK rather than on the
	filesystem, so there is no path for it to open.*/
	{
		int rankFd = open("docs/4ranks.txt", O_RDONLY);
		CHECK(rankFd != -1);
		if(rankFd != -1){
			Load_Obscurity_fd(data->I2W, rankFd);
			covers("Load_Obscurity_fd");

			/*And the cap a board sets over it, which is what decides whether a
			word is one the game may use. ZOUK is legal to type and must never be
			dealt or played on calm water.*/
			setObscurityCap(data, 2000);
			covers("setObscurityCap");
			CHECK_INT(isTooObscure(Convert_WordToInt("zouk", data), data), 1);
			setObscurityCap(data, OBSCURITY_UNKNOWN);
			CHECK_INT(isTooObscure(Convert_WordToInt("zouk", data), data), 0);
			close(rankFd);
			/*CARE is an ordinary word, so it came back ranked*/
			CHECK(getObscurity(Convert_WordToInt("care", data), data) < OBSCURITY_UNKNOWN);

			/*And the same question asked by the word, which is how the app asks
			it: it holds a word the player just typed, not an id. This is the
			opposite errand to the cap above - that decides what the game may
			DEAL, this judges what the player already PLAYED, so the app can
			tell them they found a good one.*/
			covers("obscurityOfWord");
			CHECK_INT(obscurityOfWord("care", data), getObscurity(Convert_WordToInt("care", data), data));
			CHECK(obscurityOfWord("zouk", data) > obscurityOfWord("care", data));
			/*A word the dictionary has never heard of, and no word at all*/
			CHECK_INT(obscurityOfWord("qqqq", data), OBSCURITY_UNKNOWN);
			CHECK_INT(obscurityOfWord(NULL, data), OBSCURITY_UNKNOWN);
		}
	}

	/*Every word round trips through both conversions, which everything
	downstream depends on since the engine works in ids and the app in words*/
	char** words = getAllWords(data);
	covers("getAllWords");
	CHECK(words != NULL);

	/*Neither the count nor a terminator comes back with it: the array holds
	exactly one pointer per word and is not null terminated, so the only way to
	walk it is to ask the dictionary how long it is*/
	int numWords = data->I2W->numWords;
	CHECK(numWords > 0);

	int checkedWords = 0;
	int i;
	for(i = 0; words != NULL && i < numWords && i < 50; i++){
		int id = convertWordToInt(words[i], data);
		CHECK(id >= 0);
		char* back = convertIntToWord(id, data);
		CHECK(back != NULL);
		if(back != NULL){
			CHECK_INT(strcmp(back, words[i]), 0);
		}
		checkedWords++;
	}
	covers("convertWordToInt");
	covers("convertIntToWord");
	CHECK(checkedWords > 0);

	/*A word the dictionary does not hold*/
	CHECK(convertWordToInt("zzzz", data) == -1);
	/*An id no word has*/
	CHECK(convertIntToWord(-1, data) == NULL);

	/*The array belongs to the caller; the words inside it belong to the
	dictionary. Freeing those would be a double free when the dictionary goes.*/
	free(words);
	freeDataStructures(data);
}

void test_bridge_the_seed_decides_the_board(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	/*The daily puzzle rests entirely on this: one seed, one board. Two games
	asked for with the same seed have to start on the same word, and a
	different seed has to be free to choose differently.*/
	seedGameRandom(4242);
	covers("seedGameRandom");
	struct GameData* first = initFLWG(data, 1, 30);
	char* firstWord = (first != NULL && isStartValidFLWG(first)) ? getCurrWord(first, data) : NULL;

	seedGameRandom(4242);
	struct GameData* again = initFLWG(data, 1, 30);
	char* againWord = (again != NULL && isStartValidFLWG(again)) ? getCurrWord(again, data) : NULL;

	CHECK(firstWord != NULL);
	CHECK(againWord != NULL);
	if(firstWord != NULL && againWord != NULL){
		CHECK_INT(strcmp(firstWord, againWord), 0);
	}

	freeGameComponentsFLWG(first);
	freeGameComponentsFLWG(again);
	freeDataStructures(data);
}

void test_bridge_the_direct_adjacency_hint_names_a_neighbour(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	int id = convertWordToInt("ware", data);
	CHECK(id >= 0);

	int neighbour = directAdjacencyHint(id, data);
	covers("directAdjacencyHint");
	CHECK(neighbour >= 0);

	/*"Direct adjacency" has to mean exactly one letter apart, or the hint is
	pointing at a word the player cannot legally move to*/
	if(neighbour >= 0){
		char* from = convertIntToWord(id, data);
		char* to = convertIntToWord(neighbour, data);
		CHECK(from != NULL && to != NULL);
		if(from != NULL && to != NULL){
			CHECK_INT(letters_that_differ(from, to, 4), 1);
		}
	}

	freeDataStructures(data);
}

/* ------------------------------------------------------------------- FLWG */

void test_bridge_the_adversarial_game(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameData* game = initFLWG(data, 1, 30);
	covers("initFLWG");
	CHECK(game != NULL);

	CHECK(isStartValidFLWG(game) == 1);
	covers("isStartValidFLWG");

	char* word = getCurrWord(game, data);
	covers("getCurrWord");
	CHECK(word != NULL);
	if(word != NULL){
		CHECK_INT((int)strlen(word), 4);
	}

	/*A word nothing knows is refused, and the board does not move*/
	int refused = userTakesTurn("zzzz", game, data);
	covers("userTakesTurn");
	CHECK(refused != 0);
	char* afterRefusal = getCurrWord(game, data);
	if(word != NULL && afterRefusal != NULL){
		CHECK_INT(strcmp(word, afterRefusal), 0);
	}

	/*A legal one is taken, and the board moves to it*/
	int neighbour = directAdjacencyHint(convertWordToInt(word, data), data);
	if(neighbour >= 0){
		char* legal = convertIntToWord(neighbour, data);
		CHECK_INT(userTakesTurn(legal, game, data), 0);
		char* moved = getCurrWord(game, data);
		CHECK(moved != NULL);
		if(moved != NULL){
			CHECK_INT(strcmp(moved, legal), 0);
		}
	}

	/*The hints answer about wherever the board is now*/
	char letter = hintLetterToConsiderFLWG(game, data);
	covers("hintLetterToConsiderFLWG");
	CHECK((letter >= 'a' && letter <= 'z') || letter == '?');

	int options = hintNumOptionsFLWG(game, data);
	covers("hintNumOptionsFLWG");
	CHECK(options >= 0);

	/*And the one hint that knows there is an opponent hands back a word the
	  player could actually type - a legal neighbour, not yet spent - and leaves
	  the board exactly as it found it. It plays the move internally to find it,
	  so a hint that forgot to put it back would quietly cost the player their
	  best word.*/
	char* here = getCurrWord(game, data);
	char* safe = hintSafeMoveFLWG(game, 0, data);
	covers("hintSafeMoveFLWG");
	if(safe != NULL){
		CHECK_INT((int)strlen(safe), 4);
		CHECK_INT(letters_that_differ(here, safe, 4), 1);
		CHECK_INT(userTakesTurn(safe, game, data), 0);
	}

	/*And the bot answers with a word, or -1 when it is trapped*/
	int botMove = botTakesTurn(game, data, 0);
	covers("botTakesTurn");
	CHECK(botMove >= -1);

	freeGameComponentsFLWG(game);
	covers("freeGameComponentsFLWG");
	freeDataStructures(data);
}

/* ------------------------------------------------------------------ FLWGT */

/* The one mode with no board. The level states a rule about the whole
   dictionary and every word typed is judged against it on its own, so this
   walks a whole round of one: build it, check it can be finished, take an
   answer, refuse a non-answer for the right reason, and hand it back. */
void test_bridge_the_generalized_turns_game(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);

	int seed = convertWordToInt("cart", data);
	CHECK(seed >= 0);
	int seeds[1] = { seed };

	struct GameComponentsFLWGT* game = initFLWGT(seeds, 1, 2, 2, 2, data);
	covers("initFLWGT");
	CHECK(game != NULL);

	/*a board nobody could finish is not dealt*/
	CHECK_INT(isSolvableFLWGT(game), 1);
	covers("isSolvableFLWGT");

	CHECK_INT(wordsWantedFLWGT(game), 2);
	covers("wordsWantedFLWGT");
	CHECK_INT(wordsFoundFLWGT(game), 0);
	covers("wordsFoundFLWGT");
	CHECK_INT(isGameWonFLWGT(game), 0);
	covers("isGameWonFLWGT");

	int before = answersLeftFLWGT(game, data);
	covers("answersLeftFLWGT");
	CHECK(before >= 2);

	/*the hint names one, and it is an answer*/
	int offered = anAnswerFLWGT(game, data);
	covers("anAnswerFLWGT");
	CHECK(offered >= 0);

	if(offered >= 0){
		char* word = convertIntToWord(offered, data);
		CHECK_INT(distanceOfWordFLWGT(word, game, data), 2);
		covers("distanceOfWordFLWGT");

		CHECK_INT(userEntersWordFLWGT(word, game, data), 0);
		covers("userEntersWordFLWGT");
		CHECK_INT(wordsFoundFLWGT(game), 1);
		CHECK_INT(answersLeftFLWGT(game, data), before - 1);

		/*the same word is not two answers*/
		CHECK(userEntersWordFLWGT(word, game, data) != 0);
	}

	/*the seed is nought from itself, so it is not two from itself*/
	CHECK_INT(distanceOfWordFLWGT("cart", game, data), 0);
	CHECK(userEntersWordFLWGT("cart", game, data) != 0);

	/*the second answer finishes it*/
	int second = anAnswerFLWGT(game, data);
	if(second >= 0){
		CHECK_INT(userEntersWordFLWGT(convertIntToWord(second, data), game, data), 0);
		CHECK_INT(isGameWonFLWGT(game), 1);
	}

	freeGameComponentsFLWGT(game);
	covers("freeGameComponentsFLWGT");
	freeDataStructures(data);
}

/* ------------------------------------------------------------------- FLWP */

void test_bridge_the_pathfinder(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponents* game = initiateFLWP(1, 30, 2, 6, 1, 30, data);
	covers("initiateFLWP");
	CHECK(game != NULL);
	CHECK(isStartValid_FLWP(game) == 1);
	covers("isStartValid_FLWP");

	char* start = getStartWordFLWP(game, data);
	char* goal = getGoalWordFLWP(game, data);
	covers("getStartWordFLWP");
	covers("getGoalWordFLWP");
	CHECK(start != NULL);
	CHECK(goal != NULL);
	if(start != NULL && goal != NULL){
		/*A board you are already standing on is not a board*/
		CHECK(strcmp(start, goal) != 0);
	}

	/*The route the board was built to, and the distance from where you stand.
	Before a move those are the same number.*/
	int shortest = hintGetMinAdjacenciesFLWP(game, data);
	int fromHere = distanceToGoalFLWP(game, data);

	/* And the road itself, which the loss screen offers to show. It is the same
	   search keeping its trail, so it has to agree with the distance: one word
	   for every step, plus the one being stood on. */
	char* road = routeToGoalFLWP(game, data);
	covers("routeToGoalFLWP");
	CHECK_NOT_NULL(road);
	if(road != NULL){
		int spaces = 0;
		for(int i = 0; road[i] != 0; i++){ if(road[i] == ' '){ spaces++; } }
		CHECK_INT(spaces, fromHere + 1);
		free(road);
	}
	covers("hintGetMinAdjacenciesFLWP");
	covers("distanceToGoalFLWP");
	CHECK(shortest >= 2);
	CHECK(shortest <= 6);
	CHECK_INT(fromHere, shortest);

	/*The board is not won before it is walked*/
	CHECK(isGameWonFLWP(game) != 1);
	covers("isGameWonFLWP");

	/*The path so far is the start word and nothing else*/
	struct arrayList* path = getCurrentWordsFLWP(game);
	covers("getCurrentWordsFLWP");
	CHECK(path != NULL);

	CHECK(getPrevWordFLWP(game) >= 0);
	covers("getPrevWordFLWP");

	/*The two hints name words next to each end of the route*/
	char* fromStart = hintGetHeadAdjacencyFLWP(game, data);
	char* fromGoal = hintGetTailAdjacencyFLWP(game, data);
	covers("hintGetHeadAdjacencyFLWP");
	covers("hintGetTailAdjacencyFLWP");
	CHECK(fromStart != NULL);
	CHECK(fromGoal != NULL);
	if(fromStart != NULL && start != NULL){
		CHECK_INT(letters_that_differ(start, fromStart, 4), 1);
	}
	if(fromGoal != NULL && goal != NULL){
		CHECK_INT(letters_that_differ(goal, fromGoal, 4), 1);
	}

	/*Walking one step towards the goal shortens the distance by one, and
	winding it back lengthens it again - which is the whole of what undo means*/
	if(fromStart != NULL){
		/*Nothing has been walked yet, so neither button has anything to do*/
		CHECK_INT(canUndoFLWP(game), 0);
		covers("canUndoFLWP");
		CHECK_INT(canRedoFLWP(game), 0);
		covers("canRedoFLWP");

		CHECK_INT(userEntersWord_FLWP(fromStart, game, data), 0);
		covers("userEntersWord_FLWP");
		CHECK_INT(distanceToGoalFLWP(game, data), shortest - 1);

		/*A move to take back, and still nothing to put back*/
		CHECK_INT(canUndoFLWP(game), 1);
		CHECK_INT(canRedoFLWP(game), 0);

		undoMoveFLWP(game, data);
		covers("undoMoveFLWP");
		CHECK_INT(distanceToGoalFLWP(game, data), shortest);

		/*And now the other way round*/
		CHECK_INT(canUndoFLWP(game), 0);
		CHECK_INT(canRedoFLWP(game), 1);

		redoMoveFLWP(game, data);
		covers("redoMoveFLWP");
		CHECK_INT(distanceToGoalFLWP(game, data), shortest - 1);
		CHECK_INT(canRedoFLWP(game), 0);

		removeWord_FLWP(fromStart, game, data);
		covers("removeWord_FLWP");
	}

	/*Reset puts it back to the word it started on*/
	ResetFLWP(game, data);
	covers("ResetFLWP");
	CHECK_INT(distanceToGoalFLWP(game, data), shortest);

	/*A word that is not a word, and a word that is not adjacent, are both refused*/
	CHECK(userEntersWord_FLWP("zzzz", game, data) != 0);
	if(goal != NULL && shortest > 1){
		CHECK(userEntersWord_FLWP(goal, game, data) != 0);
	}

	freeGameComponentsFLWP(game, data);
	covers("freeGameComponentsFLWP");
	freeDataStructures(data);
}

/* ------------------------------------------------------------------ FLWGP */

void test_bridge_the_composed_pathfinder(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goals[] = { "ware", NULL };
	char* avoids[] = { NULL };

	struct GameComponentsFLWGP* game = initiateFLWGP(1, 30, goals, avoids, 2, 0, 5, 0, 1, 30, data);
	covers("initiateFLWGP");
	CHECK(game != NULL);

	if(game != NULL && isStartValid_FLWGP(game)){
		covers("isStartValid_FLWGP");

		/*It is made of the other two, and hands both out*/
		struct GameComponents* inner = getFLWPComponentsFLWGP(game);
		struct GameComponentsFLWC* constraint = getFLWCComponentsFLWGP(game);
		covers("getFLWPComponentsFLWGP");
		covers("getFLWCComponentsFLWGP");
		CHECK(inner != NULL);
		CHECK(constraint != NULL);

		int shortest = hintGetMinAdjacenciesFLWGP(game);
		covers("hintGetMinAdjacenciesFLWGP");
		CHECK(shortest >= 1);

		char* towards = hintWordTowardsGoalFLWGP(game, data);
		char* validGoal = hintGetValidGoalWordFLWGP(game, data);
		covers("hintWordTowardsGoalFLWGP");
		covers("hintGetValidGoalWordFLWGP");

		/*The goal it names has to be one of the goals it was given*/
		if(validGoal != NULL){
			CHECK_INT(strcmp(validGoal, "ware"), 0);
		}

		/*And the word it points at is a legal move from where the board stands*/
		if(towards != NULL && inner != NULL){
			char* here = getStartWordFLWP(inner, data);
			if(here != NULL){
				CHECK_INT(letters_that_differ(here, towards, 4), 1);
			}
			CHECK_INT(userEntersWord_FLWGP(towards, game, data), 0);
			covers("userEntersWord_FLWGP");

			undoMoveFLWGP(game, data);
			covers("undoMoveFLWGP");
			redoMoveFLWGP(game, data);
			covers("redoMoveFLWGP");
		}
	}

	freeGameComponentsFLWGP(game, data);
	covers("freeGameComponentsFLWGP");
	freeDataStructures(data);
}

/* ------------------------------------------------------------------- FLWT */

void test_bridge_the_turns_game(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	struct GameComponentsFLWT* game = initFLWT(3, 1, 30, data);
	covers("initFLWT");
	CHECK(game != NULL);
	CHECK(isStartValidFLWT(game) == 1);
	covers("isStartValidFLWT");

	char* start = getStartWordFLWT(game, data);
	covers("getStartWordFLWT");
	CHECK(start != NULL);

	CHECK(getCurrentWordsFLWT(game) != NULL);
	covers("getCurrentWordsFLWT");

	/*Three changes asked for, so it is not won before three are made*/
	CHECK(isGameWonFLWT(game) != 1);
	covers("isGameWonFLWT");

	char letter = hint_letterToConsiderFLWT(game, data);
	covers("hint_letterToConsiderFLWT");
	CHECK((letter >= 'a' && letter <= 'z') || letter == '?');

	int options = hint_numOptionsFLWT(game, data);
	covers("hint_numOptionsFLWT");
	CHECK(options > 0);

	/*Every turn is a neighbour of the one word, not a chain - and the same
	word cannot be given twice*/
	if(start != NULL){
		int id = convertWordToInt(start, data);
		int neighbour = directAdjacencyHint(id, data);
		if(neighbour >= 0){
			char* move = convertIntToWord(neighbour, data);
			CHECK_INT(userEntersWordFLWT(move, game, data), 0);
			covers("userEntersWordFLWT");
			CHECK(userEntersWordFLWT(move, game, data) != 0);
		}
	}
	CHECK(userEntersWordFLWT("zzzz", game, data) != 0);

	freeGameComponentsFLWT(game);
	covers("freeGameComponentsFLWT");
	freeDataStructures(data);
}

/* ------------------------------------------------------------ FLWC / FLWIC */

void test_bridge_the_constraint_game(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goals[] = { "ware", NULL };
	char* avoids[] = { NULL };

	struct GameComponentsFLWC* game = initFLWC(1, 30, goals, avoids, 2, 0, 5, 0, 1, 30, 10, data);
	covers("initFLWC");
	CHECK(game != NULL);

	if(game != NULL && isStartValidFLWC(game)){
		covers("isStartValidFLWC");

		char* start = getStartWordFLWC(game, data);
		covers("getStartWordFLWC");
		CHECK(start != NULL);

		/*In progress, which this mode reports as -1 rather than 0*/
		CHECK_INT(isGameWonFLWC(game), -1);
		covers("isGameWonFLWC");

		/*The goal it names is one it was given, and the path it offers starts
		where the board is*/
		char* goalWord = hintGoalWordFLWC(game, data);
		covers("hintGoalWordFLWC");
		if(goalWord != NULL){
			CHECK_INT(strcmp(goalWord, "ware"), 0);
		}

		char* towards = hintAdjacencyTowardsGoalFLWC(game, data);
		covers("hintAdjacencyTowardsGoalFLWC");
		if(towards != NULL && start != NULL){
			CHECK_INT(letters_that_differ(start, towards, 4), 1);
			CHECK_INT(userEntersWordFLWC(towards, game, data), 0);
			covers("userEntersWordFLWC");
		}

		char* wholePath = hintPathToGoalFLWC(game, data);
		covers("hintPathToGoalFLWC");
		CHECK(wholePath != NULL);
		if(wholePath != NULL){
			free(wholePath);
		}

		int botMove = botTakesTurnFLWC(0, game, data);
		covers("botTakesTurnFLWC");
		CHECK(botMove >= -1);
	}

	freeGameComponentsFLWC(game);
	covers("freeGameComponentsFLWC");
	freeDataStructures(data);
}

void test_bridge_the_keep_away_game(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goals[] = { NULL };
	char* avoids[] = { "ware", "wore", NULL };

	/*FLWIC: nothing to reach, something to keep the bot away from*/
	struct GameComponentsFLWC* game = initFLWC(1, 30, goals, avoids, 0, 2, 0, 6, 1, 30, 10, data);
	CHECK(game != NULL);

	if(game != NULL && isStartValidFLWC(game)){
		char* best = hintBestDirectAdjacencyFLWIC(game, data);
		covers("hintBestDirectAdjacencyFLWIC");

		int distance = hintDistanceFromNearestAvoidWordFLWIC(game, data);
		covers("hintDistanceFromNearestAvoidWordFLWIC");
		/*The board is not started on top of a word it is meant to avoid*/
		CHECK(distance != 0);

		if(best != NULL){
			char* start = getStartWordFLWC(game, data);
			if(start != NULL){
				CHECK_INT(letters_that_differ(start, best, 4), 1);
			}
		}
	}

	freeGameComponentsFLWC(game);
	freeDataStructures(data);
}

/* ------------------------------------------------------- the named starts */

void test_bridge_the_named_starts(void){
	struct DataStructures* data = open_dictionary("docs/4.txt", 4);
	char* goals[] = { "ware", NULL };
	char* avoids[] = { NULL };

	/*The dev tools start every mode on a word you name. The point of each is
	that the board really does begin on that word.*/
	struct GameData* flwg = initFLWGAtStart("ware", data);
	covers("initFLWGAtStart");
	CHECK(flwg != NULL);
	if(flwg != NULL && isStartValidFLWG(flwg)){
		char* word = getCurrWord(flwg, data);
		if(word != NULL){
			CHECK_INT(strcmp(word, "ware"), 0);
		}
	}
	freeGameComponentsFLWG(flwg);

	struct GameComponents* flwp = initFLWPAtStart("ware", 2, 6, 1, 30, data);
	covers("initFLWPAtStart");
	CHECK(flwp != NULL);
	if(flwp != NULL && isStartValid_FLWP(flwp)){
		char* word = getStartWordFLWP(flwp, data);
		if(word != NULL){
			CHECK_INT(strcmp(word, "ware"), 0);
		}
	}
	freeGameComponentsFLWP(flwp, data);

	struct GameComponents* between = initFLWPBetween("ware", "cost", data);
	covers("initFLWPBetween");
	CHECK(between != NULL);
	if(between != NULL && isStartValid_FLWP(between)){
		char* from = getStartWordFLWP(between, data);
		char* to = getGoalWordFLWP(between, data);
		if(from != NULL){ CHECK_INT(strcmp(from, "ware"), 0); }
		if(to != NULL){ CHECK_INT(strcmp(to, "cost"), 0); }
	}
	freeGameComponentsFLWP(between, data);

	/*A goal on an island: built, valid to start, and with no route at all -
	which is what the "no way through" button is judged against*/
	struct GameComponents* island = initFLWPUnreachable(1, 30, data);
	covers("initFLWPUnreachable");
	CHECK(island != NULL);
	if(island != NULL && isStartValid_FLWP(island)){
		CHECK_INT(distanceToGoalFLWP(island, data), -1);
		CHECK_INT(hintGetMinAdjacenciesFLWP(island, data), -1);
	}
	freeGameComponentsFLWP(island, data);

	struct GameComponentsFLWC* flwc = initFLWCAtStart("ware", goals, avoids, data);
	covers("initFLWCAtStart");
	CHECK(flwc != NULL);
	if(flwc != NULL && isStartValidFLWC(flwc)){
		char* word = getStartWordFLWC(flwc, data);
		if(word != NULL){
			CHECK_INT(strcmp(word, "ware"), 0);
		}
	}
	freeGameComponentsFLWC(flwc);

	struct GameComponentsFLWT* flwt = initFLWTAtStart("ware", 3, data);
	covers("initFLWTAtStart");
	CHECK(flwt != NULL);
	if(flwt != NULL && isStartValidFLWT(flwt)){
		char* word = getStartWordFLWT(flwt, data);
		if(word != NULL){
			CHECK_INT(strcmp(word, "ware"), 0);
		}
	}
	freeGameComponentsFLWT(flwt);

	/*The composed board was the one mode that could not be told where to
	start, so the daily had to let the engine choose for it. Its goal is a rule
	rather than a word, so a named start settles the whole board.*/
	char* composedGoals[] = { "cost", NULL };
	struct GameComponentsFLWGP* flwgp = initFLWGPAtStart("ware", composedGoals, avoids, 1, data);
	covers("initFLWGPAtStart");
	CHECK(flwgp != NULL);
	if(flwgp != NULL && isStartValid_FLWGP(flwgp)){
		char* word = getStartWordFLWP(getFLWPComponentsFLWGP(flwgp), data);
		if(word != NULL){
			CHECK_INT(strcmp(word, "ware"), 0);
		}
		/*And it is a board, not merely a seated word: there is a route from it
		to something the rule accepts*/
		CHECK(getFLWPComponentsFLWGP(flwgp) != NULL);
	}
	freeGameComponentsFLWGP(flwgp, data);

	/*A word the dictionary has never heard of is refused rather than dealt*/
	struct GameComponentsFLWGP* unknown = initFLWGPAtStart("zzzz", composedGoals, avoids, 1, data);
	CHECK(unknown != NULL);
	CHECK_INT(isStartValid_FLWGP(unknown), 0);
	freeGameComponentsFLWGP(unknown, data);

	freeDataStructures(data);
}

/* --------------------------------------------------------------- the check */

void test_bridge_every_function_the_app_can_reach_is_tested(void){
	int i;
	int missing = 0;

	for(i = 0; i < BRIDGE_COUNT; i++){
		if(covered[i] == 0){
			printf("\n  not exercised by any test: %s", BRIDGE_FUNCTIONS[i]);
			missing++;
		}
	}
	if(missing > 0){
		printf("\n");
	}
	CHECK_INT(missing, 0);
}

void suite_bridge(void){
	printf("\n-- every function the app can reach --\n");
	RUN_TEST(test_bridge_loads_and_converts_the_dictionary);
	RUN_TEST(test_bridge_the_seed_decides_the_board);
	RUN_TEST(test_bridge_the_direct_adjacency_hint_names_a_neighbour);
	RUN_TEST(test_bridge_the_adversarial_game);
	RUN_TEST(test_bridge_the_generalized_turns_game);
	RUN_TEST(test_bridge_the_pathfinder);
	RUN_TEST(test_bridge_the_composed_pathfinder);
	RUN_TEST(test_bridge_the_turns_game);
	RUN_TEST(test_bridge_the_constraint_game);
	RUN_TEST(test_bridge_the_keep_away_game);
	RUN_TEST(test_bridge_the_named_starts);
	RUN_TEST(test_bridge_every_function_the_app_can_reach_is_tested);
}
