/**********************************
Name: debug_console.c
Purpose: Pick a game mode from a menu, name the word it starts on, and play it.

The demos in main.c are each one mode with its parameters written into the
source, so trying a different mode means editing and rebuilding, and every run
puts you somewhere new because the modes choose their own start words. This is
the same set of modes behind a menu, with the start word asked for rather than
chosen, so the same position can be played twice.

The start words come from Debug-API.h, which seats a named word and otherwise
builds the ordinary components. Blank at the prompt falls through to the mode's
own picker, which is how you get a real game out of it.
**********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include "./api/includes/FLWG-API.h"
#include "./api/includes/FLWC-API.h"
#include "./api/includes/FLWT-API.h"
#include "./api/includes/Debug-API.h"

#include "./flwp/includes/PathGameComponents.h"
#include "./flwp/includes/GameFunctions.h"
#include "./flwp/includes/UserInput.h"
#include "./structs/includes/IntLinkedList.h"

#define LINE 256
/*More words than anyone will type at a prompt, plus room for the NULL*/
#define MAX_LIST 64

/*The goal set the FLWC demo in main.c uses, for when you would rather not type
one out. Every word rhymes with the others, which makes the game easy to follow*/
static char* DEFAULT_GOALS[] = {"lack", "lock", "pack", "tack", "back", "sack",
	"rack", "sock", "dock", "rock", "mock", "kick", "lick", "pick", "tick",
	"deck", "neck", "peck", NULL};
static char* NO_WORDS[] = {NULL};

/*A line from the console, trimmed and folded to lower case. Returns 0 at
end of input, so a piped-in script stops rather than spinning*/
static int readLine(const char* prompt, char* buffer, int size){
	int i;

	printf("%s", prompt);
	fflush(stdout);

	if(fgets(buffer, size, stdin) == NULL){
		buffer[0] = '\0';
		return 0;
	}
	for(i = 0; buffer[i] != '\0'; i++){
		if(buffer[i] == '\n' || buffer[i] == '\r'){
			buffer[i] = '\0';
			break;
		}
		buffer[i] = tolower((unsigned char)buffer[i]);
	}
	return 1;
}

/*The start word, or NULL to let the mode choose one for itself*/
static char* askForStartWord(char* buffer, int size){
	readLine("Start word (blank to let it choose)> ", buffer, size);
	return (buffer[0] == '\0') ? NULL : buffer;
}

static int askForNumber(const char* prompt, int fallback){
	char line[LINE];

	readLine(prompt, line, sizeof(line));
	if(line[0] == '\0'){
		return fallback;
	}
	return atoi(line);
}

/*Splits a typed line into a NULL-terminated word list, in place. The pointers
run into buffer, so it has to outlive the list*/
static char** splitWords(char* buffer, char** list, int max){
	int count = 0;
	char* word = strtok(buffer, " \t,");

	while(word != NULL && count < max - 1){
		list[count++] = word;
		word = strtok(NULL, " \t,");
	}
	list[count] = NULL;
	return list;
}

/*Says what the engine made of a word, since the codes on their own are opaque*/
static void reportInput(int code){
	switch(code){
		case VALID: return;
		case TOO_SHORT: printf("  too short\n"); break;
		case TOO_LONG: printf("  too long\n"); break;
		case NOT_ENOUGH_LETTERS_IN_COMMON: printf("  change exactly one letter\n"); break;
		case TOO_MANY_LETTERS_IN_COMMON: printf("  that is the same word\n"); break;
		case WORD_USED: printf("  already played\n"); break;
		case WORD_DOES_NOT_EXIST: printf("  not in the dictionary\n"); break;
		case WRONG_ORDER: printf("  out of order\n"); break;
		default: printf("  rejected (%d)\n", code); break;
	}
}

/*Every mode ends the same way: say why there is no game and give up*/
static int reportUnplayable(void){
	printf("\nNo game there. Either the word is not in the dictionary, or nothing\n");
	printf("in range can be reached from it. Try another, or leave it blank.\n");
	return 0;
}

/* ----------------------------------------------------------------- FLWG --- */

static void playFLWG(struct DataStructures* data, char* startWord, int botType){
	struct GameData* game = (startWord != NULL)
		? initFLWGAtStart(startWord, data)
		: initFLWG(data, 1, 30);
	char line[LINE];

	if(!isStartValidFLWG(game)){
		reportUnplayable();
		freeGameComponentsFLWG(game);
		return;
	}

	printf("\nFLWG -- change one letter each turn. Whoever cannot move loses.\n");
	while(1){
		printf("\n  word: %s\n", getCurrWord(game, data));
		printf("  hint: try the letter '%c' -- %d move(s) available\n",
			hintLetterToConsiderFLWG(game, data), hintNumOptionsFLWG(game, data));

		if(!readLine("you> ", line, sizeof(line)) || strcmp(line, "quit") == 0){
			break;
		}

		int code = userTakesTurn(line, game, data);
		if(code != VALID){
			reportInput(code);
			continue;
		}

		int reply = botTakesTurn(game, data, botType);
		if(reply == -1){
			printf("\n  the bot has nowhere to go -- you win\n");
			break;
		}
		printf("  bot: %s\n", convertIntToWord(reply, data));
	}
	freeGameComponentsFLWG(game);
}

/* ----------------------------------------------------------------- FLWP --- */

static void playFLWP(struct DataStructures* data, char* startWord, char* goalWord){
	struct GameComponents* gc;
	char line[LINE];
	struct intList* step;

	if(startWord != NULL && goalWord != NULL){
		gc = initFLWPBetween(startWord, goalWord, data);
	}
	else if(startWord != NULL){
		gc = initFLWPAtStart(startWord, 2, 8, 1, 30, data);
	}
	else{
		gc = initiateFLWP(1, 30, 2, 8, 1, 30, data);
	}

	if(!isStartValid_FLWP(gc)){
		reportUnplayable();
		freeGameComponentsFLWP(gc, data);
		return;
	}

	printf("\nFLWP -- walk from %s to %s, one letter at a time.\n",
		getStartWordFLWP(gc, data), getGoalWordFLWP(gc, data));
	printf("  shortest route is %d move(s):", gc->solution->size - 1);
	for(step = gc->solution->next; step != NULL; step = step->next){
		printf(" %s", convertIntToWord(step->data, data));
	}
	printf("\n  (type undo, redo or quit at any point)\n");

	while(!isGameWonFLWP(gc)){
		printf("\n  at: %s   goal: %s\n",
			convertIntToWord(getPrevWordFLWP(gc), data), getGoalWordFLWP(gc, data));

		if(!readLine("you> ", line, sizeof(line)) || strcmp(line, "quit") == 0){
			break;
		}
		if(strcmp(line, "undo") == 0){
			undoMoveFLWP(gc, data);
			continue;
		}
		if(strcmp(line, "redo") == 0){
			redoMoveFLWP(gc, data);
			continue;
		}
		reportInput(userEntersWord_FLWP(line, gc, data));
	}
	if(isGameWonFLWP(gc)){
		printf("\n  home in %d move(s)\n", gc->numMoves);
	}
	freeGameComponentsFLWP(gc, data);
}

/* ------------------------------------------------------------ FLWC/FLWIC --- */

static void playFLWC(struct DataStructures* data, char* startWord, int inverse, int botType){
	char listLine[LINE];
	char* list[MAX_LIST];
	char** words;
	struct GameComponentsFLWC* flwc;
	char line[LINE];
	int result;

	readLine(inverse
		? "Words to avoid (space separated, blank for the usual set)> "
		: "Goal words (space separated, blank for the usual set)> ",
		listLine, sizeof(listLine));

	words = (listLine[0] == '\0') ? DEFAULT_GOALS : splitWords(listLine, list, MAX_LIST);

	if(startWord != NULL){
		flwc = inverse
			? initFLWCAtStart(startWord, NO_WORDS, words, data)
			: initFLWCAtStart(startWord, words, NO_WORDS, data);
	}
	else if(inverse){
		flwc = initFLWC(1, 30, NO_WORDS, words, 0, 2, 0, 6, 1, 30, 8, data);
	}
	else{
		flwc = initFLWC(1, 30, words, NO_WORDS, 2, 0, 6, 0, 1, 30, 8, data);
	}

	if(!isStartValidFLWC(flwc)){
		reportUnplayable();
		freeGameComponentsFLWC(flwc);
		return;
	}

	printf(inverse
		? "\nFLWIC -- keep away from the words on the list. The bot is pushing you towards them.\n"
		: "\nFLWC -- reach any word on the list.\n");

	while(1){
		printf("\n  word: %s\n", getStartWordFLWC(flwc, data));
		if(!inverse){
			char* towards = hintAdjacencyTowardsGoalFLWC(flwc, data);
			printf("  hint: %s is %d move(s) from a goal word\n",
				(towards == NULL) ? "(nothing)" : towards,
				hintMinDistanceToGoalFLWC(flwc, data));
		}

		if(!readLine("you> ", line, sizeof(line)) || strcmp(line, "quit") == 0){
			break;
		}

		int code = userEntersWordFLWC(line, flwc, data);
		if(code != VALID){
			reportInput(code);
			continue;
		}

		result = isGameWonFLWC(flwc);
		if(result != -1){
			printf(result == 1 ? "\n  you reached a goal word\n"
				: (result == 2 ? "\n  that was a word to avoid\n" : "\n  stuck -- nobody wins\n"));
			break;
		}

		if(botTakesTurnFLWC(botType, flwc, data) == -1){
			printf("\n  the bot has nowhere to go\n");
			break;
		}
		printf("  bot: %s\n", getStartWordFLWC(flwc, data));

		result = isGameWonFLWC(flwc);
		if(result != -1){
			if(result == 2){
				printf("\n  the bot walked you into an avoid word\n");
			}
			else{
				printf("\n  the game is over (%d)\n", result);
			}
			break;
		}
	}
	freeGameComponentsFLWC(flwc);
}

/* ----------------------------------------------------------------- FLWT --- */

static void playFLWT(struct DataStructures* data, char* startWord, int numToFind){
	struct GameComponentsFLWT* flwt = (startWord != NULL)
		? initFLWTAtStart(startWord, numToFind, data)
		: initFLWT(numToFind, 1, 30, data);
	char line[LINE];

	if(!isStartValidFLWT(flwt)){
		reportUnplayable();
		freeGameComponentsFLWT(flwt);
		return;
	}

	printf("\nFLWT -- name %d word(s) that differ from %s by one letter.\n",
		flwt->minAdjacenciesUserNeedsToFind, getStartWordFLWT(flwt, data));
	printf("  there are %d to find in all\n", flwt->maxAdjacenciesThatCanBeFound);

	while(!isGameWonFLWT(flwt)){
		printf("\n  word: %s   found %d of %d\n", getStartWordFLWT(flwt, data),
			flwt->numAdjacenciesFound, flwt->minAdjacenciesUserNeedsToFind);
		printf("  hint: try the letter '%c' -- %d left\n",
			hint_letterToConsiderFLWT(flwt, data), hint_numOptionsFLWT(flwt, data));

		if(!readLine("you> ", line, sizeof(line)) || strcmp(line, "quit") == 0){
			break;
		}
		reportInput(userEntersWordFLWT(line, flwt, data));
	}
	if(isGameWonFLWT(flwt)){
		printf("\n  all found\n");
	}
	freeGameComponentsFLWT(flwt);
}

/* --------------------------------------------------------------- the menu --- */

static void printMenu(void){
	printf("\n=====================================================\n");
	printf("Letter Substitution Puzzles -- debug console\n");
	printf("=====================================================\n");
	printf("  1  FLWG   two players, whoever cannot move loses\n");
	printf("  2  FLWP   walk from the start word to the goal\n");
	printf("  3  FLWC   reach any word on a list\n");
	printf("  4  FLWIC  keep away from every word on a list\n");
	printf("  5  FLWT   find the neighbours of one word\n");
	printf("  0  quit\n");
}

void debugConsole(void){
	char choice[LINE];
	char startBuffer[LINE];
	char goalBuffer[LINE];

	while(1){
		printMenu();
		if(!readLine("\nmode> ", choice, sizeof(choice))){
			return;
		}
		if(choice[0] == '\0'){
			continue;
		}
		if(strcmp(choice, "0") == 0 || strcmp(choice, "quit") == 0){
			return;
		}

		/*The dictionary is opened per game, so a run can move between word
		lengths without restarting. initDataStructures duplicates the
		descriptor, which leaves this the one that closes it*/
		int numLetters = askForNumber("Word length (2, 3 or 4; blank for 4)> ", 4);
		char path[32];
		if(numLetters < 2 || numLetters > 4){
			printf("  there are only 2, 3 and 4 letter dictionaries\n");
			continue;
		}
		snprintf(path, sizeof(path), "docs/%d.txt", numLetters);

		int fd = open(path, O_RDONLY);
		if(fd == -1){
			printf("  could not open %s -- run this from the repository root\n", path);
			continue;
		}
		struct DataStructures* data = initDataStructures(fd, numLetters);
		char* startWord = askForStartWord(startBuffer, sizeof(startBuffer));

		if(strcmp(choice, "1") == 0){
			playFLWG(data, startWord, askForNumber("Bot: -1 greedy, 0 random, 1+ search depth (blank for 2)> ", 2));
		}
		else if(strcmp(choice, "2") == 0){
			char* goalWord = NULL;
			if(startWord != NULL){
				readLine("Goal word (blank to let it choose)> ", goalBuffer, sizeof(goalBuffer));
				goalWord = (goalBuffer[0] == '\0') ? NULL : goalBuffer;
			}
			playFLWP(data, startWord, goalWord);
		}
		else if(strcmp(choice, "3") == 0){
			playFLWC(data, startWord, 0, askForNumber("Bot: -1 greedy, 0 random, 1+ search depth (blank for 2)> ", 2));
		}
		else if(strcmp(choice, "4") == 0){
			playFLWC(data, startWord, 1, askForNumber("Bot: -1 greedy, 0 random, 1+ search depth (blank for 2)> ", 2));
		}
		else if(strcmp(choice, "5") == 0){
			playFLWT(data, startWord, askForNumber("How many to find (blank for 3)> ", 3));
		}
		else{
			printf("  no such mode\n");
		}

		freeDataStructures(data);
		close(fd);
	}
}
