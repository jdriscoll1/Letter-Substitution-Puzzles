#ifndef seenArrayList
#define seenArrayList


#include <stddef.h>

#include "../../api/includes/FLWC-API.h"
//String, Character, Int, Double, TreeStorageNode
enum alistType{
		STR, CHARACTER, NUM, DOUBLE, TSN, STR_ARR
};

struct arrayList{
	//What does this 
	void* list; 
	//How big is it to begin with? 
	size_t initSize; 
	//How much should it move if it does need to grow or shrink
	size_t moveSize; 
	//What is it's current size? 
	size_t currSize;
	//This is EXACTLY how big it is 
	size_t currPrecision;  
	
	enum alistType type;  
	
};


//Initialize it
struct arrayList* init_ArrayList(size_t initSize, size_t moveSize, enum alistType type); 

//Add to it
void add_ArrayList(void* data, struct arrayList* aList, enum alistType type); 

/*This removes a piece of data from an array list
@param data --> The data to be removed
@param aList --> The array list which is to be removed from
@param type --> The type of data
@time --> O(n)*/
void remove_ArrayList(void* data, struct arrayList* aList, enum alistType type); 

/*Add a string to an array list
@param str --> The string to be added
@param strLen --> The length of the string to be added
@param aList --> The array list which is being added to*/
void addString_ArrayList(const char* str, int strLen, struct arrayList* aList); 

//Prints out the array list
void print_ArrayList(struct arrayList* aList, enum alistType type); 
//Remove a certain number from the back of it 
void removeBackStr_ArrayList(int length, struct arrayList* aList); 
//Frees the array list
void free_ArrayList(struct arrayList* aList); 

//Search for index
void* searchIndex_ArrayList(int index, struct arrayList* aList); 

//Search for a piece of information
int searchData_ArrayList(void* data, struct arrayList* aList); 


char* idArrayListToString(struct arrayList* aList, struct DataStructures* data);

#endif


//How am I going to get strings to work? 
//Well, I already have the string move to back method
//So, then, what I do. Well, really, I don't want to think of it as a string, I instead want to think of it as an array of characters
//I'll start off with 
//First let's get it compatable with chars
//Then, I'll get a basic string to work, Well I'll start off by defining a linked list of strings, then I'll move them in 

