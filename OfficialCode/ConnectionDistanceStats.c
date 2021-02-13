#include <stdio.h>
#include <stdlib.h>

/*This source file is going to be dedicated to all of the minConnections (2-12) and their mean and upper/lower quartile distance to determine the most efficent arrayList*/
#include "Arrays.h"
#include "HashMap.h"
#include "BreadthFirstSearch.h"
//So I need a 2D array of integers with 15 3-long arrays
//0: 2 long [the number of all of the words that connect, the one that's the absolute longest, number of words that actually connect this far out] 

void StatTracker(){
	
	
	struct wordConnections **(*HashMap) = AllocateHashMap();	
	char** wordStorage = FillHashMap(HashMap, 0);
	int arrLength = 20; 
	//This is the 2D array that stores the numbers
	int** stats = (int**)Allocate_2DArray(arrLength, 3);
	int i; 
	int ii; 
	for(i = 0; i < arrLength; i++){
		for(ii = 0; ii < 2; ii++){
			stats[i][ii] = 0; 
			
		}
		stats[i][2] = 1; 
		
	}
	//This stores all the worsd
	char** allWords = ExtrapolateAllWords();
	int wordIndex = 0; 
	for(wordIndex = 0; wordIndex <2234  /*length of the word array*/; wordIndex++){

		int minConnections; 
		for(minConnections = 2; minConnections < arrLength + 2; minConnections++){
			int numOptions = BreadthFirstSearch_DistanceOptions(allWords[wordIndex], minConnections, HashMap, HASH_SET);
			if(numOptions > 0){
			
				 
				//Now we go through each of the connection from 2-15
				//For [0] - Total Number of Options
				stats[minConnections - 2][0] += numOptions;

				//For [1] - For the maximum number of connections
				if(numOptions > stats[minConnections - 2][1]){
					stats[minConnections - 2][1] = numOptions; 
				}
				//For [2] - For the number of words that can actually connect to this distance  
				stats[minConnections - 2][2]++; 
			}
		}
	}
	
	for(i = 0; i < arrLength; i++){
		//Mean
		stats[i][0] /= stats[i][2];
		
		//Upper Quartile Range
		int UQR = (stats[i][0] + stats[i][1]) / 2; 
		printf("MinConnection: %d\n", i + 2);
		printf("Mean: %d, Max: %d, UQR: %d, totalConnectable: %d\n\n", stats[i][0], stats[i][1], UQR, stats[i][2]);  
	}
	
	
	
	
}
