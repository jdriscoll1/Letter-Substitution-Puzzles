/*
Name: Hypermax.h
Author: Jordan Driscoll
Date: 9.20.21
Description: Multiplayer FLWG with Node Culling, or Alpha-Reduction*/

#include "MaxN.h"

struct maxnNodeScore* Hypermax(int wordID, int playerID, int numPlayers, int depth, int maxDepth, struct maxnNodeScore* alphas, struct wordDataArray* IntToWord_HashMap));
