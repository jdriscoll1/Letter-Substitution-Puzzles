#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ArrayList.h"
#include "UserInput.h"
/*This is the array list class*/
//Initialize it
struct arrayList* init_ArrayList(size_t initSize, size_t moveSize, enum alistType type){
	struct arrayList* aList = malloc(sizeof(struct arrayList)); 
	aList->list = malloc((type == NUM) ? sizeof(int) * initSize : sizeof(char) * initSize); 	
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
	printf("%zu >= %zu\n", aList->currPrecision, aList->currSize);
	size_t typeSize;  
	if(aList->currPrecision + 1 >= aList->currSize){
		typeSize = aList->currSize + aList->moveSize; 
		
		
		//if it's not big enough, I have to re-allocate the current array list
		typeSize *= (type == NUM) ? sizeof(int) : sizeof(char); 
		aList->list = realloc(aList->list, typeSize); 		
		 
			 

		printf("Realloc\n"); 
		aList->currSize = aList->currSize + aList->moveSize; 

	}
     
	//Move one index further in the array list
	
	if(type == NUM){
	
		((int*)(aList->list))[aList->currPrecision] = *(int*)data; 
	}
	if(type == STR){
	
		((char*)(aList->list))[aList->currPrecision] = *(char*)data; 
		((char*)(aList->list))[aList->currPrecision + 1] = '\0';
	}
	aList->currPrecision++; 
	
}

//This adds a string to an array list
void addString_ArrayList(char* str, struct arrayList* aList){
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
				printf("%d", ((int*)(aList->list))[i]); 
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



