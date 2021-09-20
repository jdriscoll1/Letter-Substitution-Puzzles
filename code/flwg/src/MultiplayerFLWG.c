/*C File for the Multiplayer FLWG*/

#include <stdlib.h>
#include <stdio.h>

#include "../includes/MultiplayerFLWG.h"
#include "../includes/FLWGGame.h"

#include "../../algs/includes/MaxN.h"


//This allows the four letter word game to be playable with multiple players
void Multiplayer_FLWG(struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray *IntToWord_HashMap){
	
	int numPlayers = 2;
	
	int wordID = 1430;
	int depth = 4;
	
	setAlgFound(wordID, IntToWord_HashMap);
	
	int currPlayer = 0;
	
	while(wordID != -1){
		printf("%c) %s\n", (char)((currPlayer + 1) % numPlayers + 65), Convert_IntToWord(wordID, IntToWord_HashMap));
		switch(currPlayer){
		
			case 0:
				wordID = multiBotPly(wordID, currPlayer, numPlayers, depth, IntToWord_HashMap);
				break;
			
			case 1:
				wordID = weakBotPly(wordID, IntToWord_HashMap);
				break;
		}
		//if the player quit, let the algorithm know
		if(wordID != -1){
			//move to next player
			currPlayer = (currPlayer + 1) % numPlayers;
		}
		
	}
	reset_HashSet(IntToWord_HashMap);
	printf("Player %c Loses!", (char)(currPlayer + 65));

	
	
	
}

void MultiplayerTest(struct wordDataArray *IntToWord_HashMap){
	int numPlayers = 2;
	int i;
	int* wins = malloc(sizeof(int) * numPlayers);
	for(i = 0; i < numPlayers; i++){
		wins[i] = 0;
	}
	
	for(i = 0; i < 900; i++){

		int wordID = i;
		
		int depth = 4;
		
		setAlgFound(wordID, IntToWord_HashMap);
		
		int currPlayer = 0;
		
		
		while(wordID != -1){
			switch(currPlayer){
			
				case 0:
					wordID = multiBotPly(wordID, currPlayer, numPlayers, depth, IntToWord_HashMap);
					break;
				
				case 1:
					wordID = weakBotPly(wordID, IntToWord_HashMap);
					break;
			}
			//if the player quit, let the algorithm know
			if(wordID != -1){
				//move to next player
				currPlayer = (currPlayer + 1) % numPlayers;
			}
			
		}
		reset_HashSet(IntToWord_HashMap);
		//printf("Player %c Loses!\n", (char)(currPlayer + 65));
		if((char)(currPlayer + 65) == 'A'){
			printf("Lose: %d", i);
		}
		int j; 
		for(j = 0; j < numPlayers; j++){
			if(j != currPlayer){
				wins[j]++;
			}
		}
	
	
	}
	printf("{");
	for(i = 0; i < numPlayers; i++){
		printf("%d%s", wins[i], (i == numPlayers - 1) ? "" : ", ");
	}
	printf("}");
	free(wins);
}

int multiBotPly(int wordID, int playerID, int numPlayers, int depth, struct wordDataArray *IntToWord_HashMap){
	struct maxnNodeScore* output = MaxN(wordID, playerID, numPlayers, depth, depth, IntToWord_HashMap);
	int newWordID = output->wordID;
	Free_MaxNNodeScore(output, numPlayers);
	if(newWordID == -1){
		return -1; 
	}
	setAlgFound(newWordID, IntToWord_HashMap);
	return newWordID;
	
}
