/*C File for the Multiplayer FLWG*/

#include <stdlib.h>
#include <stdio.h>

#include "../includes/MultiplayerFLWG.h"
#include "../includes/FLWGGame.h"

#include "../../algs/includes/MaxN.h"
#include "../../algs/includes/Hypermax.h"

#include "../../flwp/includes/GameFunctions.h"


//This allows the four letter word game to be playable with multiple players
void Multiplayer_FLWG(struct DataStructures *data){

	int numPlayers = 3;

	int wordID = ChooseStart(data->I2W);
	int depth = 6;
	
	markUsed_WordSet(wordID, data->wordSet);
	
	int currPlayer = 0;
	printf("Start: %s\n", Convert_IntToWord(wordID, data->I2W));
	while(wordID != -1){
		
		switch(currPlayer){
		
			case 0:
				wordID = userPly(wordID, data); 
				break;

			case 1:
				wordID = userPly(wordID, data); 
				break;

			
			case 2:
				wordID = multiBotPly(wordID, currPlayer, numPlayers, depth, data->I2W, data->wordSet);
				break;
			

		}
		//if the player quit, let the algorithm know
		if(wordID != -1){
			printf("%c) %s\n", (char)((currPlayer) % numPlayers + 65), Convert_IntToWord(wordID, data->I2W));
			//move to next player
			currPlayer = (currPlayer + 1) % numPlayers;
		}
		
	}
	reset_HashSet(data->I2W);
	printf("Player %c Loses!", (char)(currPlayer + 65));
	
}

void MultiplayerTest(struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet){
	int numPlayers = 4;
	int i;
	int* wins = malloc(sizeof(int) * numPlayers);
	for(i = 0; i < numPlayers; i++){
		wins[i] = 0;
	}
	
	for(i = 0; i < 100; i++){

		int wordID = i;
		
		int depth = 6;
		
		markUsed_WordSet(wordID, wordSet);
		
		int currPlayer = 0;
		
		printf("Start: %s\n", Convert_IntToWord(wordID, IntToWord_HashMap));
		while(wordID != -1){
			switch(currPlayer){
			
				case 0:
					wordID = Hypermax(wordID, currPlayer, numPlayers, depth, IntToWord_HashMap, wordSet);
					break;
				
				case 1:
					wordID = weakBotPly(wordID, IntToWord_HashMap, wordSet);
					break;
					
				case 2:
					wordID = weakBotPly(wordID, IntToWord_HashMap, wordSet);
					break;
				case 3:
					wordID = weakBotPly(wordID, IntToWord_HashMap, wordSet);
					break;
				case 4:
					break;
					
				
				
			}
			printf("%s\n", Convert_IntToWord(wordID, IntToWord_HashMap));
			//if the player quit, let the algorithm know
			if(wordID != -1){
				//move to next player
				currPlayer = (currPlayer + 1) % numPlayers;
			}
			
		}
		reset_WordSet(wordSet);
		printf("%d) Player %c Loses!\n\n\n", i, (char)(currPlayer + 65));

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

int multiBotPly(int wordID, int playerID, int numPlayers, int depth, struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet){
	struct maxnNodeScore* output = MaxN(wordID, playerID, numPlayers, depth, depth, IntToWord_HashMap, wordSet);
	int newWordID = output->wordID;
	Free_MaxNNodeScore(output, numPlayers);
	if(newWordID == -1){
		return -1; 
	}
	markUsed_WordSet(newWordID, wordSet);
	return newWordID;
	
}

