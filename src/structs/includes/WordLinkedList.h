#ifndef seenWordList
#define seenWordList
/*
Singularly Linked List Node Functions
Uses: Stores the word connections from the start to the goal
The Queue of the words whose connections will be checked 
*/
/*The Queue's Node --> The single most basic type of linked list lol
#Note: When creating this, make sure to initialize the dataMalloc to 0*/

/*Enum --> It's main purpose is when it is time to print a linked list:
@type LINKED --> outputs list: ties->pies->lies->etc.
@type SEPERATED --> outputs list: ties, pies, lies, etc. */
enum output{
	LINKED, SEPERATED, LINES
};

struct word{
	/*The Next location*/
	struct word *next; 
	/*The word it currently stores*/
	char* word;
	/*Does the user use strdup to allocate the information*/
	int dataMalloc;  
	//How many pieces of data are in this particular linked list
	int size; 
	
};

/*Gets the size of a linked list, starting the count from 1
@param header --> the header of the linked list
@return --> returns the size
#example [SEPERATED](ties, lies, dies) size: 3*/ 
int getSize(struct word *header); 

/*Word Node Methods*/
/*Adds to the front of the linked list for efficency reasons
@param word --> the word to be added to the front of the linked list
@param header --> the header whose neighboor shall soon be the word*/
void AddToFront_WordLL(char* word, struct word *header, int dataMalloc); 

/*Adds a node to the back of the word linked list
@param word --> the word to be added
@param header --> the DHN's location*/
void AddToBack_WordLL(char* word, struct word *header, int dataMalloc);

/*Removes a specified word from the linked list
@param word --> the word to be removed
@param header --> the head of the linked list*/ 
void Remove_WordLL(char* word, struct word *header);  

/*Remove & Frees the first index
@param header --> the DHN's location*/
void RemoveFront_WordLL(struct word *header);

/*Remove the last node in the linked list
@param header --> the front of the linked list*/
void RemoveBack_WordLL(struct word *header);  

void RemoveFrom_WordLL(char* word, struct word *header); 

/*Searches for a word in the word linked list
@param word --> the word to be searched for
@param header --> the linked lists' DHN
@return --> 1 -- word is found 0 -- word is to be found*/ 
int Search_WordLL(char* word, struct word* header); 

/*Loops through the linked list and returns the last word
@param header --> The front of the linked list
@return --> Returns the last word in a linked list*/
char* FindLast_WordLL(struct word* header); 

/*Print the word Linked List
@param header --> the DHN's location*/
void Print_WordLL(struct word *header, enum output o);

/*Converts a linked list to an array
@param header --> the DHN of the linked list
@return --> Returns an array of strings (char*) */
char** ConvertWordLLToArray(struct word *header);
	
/*This takes the word linked list and converts it to a string
@param header -->The header of the word linked list
@param output o --> This is the way that it is necessary to output it
#Note: You must free the string that you are given*/
char* toString_WordLL(struct word* header, enum output o, int numLetters); 

/*Frees the linked list
@param header --> the DHN's location
#Note: If you did not set the dataMalloc on the header, you will get a valgrind error*/
void Free_WordLL(struct word *header);

/*Copies one linked list onto another linked list
@param copiedTo --> linked list onto which copiedFrom will be copied
@param copiedFrom --> linked list whose itmes will be copied
@return --> Returns the new linked list*/
struct word *Copy_WordToWordLL(struct word *copiedTo, struct word *copiedFrom);
#endif
