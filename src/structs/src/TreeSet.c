#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/TreeSet.h"
#include "../includes/HashMap.h"
#include "../includes/ArrayList.h"

#include "../../algs/includes/Minimax.h"

/*This actually adds the node into the tree set
@param data --> The data that is to be added
@param pointer --> The Node That Points to the New Node
@return --> Doesn't return anything*/
struct TreeSetNode* AddNode(void* data, struct TreeSetNode *pointer, int isSmaller); 

struct DummyHeadNode *Allocate_TreeSet(void* data){
	struct DummyHeadNode *header = malloc(sizeof(struct DummyHeadNode)); 

	header->start = malloc(sizeof(struct TreeSetNode));
	header->start->depth = 0; 
	header->start->data = data; 
	header->start->greater = NULL; 
	header->start->smaller = NULL; 
	  
	return header; 
	
} 


struct TreeSetNode* AddNode_TreeSet(void* data, void* pointer, struct TreeSetNode *curNode, enum dataType pointerType, enum dataType valueType, int numLetters){
	//if this is the first node to be added
	if(curNode == NULL){
		/*Create the new node*/ 
		struct TreeSetNode *newNode = malloc(sizeof(struct TreeSetNode));
		newNode->data = data; 
		newNode->depth = 0; 
		newNode->greater = NULL; 
		newNode->smaller = NULL;
		reappoint(pointer, newNode, pointerType, valueType, numLetters); 
	
	
		return newNode; 
	}
	//we have to compare if the node is smaller or greater
	int isSmaller = compare(data, curNode->data, valueType, numLetters);  
	
	//The node to be outputted
	struct TreeSetNode* outputNode = NULL; 
	
	//they are the same
	if(isSmaller == -1){
		//If it already exists and we are adding it, then we should return NULL to say it already exists
		return NULL; 
		
	}
	//if the new node is smaller than what it's compared against
	if(isSmaller){
		//if the smaller is already there
		if(curNode->smaller != NULL){
			//go into the smaller
			outputNode = AddNode_TreeSet(data, curNode, curNode->smaller, SET_NODE, valueType,  numLetters); 
			
		}
		else{
			//Set the new node
			outputNode = AddNode(data, curNode, isSmaller); 
			DetermineDepth(curNode);  
			return outputNode; 
		}
	}
	//if the new node is greater
	else{
		//if greater exists
		if(curNode->greater != NULL){
			//go to the greater and check it
			outputNode = AddNode_TreeSet(data, curNode, curNode->greater, SET_NODE, valueType, numLetters); 
		}
		else{
			//add the new node
			outputNode = AddNode(data, curNode, isSmaller);
			//It determines the depth, not of the node it just sent in, but the node that connected to the node that was just sent in 
		 
			DetermineDepth(curNode); 
			return outputNode;  
		}
		
	}
  
	balance(pointer, curNode, pointerType, valueType, numLetters);   
	DetermineDepth(curNode);  
	return outputNode;
}
//This determines what the depth of a node should be when a node has recently been added 
void DetermineDepth(struct TreeSetNode *curNode){
	if(curNode != NULL){
	
		//printf("Determine Depth Start: Node: %d, D: %d.\n", *(int*)curNode->data, curNode->depth); 
		//printf("\nDetermining..\n"); 
		//First, Determine if it has either a greater or smaller, or perhaps both
		int greater = (curNode->greater != NULL)?1:0; 
		int smaller = (curNode->smaller != NULL)?1:0; 
		//if it only has a greater node, its depth should be one larger than that of its child
		if(greater && !smaller){
			curNode->depth = curNode->greater->depth + 1; 
			//printf("Changing Node: %d. New Depth: %d\n", *(int*)curNode->data, curNode->depth);  
		
		//if it only has a smaller node, its depth should be one larger than that of its child	
		}
		else if(smaller && !greater){
		
			curNode->depth = curNode->smaller->depth + 1; 
			//printf("Changing Node: %d. New Depth: %d\n", *(int*)curNode->data, curNode->depth); 
		}
		//if it has both of its nodes, it gets a bit trickier
		else if(greater && smaller){
			//the depth of its greater child
			int greaterDepth = curNode->greater->depth; 
			//the depth of its smaller child
			int smallerDepth = curNode->smaller->depth; 
			//if it's greater childs depth is larger than that of its smaller child
			if(greaterDepth > smallerDepth){
				//it gets added one plus the greater depth
				curNode->depth = greaterDepth + 1;
				//printf("Changing Node: %d. New Depth: %d\n", *(int*)curNode->data, curNode->depth);  
			}
			//if the smaller depth is greater or equal
			else{
				curNode->depth = smallerDepth + 1; 
				//printf("Changing Node: %d. New Depth: %d\n", *(int*)curNode->data, curNode->depth); 
			}
		
		}
		//if not greater and not smaller
		else{
			curNode->depth = 0;
			//printf("Changing Node: %d. New Depth: %d\n", *(int*)curNode->data, curNode->depth); 
		}
	}//End - If CurNode != NULL
	
}

struct TreeSetNode*  AddNode(void* data, struct TreeSetNode *pointer, int isSmaller){
	/*Create the new node*/ 
	struct TreeSetNode *newNode = malloc(sizeof(struct TreeSetNode));
	newNode->data = data; 
	newNode->depth = 0; 
	newNode->greater = NULL; 
	newNode->smaller = NULL;
	/*Is the New Node greater or smaller*/
	if(isSmaller){
		pointer->smaller = newNode; 
	}  
	else{
		pointer->greater = newNode; 
	}
	
	return newNode; 
}


struct TreeSetNode* Search_TreeSet(void* data, struct TreeSetNode *header, enum dataType type, int numLetters){

	//if the word is smaller then the word it is on, and the smaller/greater position is not null. Then it traverses to that word
	int isEqual = compare(data, header->data, type, numLetters);  
	struct TreeSetNode* nodeOutput = NULL;
	
	//Can't be found
	int outOfBounds; 
	 
	//until it runs into a null value	
	while((outOfBounds = nextIsNull(isEqual, header)) == 0 && isEqual != -1){
		
		//if it's greater
		if(isEqual == 0){
			header = header->greater; 
		}
		//if it's smaller
		else if(isEqual == 1){
			header = header->smaller; 
		}
	
		isEqual = compare(data, header->data, type, numLetters);
		
		
	} 
	if(isEqual == -1){
		nodeOutput = header; 
	}

	return nodeOutput;


}

//Returns if the next is null
int nextIsNull(int isSmaller, struct TreeSetNode *check){
	if(isSmaller == 1){
		//if it's the smaller word, and the next word is null, then return a 1, if it's not, then return 0. 
		return (check->smaller != NULL)?0:1;  
	}
	return (check->greater != NULL)?0:1; 
	
	
	
}

int wordLLCompare(struct word* newHeader, struct word* oldHeader, int numLetters){
	return stringCompare(newHeader->word, oldHeader->word, numLetters); 
}

//Compares two strings
int stringCompare(char* word1, char* word2, int numLetters){
	int i; 
 	
	//Goes through the letters of both words
	for(i = 0; i < numLetters; i++){
		//if they aren't equal
		if(word1[i] != word2[i]){
			//return the biggest one
			return (word1[i] > word2[i])?0:1;  
				 		
		}
			
	}
	//if all the letters are equal, the words are the same
	return -1; 
}


//Compares two integers
int intCompare(int num1, int num2){
	//if the numbers are equal, it returns that
	if(num1 == num2){
		return -1; 
	}
	//otherwise, if the first number is bigger than the second number, it returns 0; if not, it returns 1. 
	return (num1 > num2)?0:1; 
}

void Print_TreeSet(struct TreeSetNode *header, enum dataType type){
	if(header == NULL){
		printf("Empty Tree Set"); 
	}
	else{
		
		if(header->smaller != NULL){
			Print_TreeSet(header->smaller, type); 
			
		} 
		if(type == INTEGER){	
			printf("%d, ", *(int*)header->data); 
		}
		if(type == WORD){
			printf("%s, ", (char*)header->data); 
		}
		if(type == WORDLL){
			printf("%s: ", ((struct word*)(header->data))->word); 
			Print_WordLL(((struct word*)(header->data)), SEPERATED); 
			printf("\n"); 
		}
		if(type == WORD_STRUCT){
			printf("%s ", ((struct wordStruct*)(header->data))->word); 
			
		}
		
		
		if(header->greater != NULL){
			Print_TreeSet(header->greater, type); 
		}
	}
}

/*This is the default compare, depending on if it is an integer or a string will determine whether it should be compared as such*/ 
int compare(void* data1, void* data2, enum dataType type, int numLetters){ 
	if(type == INTEGER){
		/*If it's an integer, do the integer compare
		#Note *(int*)(data) --> (int) just a plain ol' integer, (int*) an integer pointer *(int*) a pointer to an integer pointer'*/ 
		return intCompare(*(int*)(data1), *(int*)(data2));
	}
	else if(type == WORD){
		return stringCompare((char*)data1, (char*)data2, numLetters);  
	}
	else if(type == WORDLL){
		return wordLLCompare((struct word*)data1, (struct word*)data2, numLetters);
	}
	else if(type == WORD4LL){
		return stringCompare((char*)data1, (char*)(((struct word*)(data2))->word), numLetters); 
	}
	else if(type == WORD_STRUCT){
		//Either, it is taking a word, and throwing it in, or it is taking a node
		return stringCompare(((struct wordStruct*)(data1))->word, ((struct wordStruct*)(data2))->word, numLetters); 
	}

	else if(type == WORD_STRUCT_CHECK){

		//Either, it is taking a word, and throwing it in, or it is taking a node
		return stringCompare((char*)data1, ((struct wordStruct*)(data2))->word, numLetters); 
	}
	printf("Incorrect Enum [compare]");
	exit(0); 
}



/*This goes through and balances the tree set node, checking each node along the way.*/
void balance(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	DetermineDepth(header); 
	//maybe a do while, probably will be
	//1) Checks the balance

	 if(header == NULL){
	 	printf("[Balance_TreeSet]: Empty Set"); 
		exit(0); 
	 }

		int offset = checkBalance(header);
		//offset on the left
		if(offset == 1){
			leftOffset(pointer, header, nodeType, valueType, numLetters); 	
		}
		//offset on the right
		if(offset == 2){
			rightOffset(pointer, header, nodeType, valueType, numLetters); 
		}	
	
}

void leftOffset(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	//When we get into left offset, there are a few things we must check 
	//I don't think we even need case III anymore... Don't think we ever did 
	
	if(header->smaller == NULL){
		printf("\nIncorrect Offset Calculation [leftOffset]"); 
		exit(0); 
	}
	
	//Now we need to know which side to balance
	//If balance left is true,  B is to be the new header
	//If balance left is false, C is to be the new header
	//If it's offset on the left the pointer will be facing leftward 
	int balanceLeft = isDeeperLeft(header->smaller, 1); 
	
	//Case 1, see if the smaller greater is not equal to null. 
	if(balanceLeft == 0){
		//If it isn't null, then we want to call the new case I rotate right calculation
		rotateRightCase1(pointer, header, nodeType, valueType, numLetters); 
		
	}
	else{
	//Case 2, see if the smaller smaller is not equal to null
	//else if(header->smaller->smaller != NULL){
		rotateRightCase2(pointer, header, nodeType, valueType, numLetters); 
	}
	//}
	
	DetermineDepth(header); 
	
	

	
}

void rightOffset(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	//So here it is necessary to determine which rotate will be used
	if(header->greater == NULL){
		printf("Incorrect Offset Calculation [rightOffset]");
		exit(0);  
	}
	//Finds which side is deeper. 
	//If it's offset on the right, then it's facing rightwards
	int balanceLeft = isDeeperLeft(header->greater, 0);
	 
	 
	 //If it is deeper on the left then we have to do case 1
	 if(balanceLeft == 1){
	 	rotateLeftCase1(pointer, header, nodeType, valueType, numLetters); 
	 }
	 else{
	 	rotateLeftCase2(pointer, header, nodeType, valueType, numLetters); 
	 }
	 DetermineDepth(header); 

}

//This is, in the left 
int isDeeperLeft(struct TreeSetNode *header, int pointedLeft){
	//Keep in mind, if we need to focus on the right one return 0. 
	//If we need to focus on the left one, return 1

	//Now make sure that the left one isn't null 
	if(header->smaller == NULL){
		return 0; 
		
	}	
	if(header->greater == NULL){
		return 1; 
	}
	//This can only happen in the event of a remove 
	if(header->smaller->depth == header->greater->depth){
		//If it's pointed left, we want the right value 
		if(pointedLeft == 1){
			return 1; 
		}
		//If it's facing from the left, you want a right
		return 1; 
		
	}
	
	//Now if neither are equal to null. We have to base on the depth. 
	return header->smaller->depth > header->greater->depth; 
}





void reappoint(void* pointer, struct TreeSetNode* node, enum dataType nodeType, enum dataType valueType, int numLetters){

	//First, 2 conditions, 
	//1: It is a Dummy Head Node
	if(nodeType == DUMMY){	
		//We just appoint it the new node
		((struct DummyHeadNode*)(pointer))->start = node; 
	}
	
	//2: It is a Tree Set Node
	if(nodeType == SET_NODE){
		struct TreeSetNode *pointerNode = (struct TreeSetNode*)pointer; 
		
		//Not quite as simple
		//We have to compare it
		//Use isSmaller method, if it's 1, it's smaller
		void* pointerValue = pointerNode->data; 
		void* nodeValue = node->data;
		int isSmaller = compare(pointerValue, nodeValue, valueType, numLetters); 
		
		switch(isSmaller){ 
			//Case -1: They are equal, program failed along the way, crash. 
			case(-1):
				printf("Values Are Equal [reappoint]");
				exit(0);  
				break; 
		//Case 1: Set it to the smaller
			case(0):
				pointerNode->smaller = node; 
				break; 
		//Case 2: Set it to the greater
			case(1):
				pointerNode->greater = node; 
				break; 
		//To be safe, determine depth
		
		}
		DetermineDepth(pointerNode); 
	}



}

/*
            H                        H
            |                        |  
            A                        C
          /  \                     /   \
         B    N4                 B       A
       /  \          ===>      /  \    /  \
	 N1    C                 N1   N2  N3  N4
	     /  \
	    N2   N3	  
	 
	 
*/
void rotateRightCase1(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	//First let's set our variables
	//These are the variables we are certain we have
	struct TreeSetNode* A  = header; 
	struct TreeSetNode* B  = header->smaller;
	struct TreeSetNode* C  = header->smaller->greater;
	
	//There are the variables we may or may not have, so we must account for them anyways
	struct TreeSetNode* N1 = B->smaller; //Is the same at the end
	struct TreeSetNode* N2 = C->smaller; //Becomes B->greater
	struct TreeSetNode* N3 = C->greater; //Becomes A->smaller
	struct TreeSetNode* N4 = A->greater; //Is the same at the end
	
	//C is now at the top, although  it is not being pointed to 
	C->greater = A; 
	DetermineDepth(A);  
	DetermineDepth(C);
	
	C->smaller = B; 
	DetermineDepth(B);
	DetermineDepth(C);  
	
	//A is now on the right, being pointed at by C
	A->greater = N4; 
	A->smaller = N3; 
	DetermineDepth(N4); 
	DetermineDepth(N3); 
	DetermineDepth(A); 

	
	
	//B is now on the left, being pointed at by C
	B->greater = N2; 
	B->smaller = N1; 
	DetermineDepth(N1); 
	DetermineDepth(N2); 
	DetermineDepth(B); 
	
	DetermineDepth(C); 
	
	
	 
	//C no longer has any direct affiliation with conditional nodes
	
	//Now I'm going to create a sub method that figures out our pointer
	reappoint(pointer, C, nodeType, valueType, numLetters); 
}


/*
		     A                       B
	       /  \                    /  \
	      B    N4                C     A
	    /  \           ==>     /  \   / \
 	   C   N3                N1   N2 N3  N4
	 /  \
   N1    N2
*/
void rotateRightCase2(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	
	//These are all the certain nodes
	struct TreeSetNode* A  = header; 
	struct TreeSetNode* B  = header->smaller;
	struct TreeSetNode* C  = header->smaller->smaller;

	//These are all the nodes that may be null 
	struct TreeSetNode* N1 = C->smaller;
	struct TreeSetNode* N2 = C->greater;
	struct TreeSetNode* N3 = B->greater;
	struct TreeSetNode* N4 = A->greater;
	
	//First, set B to be at the top (disregarding the pointer)
	B->smaller = C; 
	DetermineDepth(C); 
	DetermineDepth(B); 
	
	B->greater = A; 
	DetermineDepth(B); 
	DetermineDepth(A); 
	
	//Set C's nodes 
	C->smaller = N1; 
	C->greater = N2; 
	DetermineDepth(N1); 
	DetermineDepth(N2); 
	DetermineDepth(C); 
	
	A->smaller = N3; 
	A->greater = N4; 
	DetermineDepth(N3); 
	DetermineDepth(N4); 
	DetermineDepth(A); 
	
	DetermineDepth(B); 
	
	reappoint(pointer, B, nodeType, valueType, numLetters); 
		
}


/*
        A                           C
      /  \                       /    \
    N1    B                     A       B
         / \       ==>        /  \    /   \
        C   N4              N1  N2   N3   N4
      /  \
    N2   N3
*/
void rotateLeftCase1(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	//First, Define the Certain Variables
	struct TreeSetNode* A  = header;
	struct TreeSetNode* B  = header->greater;
	struct TreeSetNode* C  = header->greater->smaller;
	
	//The leaf nodes that may be null
	struct TreeSetNode* N1 = A->smaller;
	struct TreeSetNode* N2 = C->smaller;
	struct TreeSetNode* N3 = C->greater;
	struct TreeSetNode* N4 = B->greater;
	
	//Then set the nodes that will not be null
	C->smaller = A; 
	DetermineDepth(A); 
	DetermineDepth(C); 
	
		
	C->greater = B; 
	DetermineDepth(B); 
	DetermineDepth(C); 
	
	//Then, set the nodes that may be null
	A->smaller = N1; 
	DetermineDepth(N1); 
	
	A->greater = N2; 
	DetermineDepth(N2);
	 
	DetermineDepth(A); 
	DetermineDepth(C); 
	
	B->smaller = N3; 
	DetermineDepth(N3); 
	
	B->greater = N4; 
	DetermineDepth(N4);
	 
	DetermineDepth(B); 
	DetermineDepth(C); 	
	
	//Change the pointer such that now it points at C
	reappoint(pointer, C, nodeType, valueType, numLetters); 
}




/*
    A                          B
   / \                      /    \
 N1    B                  A        C
     /  \        ==>    /  \     /  \
   N2    C            N1   N2   N3  N4
        / \ 
      N3   N4


*/
void rotateLeftCase2(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType, int numLetters){
	//First, set the certain nodes
	struct TreeSetNode* A  = header; 
	struct TreeSetNode* B  = header->greater;
	struct TreeSetNode* C  = header->greater->greater; 
	
	//Then, set the uncertain nodes (could be null)
	struct TreeSetNode* N1 = A->smaller; 
	struct TreeSetNode* N2 = B->smaller; 
	struct TreeSetNode* N3 = C->smaller; 
	struct TreeSetNode* N4 = C->greater; 
	
	//Get B at the top 
	B->greater = C; 
	DetermineDepth(B); 
	DetermineDepth(C); 
	
	//Get A to the left
	B->smaller = A; 
	DetermineDepth(A); 
	DetermineDepth(B); 
	
	//Get A to point at potential nodes
	A->smaller = N1; 
	A->greater = N2; 
	DetermineDepth(N1); 
	DetermineDepth(N2); 
	DetermineDepth(A); 
	
	//Get C to point at potential nodes 
	C->smaller = N3; 
	C->greater = N4; 
	DetermineDepth(N3); 
	DetermineDepth(N4); 
	DetermineDepth(C); 
	
	//Determine B's Depth
	DetermineDepth(B); 
	
	//Makes the pointer point at B 
	reappoint(pointer, B, nodeType, valueType, numLetters); 
	
}




int checkBalance(struct TreeSetNode *header){
	//Too long on the right (greater): 
		//Return 2
	//Too long on the left (smaller):
		//Return 1
	//They are Equal:
		//Return 0
	//Defining "Too Long": If one of direct child's node's depth is greater than the other child's depth by 2, then it is too long
	//If the greater node is null, set it to -1, otherwise, set it to the greater's depth
	int greater = (header->greater == NULL)?-1:header->greater->depth;
	int smaller = (header->smaller == NULL)?-1:header->smaller->depth; 
	 
	if(header == NULL){
		printf("Header Null [checkBalance]"); 
		exit(0); 
	}
	//if greater is larger than smaller, it is too long on the right
	if(greater - smaller >= 2){
		return 2; 
	}
	else if(smaller - greater >= 2){
		return 1; 
	}
	//if they are equal or only subtract to 1, return 0
	return 0; 
	
}


int Remove_TreeSet(void* data, void* pointer, struct TreeSetNode* curNode, enum dataType pointerType, enum dataType valueType, int numLetters){
	int found = 0;
	if(curNode == NULL){
		return -1; 
	}
	int isSmaller = compare(data, curNode->data, valueType, numLetters); 
	//Compares to see if they're the same, only cares if they're the same
	if(isSmaller == -1){
		Removal(pointer, curNode, pointerType, valueType, numLetters); 
		//Alright, we've removed the node

		return 1; 
	}
	//if they are not the same
	else{
		//if the node to be removed is smaller than the previous one, it'll go into the smaller value
		if(isSmaller){
			found = Remove_TreeSet(data, curNode, curNode->smaller, SET_NODE, valueType, numLetters);
		
		}

		//if the node to be removed is greater than the previous one, it'll go into the greater value
		else{
			found = Remove_TreeSet(data, curNode, curNode->greater, SET_NODE, valueType, numLetters); 
			
		
		}
			
	
		
		
	}

	if(found == 1){
	
		balance(pointer, curNode, pointerType, valueType, numLetters); 
		DetermineDepth(curNode); 
	}
	
	return found; 

}


/*This actually goes ahead and removes it once it has been found*/  
void Removal(void* pointer, struct TreeSetNode* header, enum dataType pointerType, enum dataType valueType, int numLetters){
	//Now let's get the pointer defined
	int isDummyPointer = (pointerType == DUMMY)?1:0;
	//Is it the new node greater than or less than the pointer
	int isGreater = 0;
	//if the greater is connected 
	int greater = (header->greater != NULL)?1:0;
	//if the smaller is connected
	int smaller = (header->smaller != NULL)?1:0;
	//check if this node's value is greater than that of the header's
	if(isDummyPointer == 0){
		isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType, numLetters);
		
	}
	
	//if it is not connected to anything

	if(!greater && !smaller){
		//Free the header
		free(header); 
		//first you want to free the header
		//What to do if the pointer is only a dummy pointer
		if(isDummyPointer == 1){
			((struct DummyHeadNode*)(pointer))->start = NULL; 
		}
		//What to do if the pointer is a Tree Set Node
		else{
			
			//if the header is greater than the pointer
			if(isGreater == 1){
				((struct TreeSetNode*)(pointer))->greater = NULL; 
			}
			else{
				((struct TreeSetNode*)(pointer))->smaller = NULL;
			}
			
		}
		
	}

	//if it only connects to the greater one
	else if(greater ^ smaller){
		struct TreeSetNode *connection = (greater)?header->greater:header->smaller;  
		Remove_OneNodeAttatchment(pointer, connection, isGreater, isDummyPointer); 
		//Then, free the header
		free(header); 
	}

	//if it connects to both smaller && greater, then it's a bit trickier
	else{
		
		header->data = Remove_TwoNodeAttatchment(header, (isGreater)?header->greater:header->smaller, isGreater, pointerType, valueType, numLetters);  
		balance(pointer, header, pointerType, valueType, numLetters); 
		DetermineDepth(header); 
 
	}
	
}

//Removal for one node attatchment
void Remove_OneNodeAttatchment(void* pointer, struct TreeSetNode *connection, int isGreater, int isDummyPointer) { 
	//Then connect the pointer to the temp
	if(isDummyPointer == 1){
		
		((struct DummyHeadNode *)(pointer))->start = connection;
	}
	else{
		
		//if the header is greater than the pointer
		if(isGreater == 1){
			((struct TreeSetNode*)(pointer))->greater = connection; 
		}
		else{
			((struct TreeSetNode*)(pointer))->smaller = connection;
		}
	}
}


void* Remove_TwoNodeAttatchment(void* pointer, struct TreeSetNode *curNode, int isGreater, enum dataType pointerType, enum dataType valueType, int numLetters){
	//Keeps the data such that when it is removed, it can still be stored & returned
	void* temp; 
	void* output = NULL; 
	//So this has to start off by being given a pointer and a curNode and an isGreater
	if(isGreater){
		if(curNode->smaller == NULL){
			temp = curNode->data; 
			Removal(pointer, curNode, SET_NODE, valueType, numLetters); 
			return temp; 
		}
		else{
			output = Remove_TwoNodeAttatchment(curNode, curNode->smaller, isGreater, pointerType, valueType, numLetters); 
		}
	}
	//If it is smaller
	else{
		if(curNode->greater == NULL){
			temp = curNode->data; 
			Removal(pointer, curNode, SET_NODE, valueType, numLetters);
			return temp; 
		}
		else{
			output = Remove_TwoNodeAttatchment(curNode, curNode->greater, isGreater, pointerType, valueType, numLetters); 
		}
	}
	
	DetermineDepth(curNode); 

	return output; 
	//There will be two cases, one if it is greater and another if it isn't
	//Base Case: Smaller if smaller is null
	//Greater: If Greater Is Null
	//It must return the value that it finds	
}

/*Goes through and frees the tree set*/ 
void Free_TreeSet(struct TreeSetNode *header, enum dataType type){

	if(header != NULL){
	
		/*Checks if the greater location exists, if it does, it goes into it and repeats this*/ 
		if(header->greater != NULL){ 
			Free_TreeSet(header->greater, type); 

		}
		/*Checks if the smaller location exists, if it does, it goes into it and checks it's greater and smaller'*/ 
		if(header->smaller != NULL){
			Free_TreeSet(header->smaller, type); 
	 
		}
		//Now that both the greater and smaller were null, or have been freed, it frees the header
		if(type == WORDLL){
			//Frees the linked list 
			Free_WordLL(header->data); 
		}
		if(type == WORD_STRUCT){
			Free_WordStruct(header->data); 
		}
	
		free(header); 
	}
	
}

void preorder_TreeSet(struct TreeSetNode *header){

	if(header!=NULL)
	{
		printf("%d(Depth: %d)\n", *(int*)header->data, header->depth);
		preorder_TreeSet(header->smaller);
		preorder_TreeSet(header->greater);
	}

}

void postorder_TreeSet(struct TreeSetNode *header){
	if(header!=NULL)
	{
		preorder_TreeSet(header->smaller);
		printf("%d(Depth: %d)\n", *(int*)header->data, header->depth);
		preorder_TreeSet(header->greater);
	}
	
} 

struct DummyHeadNode* ConvertArrayToTree(int dim1Size, void** arr, enum dataType type, int numLetters){
	//Allocate the tree
	struct DummyHeadNode *tree = Allocate_TreeSet(arr); 
	int i;
	int* a = (int*)arr;   
	//Loop until the one before the last one, starting at 1
	for(i = 1; i < dim1Size - 1; i++){
		void* data = (void*)&a[i];		
		AddNode_TreeSet(data, tree, tree->start, DUMMY, type, numLetters); 
		
	} 
	void* data = (void*)&a[i];		
	AddNode_TreeSet(data, tree, tree->start, DUMMY, type, numLetters); 
	return tree; 
}

void BigAVLTest(int numLetters){

int rr[] = {1, 2, 3}; 
	AVLTestQuick(rr, 3, numLetters); 
	
	int rl[] = {1, 3, 2}; 
	AVLTestQuick(rl, 3, numLetters); 
	
	int rll[] = {50, 25, 75, 60, 100, 55}; 
	AVLTestQuick(rll, 6, numLetters); 
	
	int rlr[] = {50, 25, 75, 60, 100, 65}; 
	AVLTestQuick(rlr, 6, numLetters); 
	
	int rrl[] = {50, 25, 75, 60, 100, 80}; 
	AVLTestQuick(rrl, 6, numLetters); 	
	
	int rrr[] = {50, 25, 75, 60, 100, 115};
	AVLTestQuick(rrr, 6, numLetters);
	
	int rrrr[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 145}; 
	AVLTestQuick(rrrr, 12, numLetters); 
	
	int rrrl[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 130}; 
	AVLTestQuick(rrrl, 12, numLetters); 
	
	int rrlr[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 115}; 
	AVLTestQuick(rrlr, 12, numLetters); 
	
	int rrll[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 105}; 
	AVLTestQuick(rrll, 12, numLetters); 	
	
	int rlrr[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 85}; 
	AVLTestQuick(rlrr, 12, numLetters); 	 
	
	int rlrl[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 78}; 
	AVLTestQuick(rlrl, 12, numLetters); 
	
	int rllr[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 70}; 
	AVLTestQuick(rllr, 12, numLetters); 
	
	int rlll[] = {50, 0, 100, -25, 25, 75, 125, 60, 80, 110, 140, 55};
	AVLTestQuick(rlll, 12, numLetters);	 	 
	
	int ll[] = {3, 2, 1};
	AVLTestQuick(ll, 3, numLetters); 
	
	int lr[] = {3, 1, 2}; 
	AVLTestQuick(lr, 3, numLetters); 

	int lll[] = {4, 2, 5, 1, 3, 0}; 
	AVLTestQuick(lll, 6, numLetters); 
	
	int llr[] = {4, 2, 5, 0, 3, 1}; 
	AVLTestQuick(llr, 6, numLetters); 
		
	int lrl[] = {4, 1, 5, 0, 3, 2}; 
	AVLTestQuick(lrl, 6, numLetters);
	
	int lrr[] = {4, 2, 5, 1, 3, 0}; 
	AVLTestQuick(lrr, 6, numLetters);
	
	int llll[] = {50, 40, 75, 20, 45, 60, 100, 10, 30, 42, 47, 5}; 
	AVLTestQuick(llll, 12, numLetters); 
	
	int lllr[] = {50, 40, 75, 20, 45, 60, 100, 10, 30, 42, 47, 15}; 
	AVLTestQuick(lllr, 12, numLetters); 
	
	int llrl[] = {50, 40, 75, 20, 45, 60, 100, 10, 30, 42, 47, 25}; 
	AVLTestQuick(llrl, 12, numLetters); 
	
	int llrr[] = {50, 40, 75, 20, 45, 60, 100, 10, 30, 42, 47, 35}; 
	AVLTestQuick(llrr, 12, numLetters); 
}


void AVLTestQuick(int* a, int length, int numLetters){

	struct DummyHeadNode *tree = ConvertArrayToTree(length, (void**)a, INTEGER, numLetters);
	Print_TreeSet(tree->start, INTEGER); 
	printf("\n"); 
	preorder_TreeSet(tree->start); 
	Free_TreeSet(tree->start, INTEGER);
	free(tree); 	
	
	
	
}
//This is the lobby where all the permutations will be started
void arrayPermutations_Lobby(int numLetters){
	int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int length = 10;  

	permute(a, 0, length - 1, numLetters); 

	
	//In order to get the permutations, I need to: 	
	//Have a 2D for loop 
	
	//The first dimension is going to be j, which moves very slowly, and only goes back after all permutations have been done 
	//The second dimension is going to pivot on i. This will move very quickly, each time. And will run factorial(j) times
	//Then, each time, i will go up we're going to have to modulo it by j
	//First, let's set the max variables
	//Also I'm going to need a switch elements variable
}

//So, we're going to need to have a for loop: 
//We're also going to need to have a recursion

void permute(int *arr, int l, int r, int numLetters) { 
	
	int i; 
	if (l == r) {
	   	
	   	struct DummyHeadNode *t = ConvertArrayToTree(r + 1, (void**)arr, INTEGER, numLetters); 
	   	int i = 5; 
	   	int *p = &i; 
	   	Remove_TreeSet((void*)p, t, t->start, DUMMY, INTEGER, numLetters); 
		i = 0; 
		Remove_TreeSet((void*)p, t, t->start, DUMMY, INTEGER, numLetters); 
		i = 3; 
		Remove_TreeSet((void*)p, t, t->start, DUMMY, INTEGER, numLetters); 
		i = 7; 
		Remove_TreeSet((void*)p, t, t->start, DUMMY, INTEGER, numLetters); 
		i = 8; 
		Remove_TreeSet((void*)p, t, t->start, DUMMY, INTEGER, numLetters); 
		Print_TreeSet(t->start, INTEGER);
		printf("\n");  
	}
	
	else
	{ 
	    for (i = l; i <= r; i++) 
	    { 
	    
	        swap(arr,l,i); 
	        permute(arr, l+1, r, numLetters); 
	        swap(arr,l,i); //backtrack 
	    } 
	} 
	
	
}

void swap(int* arr, int i, int j){
	int temp = arr[i]; 
	arr[i] = arr[j];
	arr[j] = temp;    
}
