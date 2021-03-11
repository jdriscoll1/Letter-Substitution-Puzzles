#ifndef seenTree
#define seenTree

#include "WordLinkedList.h"
/*This is the tree set Node
The primary use of the tree set node is to store the words that have already been looked through while pathfinding, to avoid it running indefintely
How it works --> It starts off with a word, for example, pies. Then when a word is added, depending on if it is smaller or bigger, it will
Sort it as such. Then, if the next word is smaller too, then it will go into the smaller word, and determine if it is smaller or bigger than that word 

When calling header, always use header->start because the dummy head node is a different type of node 
*/ 

enum dataType{
	INTEGER, WORD, DUMMY, SET_NODE, GENERIC_NODE, WORDLL
};
/*The Structure of the Tree Set*/ 
struct TreeSetNode {
	/*The word it stores*/ 
	void* data; 
	//How deep the word is 
	int depth; 
	/*The next location if the word is greater (a = 0, z = 25)*/ 
	struct TreeSetNode *greater; 
	/*The next location if the word is smaller*/ 
	struct TreeSetNode *smaller;

};

/*The dummy head node*/ 
struct DummyHeadNode {
	/*The start of the tree set*/ 
	struct TreeSetNode *start; 
};

/*Allocates the Hash Map
@param word --> The first word to be placed, the starting word 
@return --> Returns a a dummy head node that links to the beginning of the Tree Set 
*/
struct DummyHeadNode *Allocate_TreeSet(void* data); 


/*This actually goes through recursively and goes until it finds the word
@param data --> The information to be added
@param pointer --> This is what points to the current node, could either be a dummy header node, or a tree set node
@param curNode --> the current node that the person is on
@param valueType --> the type of data, Word? Int? Char?
@param pointerType --> The type of pointer that points to the current node
@return --> Returns the added node*/ 
struct TreeSetNode*  AddNode_TreeSet(void* data, void* pointer, struct TreeSetNode *curNode, enum dataType pointerType, enum dataType valueType); 

/*This determines the depth of a node when a node has just been added
@param curNode --> the nodes whose depth will be determined*/ 
void DetermineDepth(struct TreeSetNode *curNode);

/*Searches the tree set, always use header->start 
@param word --> the word it searches for 
@param header --> the first word in the Tree Set (the top) always use DummyHeaderNode->start
@return 1 if the word is found, 0 if it isn't*/ 
int Search_TreeSet(void* data, struct TreeSetNode *header, enum dataType type);  

/*Checks if the next location is null
@param isSmaller --> 1: Checks the smaller node 0: checks the bigger node
@param check --> the location that it checks if it is null
@return --> returns 1 if it is null, 0 if it is not null*/ 
int nextIsNull(int isSmaller, struct TreeSetNode *check); 

/*Prints out a tree set*/
void Print_TreeSet(struct TreeSetNode *header, enum dataType type);  

/*Compares a word to a header of a link list
@param word --> The word being compared
@param header --> The header of the word linked list whose nodes will be compared*/
int wordLLCompare(struct word* newHeader, struct word* oldHeader); 
/*Compares two words to see which is larger
@param word1 --> the first word, contextually it is not part of the tree set
@param word2 --> the second word, contextuallly it is part of the tree set
@return -1: if they are equal; 0: if the word1 is greater than word2; 1: if word1 is smaller than word2*/ 
int stringCompare(char* word1, char* word2); 

/*Compares Two Integers to See Which is Larger
@param num1 --> the first word, contextually it is not part of the tree set
@param num2 --> the second word, contextually, it is part of the tree set
@return --> (Equal: -1); (Num1 > Num2: 0); (Num2 > Num1: 1)*/ 
int intCompare(int num1, int num2); 

/*This goes through and takes 2 pieces of data and compares them
@param data1 --> the first piece of data
@param data2 --> the second piece of data
@return (look above)*/ 
int compare(void* data1, void* data2, enum dataType type); 


/*This goes through and balances the tree set node, checking each node along the way.
@param pointer --> the pointer the points to the set of nodes, most likely the previous value, so it'll have to store itself
@param header --> the header that the pointer points to
@param nodeType --> the type of the header, is it a dummy node? or does it lie within the tree set?
@param type --> the data type*/
void balance(void* pointer, struct TreeSetNode* header, enum dataType nodeType, enum dataType valueType); 

/*Checks if a Tree Set is balanced on both sides
@param header --> the header, will not be included in the counting. Is void in case it needs to be a dummy head node
@return 0 -- if it is balanced. 1 -- If it is long on the left. 2-- if it is too long on the right.*/
int checkBalance(struct TreeSetNode *header);

/*This method takes 1 node, and figures out which side is deeper
@param header--> The method whose values will be checked
@param pointerRight --> This checks whether how it is linked to pointer, necessary in the event depth is equal  
  O                       O
   \   ---> 1            /    ---> 0
    O                   O
    

*/
int isDeeperLeft(struct TreeSetNode *header, int pointedLeft); 

/*If it is offset on the left
@param pointer --> The header being pointing at the list 
@param header --> The header who may be offset on the left
@param valueType --> The dataType that may be offset
@param nodeType --> Is the header a DummyHeader Node, or instead a Tree Set Node*/
void leftOffset(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 

/*If it is offset on the right
@param pointer --> The header being pointing at the list
@param header --> The header who will get rotated
@param valueType --> The dataType (Integer)
@param nodeType --> Is the header a DummyHeader Node, or instead  */
void rightOffset(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 



/*Rotates a set of nodes rightwards
@param pointer --> the pointer that will soon be appointed to a new node*/
void rotateRight(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType type); 

/*
     c
    /
   b
  /
 a 
*/
void rotateRightCase1(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 

/*
     c
    /
   a
   \
    b 
*/
void rotateRightCase2(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 

/*
     b
    / \
   a   c

*/
void rotateRightCase3(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 

/*Rotates a set of nodes leftwards
@param pointer --> the pointer that will soon be appointed to a new node*/
void rotateLeft(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType type); 


/*
      a
       \
        c
       /
      b
*/
void rotateLeftCase1(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 

/*  a
     \ 
      b
       \
        c
*/
void rotateLeftCase2(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType); 
/*
	b
   / \
  a   c
*/
void rotateLeftCase3(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType);


/*This is the method where the pointer figures out exactly what it is pointing at
@param pointer --> The pointer who will point at a new node
@param node --> The node at which the pointer will point
@param nodeType --> What is the node?
@param valueType --> The is the type of value?*/
void reappoint(void* pointer, struct TreeSetNode* node, enum dataType nodeType, enum dataType valueType); 

/*Finds the word that is to be removed from the tree set, and removes it
@param data --> the data to be removed
@param pointer --> what points to the data being removed (considering this will likely be recursive)
@param header --> the top of the tree set from which the search will begin
@param type --> the type of data that is about to be removed
@return --> returns an integer that declares if it has or has not yet been found, so that it will stop*/
int Remove_TreeSet(void* data, void* pointer, struct TreeSetNode* header, enum dataType pointerType, enum dataType valueType); 
 
/*This actually does the removing
@param pointer --> The pointer that connects to the node
@param header --> The node that is going to be removed
@param pointerType --> is the pointer a dummy head node, or a tree set node
@param valueType --> the type of the data, string? char? int?
@return --> needs to return depending on where it */ 
void Removal(void* pointer, struct TreeSetNode* header, enum dataType pointerType, enum dataType valueType); 

/*This is the remove method just in case the node only connects to 1 node
@param isGreater --> is the node greater than or less than the pointer. 1 == the node is greater than the pointer. 0 the node is less than the pointer
@param isDummyPointer --> is the pointer a dummy head pointer? No? It instead must be a tree set pointer
@param connection --> This was connected to the pointer before he "passed on" ;(  
@param pointer --> the pointer that connects the node to be removed*/ 
void Remove_OneNodeAttatchment(void* pointer, struct TreeSetNode *connection, int isGreater, int isDummyPointer); 

/*This is the remove method just in case the node connects to 2 nodes
@param pointer --> the pointer that points to curNode
@param curNode --> The node whose values are being examined
@param isGreater --> This asks the question is the node greater than, or less than its pointer?
@param pointerType --> Is the original pointer a dummy head node, or set_node? something else?
@param valueType --> This is the data type of the data. For example, an integer or a string*/
void* Remove_TwoNodeAttatchment(void* pointer, struct TreeSetNode *curNode, int isGreater, enum dataType pointerType, enum dataType valueType); 
/*Recursively frees the tree set, afterwards, free the dummy head node
@param header --> the head of the tree node
Don't forget to free the tree head
Should look like: FreeTreeSet(TreeHead->start); free(TreeHead); */ 
void Free_TreeSet(struct TreeSetNode *header, enum dataType type);

void preorder_TreeSet(struct TreeSetNode *header); 

void postorder_TreeSet(struct TreeSetNode *header);

/*Takes an array and converts it into a tree set
@param dim1Size --> The length of the array
@param arr --> The array being inserted
@time --> O(n)
@return --> Returns a tree set with nothing but the values in the array*/
struct DummyHeadNode* ConvertArrayToTree(int dim1Size, void** arr, enum dataType data);  

/*This is a huge AVL Tree Test*/	
void BigAVLTest(); 

/*This takes an array, converts it into an AVL tree, and outputs the preorder for testing purposes
@param a --> The array to be converted
@param length --> The length of that array */
void AVLTestQuick(int* a, int length); 	
	
	
void arrayPermutations_Lobby(); 
void permute(int *arr, int l, int r); 
void swap(int* arr, int i, int j);  	

#endif
