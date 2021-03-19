#ifndef seenHashSet
#define seenHashSet

/*File --> HashSet.h
Goal --> Hopefully a more efficent way of tracking the words already used --> Certainly for removing words than the tree set

How to Instantiate It: 
1) Create the 2D Array --> Allocate Hash Map
2) Fill it as the program runs
3) Free it 
*/

/*Necessary Methods*/
/*Allocate Hash Set*/
struct word ***AllocateHashSet();    
/*Adds to the front of the Hash Set for 2 reason
1) Less loopage while being added
2) It always will remove the more recently added words, therefore, less loopage in that respect too 
*/ 
void AddToHashSet(char* word, struct word **(*HashSet), int dataMalloc);

/*Searches through a hash set to see if a word is on it
@param word --> the word it's searching for
@param HashSet --> The hash set it's searching through
@return --> Retuns 1 if it's found, returns 0 if it's not found*/ 
int Search_HashSet(char* word, struct word **(*HashSet));  
/*Remove From Hash Set*/
int Remove_HashSet(char* word, struct word **(*HashSet)); 
/*Print HashSet*/
void Print_HashSet(struct word **(*HashSet)); 
/*Free Hash Set */  
void Free_HashSet(struct word **(*HashSet));  

#endif
