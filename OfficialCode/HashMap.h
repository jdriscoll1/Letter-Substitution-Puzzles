#ifndef seenHashMap
#define seenHashMap




/*Hash Map Header File
@Goal: Takes a word, and efficently returns all words that share a single letter. (Pies --> Pigs, Ties, Dies, Lies, Pins, Pits, Etc.)
@Defintion: The Hash Map is 4 Dimension.
@Dim 1 --> A size 26 array, each number being another letter. This key takes the first letter of every word and assigns to the array 
@Dim 2 --> A size 6 array, each number being anotehr vowel. This key takes the first vowel of every word and assigns to the array 
@Dim 3 --> A x sized linked list, each link containing another word. All these words contain the same letter and same first vowel (pair, play, part, etc.)
@Dim 4 --> A y sized linked list, each link contianing another word. All these words are connected by a single letter to the previous dimension word
@Example --> Pies --> Dimension 1 (P) Dimension 2 2 (i) Dimension 3 (Pies) Dimension 4 (Pits-->Dies-->Lies-->Ties-->Pins-->Pigs-->Etc.)*/


/*Allocates the Hash Map
Loops through and allocates pieces of memory for the 2 dimensions of the arrays */
struct wordConnections ***AllocateHashMap(); 

/*Fills the hash map with words
@param HashMap --> The HashMap which it to fill
@param isRandom --> Randomly orders all of the 2nd dimension of linked lists.
cont) Normally it is alphabetical, for example pies --> dies pigs ties, this will randomize that 
if(0) --> it will not be random, if(1) --> it will, in fact, be random
@return --> Returns a 4 Dimension Structure, but it's only necessary to dereference 3 down 1 array 2 array 3 2D Linked List */
char** FillHashMap(struct wordConnections ***HashMap, int isRandom); 


/*Opens the flwd file that has all the connections
@return --> Returns the newly opened file */
FILE *OpenFile();


/*Goes through the hash map, checks if a word is in it, and returns that word
@param word --> The word to be checked and returned
@param HashMap --> The Hash Map
@return --> Returns NULL if the word is not w/in the hash map, otherwise returns the word*/
struct wordConnections* SearchHashMap(struct wordConnections ***HashMap, const char* word);  

/*Print out Hash Map
@param HashMap --> The Hash Map to be printed*/
void PrintHashMap(struct wordConnections ***HashMap);
 

/*Freeing the Hash Map
This part is specifically designed for the Hash Map part, the arrays, within the 2D for loop is a method, (see 2DLinkedList.h)
@param HashMap --> The HashMap to be freed*/
void FreeHashMap(struct wordConnections ***HashMap);

/*Frees all of the words in the program
Word Storage contains all of the words within the hash map (it is big)
@param wordStorage --> the String Array to be freed */
void FreeWordStorage(char** wordStorage); 



#endif
