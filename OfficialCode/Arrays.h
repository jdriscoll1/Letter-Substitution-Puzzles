#ifndef seenArray
#define seenArray

#include "WordLinkedList.h"
//extern int numLetters;
/*A class entirely dedicated to arrays
The creation, filling, and freeing of arrays
Date: 9/3/20*/

enum arrayType{
	INT, CHAR, STRING
}arrayType;


//This is the single array structure, convenient if you need the length type and array in one neat package
struct array{
	//The length of the array
	int length; 
	//The actual array
	void** arr; 
	//The type of the array 
	enum arrayType type; 
};

//This is a 2D array structure, convenient if you need the length type and array in one neat package
struct array2D{
	//This is the 1st diemnsion { {}, {}, {} }{ {}, {}, {} } = 2
	int dim1Length;
	//This is the 2nd dimension { {}, {}, {} }{ {}, {}, {} } = 3
	int dim2Length;  
	//This is the 2nd dimensional array
	void*** arr; 
	//This is the type of the array
	enum arrayType type; 	
	
};

//This initalizes the array structure
struct array* init_Array(int length, enum arrayType type);

//This initliazes the 2D array structure
struct array2D* init_Array2D(int dim1Size, int dim2Size, enum arrayType type); 

//This frees the 1D array structure
void free_Array(struct array* a); 

//This frees the 2D array structure
void free_2DArray(struct array2D* a); 




/*Create array of void pointers
@param size --> the size of the array
#remember --> it is always necessary to explicitly determine what the array should be */
void** Allocate_Array(int size);

/*Prints the Array, single dimensional
@param size --> the size of the array
@param array --> the array whose values are to be printed
@arrayType type --> the type of array that it is, it needs to be determined*/ 
void Print_Array(int size, void** array, enum arrayType type); 


/*Converts a word list into a string dimensional array. This creates a new array, it doesn't copy it onto the back
#Note: It does not have to be void*** because a word linked list will only store char* values
@param header --> the header of the linked list whose values will be copied onto the array 
@return --> Returns a String array*/ 
char** ConvertWordLLTo2DArray(struct word *header); 

/*Randomize array*/ 
/*Fill string array*/   
/*Goes through and allocates a 2D array*/ 
void*** Allocate_2DArray(int dim1Size, int dim2Size);

/*Goes through and prints a 2 dimensional array
@param dim1Size --> the size of the first dimension
@param array --> the array that will be printed out*/ 
void Print_2DArray(int dim1Size, void*** array, enum arrayType type); 

/*Starting at the minimum and ending with the maximum number this will fill the array
@param min --> the starting number
@param max --> the ending number
@Note --> Must be an integer array
@Note --> To get the length, make sure to do max - min + 1. 
@Returns an integer array with the values from min all the way to and including max*/ 
int* FillRange_Array(int min, int max); 


/*This does a binary search on an array*/
int BinarySearch_Array(int size, void* goal, void* array, enum arrayType type); 

/*Copies one array onto the other
@param dim1Size --> the size of the first dimension of each array
@param array1 --> the receiver of the copy
@param array2 --> the array being copied onto array 1
@type --> the type of object that the array is*/ 
void Copy_2DArray(int dim1Size, void*** array1, void*** array2, enum arrayType type); 

/*Goes through a 2-Dimensional Array & Frees all of their values
@param dim1Size --> the size of the first dimension of the array
@param array --> the array to be freed
@param skipFirst --> Sometimes, the first one will be freed somewhere else, so it is necessary to not free it*/ 
void Free_2DArray(int dim1Size, void*** array, int skipFirst); 
 


int compare_Array(void* array, int index, void* c2, enum arrayType type); 
  
char** ExtrapolateAllWords(); 

#endif
