#ifndef seenPathGame
#define seenPathGame

#include "2DLinkedList.h"
#include "PathGameComponents.h"

#define NUM_ROUNDS 10

struct PathfinderGame{
	//This is the current round number the user is on
	int currRound; 
	//This is the total number of rounds they will play
	int numRounds; 
	//This is the number of hint points they current have
	int hintPoints; 
	//This is there total Score 
	int scores[NUM_ROUNDS]; 


}; 
//This will initialize the pathfinder game
struct PathfinderGame* init_PathfinderGame();


void Play_FLWP(struct wordConnections*** HashMap, char** wordStorage, char** allWords); 

/*This gives the final score of the game*/
int finalScore(struct PathfinderGame* pc); 

#endif
