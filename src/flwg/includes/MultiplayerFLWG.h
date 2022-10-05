/*
Name: MultiplayerFLWG.h
Date: 9/18/21
Author: Jordan Driscoll
Description: This is for methods that allow the Four Letter Word Game to be play-able in multiplayer 
*/

#ifndef seenMultiplayerFLWG
#define seenMultiplayerFLWG

#include "../../structs/includes/HashMap.h"
#include "../../structs/includes/TreeSet.h"
#include "../../structs/includes/WordSet.h"

void Multiplayer_FLWG(struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet);

void MultiplayerTest(struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet);

int multiBotPly(int wordID, int playerID, int numPlayers, int depth, struct wordDataArray *IntToWord_HashMap, struct WordSet* wordSet);





#endif
