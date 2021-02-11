#ifndef seenTreeStorage
#define seenTreeStorage

#include "ArrayList.h"
/*Goal: The goal of this whole piece of code, is at this point of the program, it's already gone through and found the word through breadth first search
Now, it's time to show the list of words that demonstrate how they connect. That's the purpose of this
'*/ 

/*The node for tree storage*/
struct TreeStorageNode{
	/*The next location horizontally*/
	struct TreeStorageNode *next; 
	/*The next location from the trees point of view. That is, if (pies) goes to (ties, pits, pigs), they would go to pies*/
	struct TreeStorageNode *prev; 
	/*The Word It Stores*/
	char* word; 
	/*How deep it is in the structure. How many nodes it takes to return to the start*/ 
	int depth; 
	
};

/*This creates the Tree Storage Node, there will be a header
@param word --> the word for the second creation, that is, the one after the header
@return --> returns the header of the Tree Storage Linked List 
*/ 
struct TreeStorageNode *Allocate_TreeStorageNode(char* word); 


/*This adds a word to the tree storage node, and the previous location from the tree set
@param word --> The partiuclar word this node stores
@param prev --> the previous location from the tree set, that connects this word
@param header --> The dummy header node of the Tree Storage Linked List
@return --> Returns the word that it just found*/
struct TreeStorageNode *Add_TreeStorageNode(char* word, struct TreeStorageNode *prev, struct TreeStorageNode *header, int depth); 

/* 
Copies the Word Linked List to the Tree Storage Node. Loops through the word linked list, and adds it to the tree storage linked list 
@param header --> The DHN (dummy head node) of the Tree Storage Linked List 
@param prev --> the previous location in the Tree 
@param list --> the list that will be copied into the Tree Storage List
@param goal --> necessary, because once found, it will immediately return itself  
@return --> Returns itself everytime, so that when the goal is found, it will immediately be able to read through its previous locations in the Pathfind Method*/ 
struct TreeStorageNode *Copy_WordLLToTreeStorageNode(struct TreeStorageNode *header, struct TreeStorageNode *prev, struct word *list, char* goal, int minConnWections);


/*Searches through the tree storage node such that it will be able to connect the previous locations
@param word --> the word that it attempts to find
@param header --> the start of the linked list 
@return --> once it finds its return location, it returns it, and it gets set to the previous location*/ 
struct TreeStorageNode *Search_TreeStorageNode(char* word, struct TreeStorageNode *header); 

/*This searches for the Node just before the Node that it's given the input for, that is, if it's given ties lies dies pies and it's searching for pies, it will output dies
@param word --> the word after the word it's looking for
@param header --> the header of the Tree Storage Linked List
@return --> Returns the node before the node w/ the word it searches for*/ 
struct TreeStorageNode *SearchPrev_TreeStorageNode(char* nextWord, struct TreeStorageNode *header); 

/*Returns the last value of the index
@param header --> the front of the linked list
@return --> Returns the very last value*/
struct TreeStorageNode *ReturnLast_TreeStorageNode(struct TreeStorageNode *header);  

/*When it searches through the previous locations, it prints it out in reverse order, the goal of this is to take the word connections, put
it in the correct order (reverse order to how it is prior this) and also put it in a linked list such that it can be stored & manipulated
@param newList --> the header of the new list that the words will be put into
@param header --> the final word at the very end of list, it starts off at the found word, and runs through the previous word until it hits the dummy head node
@return --> Returns the brand new list of word nodes*/ 
struct word *Convert_TreeStorageNodeToWordLL(struct word *newList, struct TreeStorageNode *header);

/*This Copies all the words on a linked list to a tree storage node so that all it's previous connections can be tracked 
@param currEnd --> The current last node in the tree storage node
@param prev --> The node that all of the newly added words are directly connected to 
@param linkOutput --> The source from which we will grab all our words
@param options --> All of the options that will be considered when finding the output 
@param minConnections --> The absolute minimum number of connections
@return --> Returns the current last node in the tree storage node linked list 
*/
struct TreeStorageNode* Copy_WordLL_Onto_TreeStorageNode_Distance(struct TreeStorageNode *currEnd, struct TreeStorageNode *prev, struct word* linkOutput, struct arrayList *options, int minConnections); 
/*Converts the Tree Storage Node To A 2D Array, goes from the very back to each previous connection
@param arr --> The array whose values will take that of the Tree storage list
@param End --> The very last node in the Tree Storage Array
@param minConnections --> The starting index*/ 
void Convert_TreeStorageNodeTo2DArray(char** arr, struct TreeStorageNode *End, int minConnections);  

/*A method that's not actually used anywhere in the program, it starts at a word, and prints out all of the previous nodes, this was more inteded for testing purposes
@param header --> a word in the list*/
void Print_TreeStorageNode(struct TreeStorageNode *header); 

/*Goes through and frees all of the nodes in the Tree Storage
@param header --> the dummy head node of the Tree Storage Linked List*/ 
void Free_TreeStorageNode(struct TreeStorageNode *header); 

/*Removes a single node/word from the Tree Storage Linked List
@param word --> The word to be removed
@param header --> the header point from which the search begins
@param HashSet --> It is necessary to remove the word from the Hash Set, since it should, no longer be considered as found*/ 
void Remove_TreeStorageNode(char* word, struct TreeStorageNode *header, struct word **(*HashSet)); 

/*The difference between this and free is that it is more thorough in removing it, as it removes all of them past the point, and also gets rid of them from the HashSet
Goes from the point of the word and removes all the words afterwords, as well as removing them from the TreeSet
@param header --> the point from which it will start removing the words in the tree storage node
@param HashSet --> The hash set whose words will soon be removed*/
void RemoveFromPoint_TreeStorageNode(struct TreeStorageNode *header, struct word *QueueHeader, struct word **(*HashSet)); 

#endif
