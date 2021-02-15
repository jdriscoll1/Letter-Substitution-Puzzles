#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ArrayList.h"
#include "UserInput.h"
#include "TreeStorageNode.h"
/*This is the array list class*/
//Initialize it
struct arrayList* init_ArrayList(size_t initSize, size_t moveSize, enum alistType type){
	struct arrayList* aList = malloc(sizeof(struct arrayList)); 
	if(type == TSN){
		aList->list = malloc(sizeof(struct TreeStorageNode) * initSize); 
	}

	else if(type == NUM){
		aList->list = malloc(sizeof(int) * initSize); 
	}
	else if(type == CHARACTER){
		aList->list = malloc(initSize); 	
	}
	else if(type == STR){
		aList->list = malloc(sizeof(char*) * initSize); 
	}
	//How big is it to begin with? 
	aList->initSize = initSize; 
	aList->currPrecision = 0; 
	
	//How much should it move if it does need to grow or shrink
	aList->moveSize = moveSize; 
	//What is it's current size? 
	aList->currSize= initSize; 
	//What is the array list's type
	aList->type = type;  
	
}


//Add to it
void add_ArrayList(void* data, struct arrayList* aList, enum alistType type){
	//When I add something, I have to make sure it is big enough for it
	size_t typeSize;  
	if(aList->currPrecision + 1 >= aList->currSize){
		typeSize = aList->currSize + aList->moveSize; 
		
		
		//if it's not big enough, I have to re-allocate the current array list
		if(type == TSN){
			typeSize *= sizeof(struct TreeStorageNode); 
		}
		else if(type == NUM){
			typeSize *= sizeof(int); 	
		}
		else if(type == CHARACTER){
		
			typeSize *= sizeof(char); 
		}
		aList->list = realloc(aList->list, typeSize); 		 
			 

		aList->currSize = aList->currSize + aList->moveSize; 

	}
     
	//Move one index further in the array list
	
	if(type == NUM){
	
		((int*)(aList->list))[aList->currPrecision] = *(int*)data; 
	}
	else if(type == TSN){
		((struct TreeStorageNode**)(aList->list))[aList->currPrecision] = (struct TreeStorageNode*)data; 
	}
	else if(type == STR){
	
		((char*)(aList->list))[aList->currPrecision] = *(char*)data; 
		((char*)(aList->list))[aList->currPrecision + 1] = '\0';
	}

	else if(type == CHARACTER){
		((char*)(aList->list))[aList->currPrecision] = *(char*)data; 

	}
	aList->currPrecision++; 
	
}


void remove_ArrayList(void* data, struct arrayList* aList, enum alistType type){
	//So, the best way to have it remove from the array list 
	//Ngl, probably just copy all of them to a new linked list
	//Well it depends, if currSize - 1 <= currSize - moveSize, then we want to shrink it by move size
	//If currSize - 1 > currSize - moveSize, then we want to shrink it by only 1 
	int i;
	//We need to figure out the new size
	//We will allocate the array based on that size
	int newSize = (aList->currSize - 1 <= aList->currSize - aList->moveSize) ? aList->currSize - aList->moveSize : aList->currSize; 
	int newPrecision = aList->currPrecision - 1; 
	//Create a new list that's the current precision - 1
	void* newArr; 
	if(type == CHARACTER){
		newArr = malloc(newSize);	
		
	}   
	int newArrIndex = 0;
	int oldArrIndex;   
	for(oldArrIndex = 0; oldArrIndex < aList->currPrecision; oldArrIndex++){
		//Loop through the list and search for the data
		//If it has not found it, just add it to the list
		//If it has found it, don't add it, and subtract i by 1 
		if(type == CHARACTER){
			if(((char*)(aList->list))[oldArrIndex] == *(char*)data){
				//It'll add it to the new location again, writing over the old one 
				newArrIndex--; 
			}
			else{
				((char*)(newArr))[newArrIndex] = ((char*)aList->list)[oldArrIndex]; 
			}		
	
		}
	
		newArrIndex++; 
		
	}
	free(aList->list); 
	aList->list = newArr; 
	aList->currPrecision = newPrecision; 
	aList->currSize = newSize; 
	
}

//This adds a string to an array list
void addString_ArrayList(const char* str, struct arrayList* aList){
	//So, this has to take the current location, and substr the string onto the back
	//Because I am exceptionally lazy, I'm gonna use the method I already built
	//However, FIRST, il faut que figure out just how big the string I'm gonna add is, and see if I need to allocate space for it 
	int strLen = strlen(str);

	//Make sure it is big enough. 
	//To check this: take the current size (50), subtract it by precise size (47) + strSize (29)
	int offset = aList->currSize - (aList->currPrecision + strLen);
    
	//If it is bigger (by 26), 
	if(offset < 0){
		
		//it has to divide that number (26) by growth size (5)
		//This will give us a double (5.2) we have to ceil it
		offset *= -1; 
		int blocks = (int)ceil(((double)offset / (double)aList->moveSize)); 
		int newSize = aList->currSize + (blocks * aList->moveSize); 
		//I need to make sure that if it's 10, and the word is 10. There's space for a \0. So, to do this, it is necessary that I 
		
		if(aList->currPrecision + strLen == newSize){
			
			newSize += aList->moveSize; 
		}
		aList->list = realloc(aList->list, newSize); 
		aList->currSize = newSize; 
	//Then, we have to realloc the current one by that gotten number (6) * growth size (5)
	//This will give us 30.
	}

	else{

		if(aList->currPrecision + strLen == aList->currSize){		
			aList->list = realloc(aList->list, aList->currSize + aList->moveSize); 
			aList->currSize += aList->moveSize; 
		}
	
	
	}
	//To Check out work, we said initially, 50 + 30 = 80. 
	//and then 47 + 29 = 76
	//Now it is big enough, and we can add the word in using the strcat method 
	//list->precLoc = int safeStrcat(dest. list->list, input.str, int destLength.curSize, int buff.strLen, int start.precLoc); 
	aList->currPrecision = safeStrcat((char**)&aList->list, str, aList->currSize, strLen, aList->currPrecision); 
} 

void removeBackStr_ArrayList(int length, struct arrayList* aList){
	if(length > aList->currPrecision){
		printf("Removing Too Much! [removeBackStr_ArrayList]");
		exit(0);  
	}
	int i;  
	length -= 1; 
	//The distance between the current size and current precision
	int diff = aList->currSize - aList->currPrecision; 
	//To remove, it is necessary to figure out exactly how many you would like to remove
	//Say the current size is 50. Change Size is 5. And you would like to remove 17. 
	//Blocks = You would divide the remove size by change size. So, that is 17/3 = 5. 
	int blocks = length / aList->moveSize; 
	//Bits = And you would keep the extra for precision. So that is 17%5, or 2
	int bytes = length % aList->moveSize; 
	//If blocks is greater than 0, you do have to realloc
	if(blocks > 0){
		int newSize = aList->currPrecision - (blocks * aList->moveSize); 
		aList->list = realloc(aList->list, newSize); 
		//Then, you set the current size to the new number of blocks. Or, currSize - Blocks
		aList->currSize = newSize; 

	}
	
	
	
	
	//Then you set the cur prec size to blocks - bytes
	aList->currPrecision = aList->currSize - diff - bytes - 1;

	//Then, you put a \0 at the curr prec + 1
	((char*)(aList->list))[aList->currPrecision] = '\0'; 
	
	
	
	
}

void print_ArrayList(struct arrayList* aList, enum alistType type){
	int i; 
	if(type == STR){
		printf("%s", (char*)aList->list); 
	}
	//If it's not a string, it'll have to loop through all the characters
	else{
		for(i = 0; i < aList->currPrecision; i++){
			if(type == NUM){
				printf("%d, ", ((int*)(aList->list))[i]); 
			}
			else if(type == TSN){
				printf("%s, ", ((struct TreeStorageNode**)(aList->list))[i]->word); 
			}
			else if(type == CHARACTER){
				printf("%c", ((char*)(aList->list))[i]); 
			}
	
	
		} 
	}
	printf("\n"); 
}
//This frees the array list
void free_ArrayList(struct arrayList* aList){
	free(aList->list); 
	free(aList); 
} 



