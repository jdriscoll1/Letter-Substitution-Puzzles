#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../includes/PathfinderGame.h"
#include "../includes/GameFunctions.h"
#include "../includes/UserInput.h"

//This will initialize the pathfinder game
struct PathfinderGame* init_PathfinderGame(){
	struct PathfinderGame* pc = malloc(sizeof(struct PathfinderGame)); 
	pc->currRound = 2; 
	pc->hintPoints = 50;
	int i; 
	for(i = 0; i < NUM_ROUNDS; i++) {
		pc->scores[i] = 0;
	}  
	pc->numRounds = 50;

	return pc; 
	
}




//One that happens if the user chooses not to redo
//The curr round should go up one
//The curr start and curr goal should change

//One that happens if the user chooses to redo
//Nothing should update 


void Play_FLWP(struct DataStructures *data){
	struct PathfinderGame* pc = init_PathfinderGame(); 
	int score = 0; 
	int endGame = 0; 

	//This initialises the game components
 	struct GameComponents* gc = InitializeGameComponents(data->I2W, pc->currRound, data->wordSet, 20);
 	
	//This goes through and plays multiple rounds
	while(endGame == 0 && pc->currRound < pc->numRounds){
		
		//This starts a round
		//quit is based on whether the user chooses to quit
		score = round_FLWP(gc, pc, data); 
			//Sets the hint points left 
		pc->hintPoints = gc->hc->hintPoints; 
		pc->scores[pc->currRound - 2] = score; 
		printf("Score: %d", score); 
		//If quit is equal to 0, the user wants to continue 
		if(score != -1){
			
			//Checks if the user would like to continue
			int quit = ContinueGames();
			if(quit != 1){
				//The user wants to advance
				if (quit == 0){
				 
					//curr round goes foward 1
					pc->currRound++;  
					
				}
				//The user wants to go backwards
				else if(quit == 2){
				
					if(pc->currRound > 2){	
							pc->currRound -= 1; 
					}
					
				}
		
				
				
				
			}
			else{
				score = 0; 
				endGame = 1; 
			}	
			
			
			
			//Initialize the Game Componenents again
			if(quit != 4){
		
				FreeGameComponents(gc, data->I2W); 
				gc = InitializeGameComponents(data->I2W, pc->currRound, data->wordSet, 20);
			}
			else{
				ResetGameComponents(gc, data->I2W); 
				
			}
		
		}
			 
		
		//2 things that we have to do: 
		//a: Make it so that the hint points don't reset 
		//b: Make it so that the user can redo, quit or try again
		//c: Make sure that it won't get caught in an infinite loop 
		//If the user chooses to quit, it is necessary to free teh components
			else{
				printf("Play Again Soon!"); 
				score = 0; 
				endGame = 1; 
			}		
		
	}
	
	FreeGameComponents(gc, data->I2W); 
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
