#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "PathfinderGame.h"
#include "GameFunctions.h"
#include "UserInput.h"

extern int numLetters; 

//This will initialize the pathfinder game
struct PathfinderGame* init_PathfinderGame(){
	struct PathfinderGame* pc = malloc(sizeof(struct PathfinderGame)); 
	pc->currRound = 2; 
	pc->hintPoints = 50;
	int i; 
	for(i = 0; i < NUM_ROUNDS; i++) {
		pc->scores[i] = 0;
	}  
	pc->numRounds = NUM_ROUNDS; 

	return pc; 
	
}




//One that happens if the user chooses not to redo
//The curr round should go up one
//The curr start and curr goal should change

//One that happens if the user chooses to redo
//Nothing should update 


void Play_FLWP(struct wordConnections*** HashMap, char** wordStorage, char** allWords){
	struct PathfinderGame* pc = init_PathfinderGame(); 
	int score = 0; 
	int i; 
	//This initialises the game components
 	struct GameComponents* gc = InitializeGameComponents(allWords, HashMap, pc->currRound); 
 	
	//This goes through and plays multiple rounds
	while(score != -1 && pc->currRound < pc->numRounds){
		
		//This starts a round
		//quit is based on whether the user chooses to quit
		score = round_FLWP(pc->currRound, allWords, HashMap, gc, pc); 
			//Sets the hint points left 
		pc->hintPoints = gc->hc->hintPoints; 
		pc->scores[pc->currRound - 2] = score; 
		//If quit is equal to 0, the user wants to continue 
		if(score != -1){
			
			//Checks if the user would like to continue
			int quit = ContinueGames();
			if(quit != 1){
							//The user wants to advance
				if (quit == 0){
				 
					//curr round goes back 1
					pc->currRound++;  
					
				}
				
				else if(quit == 2){
				
					if(pc->currRound > 2){
						
							pc->currRound -= 1; 
					}
					
				}
		
				
				
				
			}
			else{
				score = -1; 
			}	
			
			//This frees the game components
			
			
			
			//Initialize the Game Componenents again
			if(quit != 4){
		
				FreeGameComponents(gc); 
				gc = InitializeGameComponents(allWords, HashMap, pc->currRound); 	
			}
			else{
				ResetGameComponents(gc); 
				
			}
		
		}
			 
		
		//2 things that we have to do: 
		//a: Make it so that the hint points don't reset 
		//b: Make it so that the user can redo, quit or try again
		//c: Make sure that it won't get caught in an infinite loop 
		//If the user chooses to quit, it is necessary to free teh components
			else{
				printf("Play Again Soon!"); 
				score = -1; 
			}		
		
	}
	FreeGameComponents(gc); 
	printf("Final Score: %d%%", finalScore(pc)); 	

	free(pc); 
}


int finalScore(struct PathfinderGame* pc){
	int score = 0;
	int i; 
	for(i = 0; i <= pc->currRound - 2; i++){
		score += pc->scores[i];  
		
	}  
	score /= pc->currRound - 1; 
	
	
	return score; 
}
//Advance
//Replay
//Backwards
//Quit


//So, what exactly do I need to do? 
//I need to completely remove all initializations and fres out of the round. That should all be done outside of it
//I need to put gc in the header so that I can pull it
//I need to make reinitialize GameComponents in the Redoing part 
