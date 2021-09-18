/*C File for the Multiplayer FLWG*/

#include "../includes/MultiplayerFLWG.h"
#include "../includes/FLWGGame.h"

#include "../../algs/includes/MaxN.h"


//This allows the four letter word game to be playable with multiple players
void Multiplayer_FLWG(struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray *IntToWord_HashMap){
	
	int numPlayers = 2;
	
	int wordID = 0;
	
	int depth = 3;
	
	setAlgFound(wordID, IntToWord_HashMap);
	
	int currPlayer = 0;
	
	while(wordID != -1){
		printf("%s\n", Convert_IntToWord(wordID, IntToWord_HashMap));
		switch(currPlayer){
		
			case 0:
				wordID = multiBotPly(wordID, currPlayer, numPlayers, depth, IntToWord_HashMap);
				break;
			
			case 1:
				wordID = userPly(wordID, WordToInt_HashMap, IntToWord_HashMap);
				break;
		}
		currPlayer = (currPlayer + 1) % numPlayers;
	}
	
	
	
	
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
