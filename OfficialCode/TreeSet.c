#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TreeSet.h"
extern int numLetters; 

/*This actually adds the node into the tree set
@param data --> The data that is to be added
@param pointer --> The Node That Points to the New Node
@return --> Doesn't return anything*/
static void AddNode(void* data, struct TreeSetNode *pointer, int isSmaller); 

struct DummyHeadNode *Allocate_TreeSet(void* data){
	struct DummyHeadNode *header = malloc(sizeof(struct DummyHeadNode)); 

	header->start = malloc(sizeof(struct TreeSetNode));
	header->start->depth = 0; 
	header->start->data = data; 
	header->start->greater = NULL; 
	header->start->smaller = NULL; 
	  
	return header; 
	
} 


int AddNode_TreeSet(void* data, void* pointer, struct TreeSetNode *curNode, enum dataType pointerType, enum dataType valueType){
	//we have to compare if the node is smaller or greater
	int isSmaller = compare(data, curNode->data, valueType); 
	//they are the same
	if(isSmaller == -1){
		return -1; 
		
	}
	//if the new node is smaller than what it's compared against
	if(isSmaller){
		//if the smaller is already there
		if(curNode->smaller != NULL){
			//go into the smaller
			AddNode_TreeSet(data, curNode, curNode->smaller, SET_NODE, valueType); 
			
		}
		else{
			//Set the new node
			AddNode(data, curNode, isSmaller); 
			DetermineDepth(curNode);  
			return 1; 
		}
	}
	//if the new node is greater
	else{
		//if greater exists
		if(curNode->greater != NULL){
			//go to the greater and check it
			AddNode_TreeSet(data, curNode, curNode->greater, SET_NODE, valueType); 
		}
		else{
			//add the new node
			AddNode(data, curNode, isSmaller);
			DetermineDepth(curNode); 
			return 1;  
		}
		
	}
	balance(pointer, curNode, pointerType, valueType); 
	DetermineDepth(curNode);  
	
}
//This determines what the depth of a node should be when a node has recently been added 
void DetermineDepth(struct TreeSetNode *curNode){
	printf("\nDetermining..\n"); 
	//First, Determine if it has either a greater or smaller, or perhaps both
	int greater = (curNode->greater != NULL)?1:0; 
	int smaller = (curNode->smaller != NULL)?1:0; 
	//if it only has a greater node, its depth should be one larger than that of its child
	if(greater && !smaller){
		curNode->depth = curNode->greater->depth + 1; 
	//if it only has a smaller node, its depth should be one larger than that of its child	
	}
	else if(smaller && !greater){
		curNode->depth = curNode->smaller->depth + 1; 
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
		}
		//if the smaller depth is greater or equal
		else{
			curNode->depth = smallerDepth + 1; 
		}
	
	}
	//if not greater and not smaller
	else{
		curNode->depth = 0; 
	}
	
}

static void AddNode(void* data, struct TreeSetNode *pointer, int isSmaller){
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
	
}


int Search_TreeSet(void* data, struct TreeSetNode *header, enum dataType type){
	//if the word is smaller then the word it is on, and the smaller/greater position is not null. Then it traverses to that word
	int isEqual = compare(data, header->data, type);  
	//until it runs into a null value
	while(nextIsNull(isEqual, header) == 0 && isEqual != -1){
		
		//if it's greater
		if(isEqual == 0){
			header = header->greater; 
		}
		//if it's smaller
		else if(isEqual == 1){
			header = header->smaller; 
		}
	
		isEqual = compare(data, header->data, type);
		
	} 
	
	return (isEqual == -1)?1:0;


}

//Returns if the next is null
int nextIsNull(int isSmaller, struct TreeSetNode *check){
	if(isSmaller == 1){
		//if it's the smaller word, and the next word is null, then return a 1, if it's not, then return 0. 
		return (check->smaller != NULL)?0:1;  
	}
	return (check->greater != NULL)?0:1; 
	
	
	
}
//Compares two strings
int stringCompare(char* word1, char* word2){
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
		printf("\n[Print_TreeSet]: Empty List\n"); 
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
		if(header->greater != NULL){
			Print_TreeSet(header->greater, type); 
		}
	}
}

/*This is the default compare, depending on if it is an integer or a string will determine whether it should be compared as such*/ 
int compare(void* data1, void* data2, enum dataType type){ 
	if(type == INTEGER){
		/*If it's an integer, do the integer compare
		#Note *(int*)(data) --> (int) just a plain ol' integer, (int*) an integer pointer *(int*) a pointer to an integer pointer'*/ 
		return intCompare(*(int*)(data1), *(int*)(data2));
	}
	int compare = stringCompare((char*)data1, (char*)data2);  
	return compare; 
}



/*This goes through and balances the tree set node, checking each node along the way.*/
void balance(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType){
	//maybe a do while, probably will be
	//1) Checks the balance
	
	 if(header == NULL){
	 	printf("[Balance_TreeSet]: Empty Set"); 
		exit(0); 
	 }
	int offset;
	//do{
		offset = checkBalance(header);
		//offset on the left
		if(offset == 1){
			if(header->smaller->greater != NULL){
				printf("\n"); 
				if( (header->smaller->greater->greater != NULL) ||
					(header->smaller->greater->smaller != NULL) ){
					//printf("\n[Double Rotate Right] "); 
					rotateLeft(header, header->smaller, SET_NODE, valueType); 
					header->smaller->smaller->depth--; 
					DetermineDepth(header->smaller); 
					//printf("  [Rotate Right]"); 
				}											
			}
				//So, if it is offset on the left, it is necessary to check if it needs to be a double rotation upon
				rotateRight(pointer, header, nodeType, valueType); 
			
		}
		//offset on the right
		if(offset == 2){
			if(header->greater->smaller != NULL){
		    	//printf("\n"); 
				//Let's see if it needs a double rotation
				if( (header->greater->smaller->greater != NULL) ||
			    	(header->greater->smaller->smaller != NULL) ){
			    	//printf("\n[Double Rotate Left]"); 
			 		rotateRight(header, header->greater, SET_NODE, valueType);
			 		header->greater->greater->depth--;
					DetermineDepth(header->greater); 
					//printf("  [Rotate Left]"); 
					
			 	
			   	
			   }
			}
			//If it's offset on the right, it is again, necessary to check if it needs to be rotated upon
			rotateLeft(pointer, header, nodeType, valueType);
			//printf("  [Double Rotate Left]"); 
			
		}	
	
}

void rotateRight(void *pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType){
	 
	//If the left is not being pointed at 
	if(header->smaller == NULL){
		//printf("\n[Rotate Right] --> No node on which to pivot");
		exit(0);  
	}
		/*3
	/
   2
  /
 1  */ 
 	//CASE 2
	if(header->smaller->smaller != NULL){
		//first create a temp to 2's greater
		struct TreeSetNode *temp = header->smaller->greater;
		//next lock 2 greater into 3
		header->smaller->greater = header; 
		//second, connect the header to 2
		//if the pointer is of the dummy head node type
		if(nodeType == DUMMY){
			((struct DummyHeadNode*)(pointer))->start = header->smaller;
		}
		//if the pointer is the tree set type
		else{
			//we must figure out if it's pointing using it's smaller or greater node
			//determines if the header is greater than the pointer
			int isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType); 
			//if they're equal, there's an issue with the tree
			if(isGreater == -1){
				printf("Values Are Equal [Rotate Right]"); 
				exit(0);
			}
		
			//If the pointer is greater than the header. Ex: pointer: 5 header: 3. That is, the pointer points to the smaller node
			if(isGreater == 0){
				((struct TreeSetNode*)(pointer))->smaller = header->smaller;
			}
			//If the header is greater than the pointer. That is the pointer points to the greater node
			else{
				((struct TreeSetNode*)(pointer))->greater = header->smaller;
			}
			
		}
		//if the pointer is of the tree set node type 
		//finally, unlock 3 from 2
		header->smaller = temp; 
	}
	
	/*Case I: 
		3
	   /
	  1
	   \
	    2*/ 
	else if(header->smaller->greater != NULL){
		/*First set the two nodes to which 2 connects to as temporaries*/
		struct TreeSetNode *tempSmall = header->smaller->greater->smaller; 
		struct TreeSetNode *tempBig = header->smaller->greater->greater; 
		/*Secondly, have the 2 greater point to the 3*/
		header->smaller->greater->greater = header; 
		/*Thirdly, have the 2 smaller point to the 1*/
		header->smaller->greater->smaller = header->smaller;
		/*Fourthly, have the header point to the 2*/
		if(nodeType == DUMMY){
			((struct DummyHeadNode*)(pointer))->start = header->smaller->greater; 
		}
		else{
			/*This determines if the header node is greater than the pointer node
			@result 0: Pointer > Header
			@result 1: Header > Pointer
			@result -1: Error
			*/
			int isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType); 
			if(isGreater == -1){
			//	printf("Values Are Equal [Rotate Right]"); 
				exit(0);
			}
		
			//If the pointer is greater than the header. Ex: pointer: 5 header: 3. That is, the pointer points to the smaller node
			if(isGreater == 0){
				((struct TreeSetNode*)(pointer))->smaller = header->smaller->greater;
			}
			//If the header is greater than the pointer. That is the pointer points to the greater node
			else{
				((struct TreeSetNode*)(pointer))->greater = header->smaller->greater;
			}
		}
		/*Fifthly, disconnect the 1 greater from the 2, and connect it to the tempSmall*/
		header->smaller->greater = tempSmall; 
		/*Sixthly, disconnect the 3 smaller from the 1, and connect it to the tempBig */
		header->smaller = tempBig; 
		       
		
	}

	/*
	2
  /  \
 1    3
	*/
	else if(header->greater != NULL){
		//set a temporary for the 1's greater
		struct TreeSetNode *temp = header->smaller->greater; 
		//connect the one's greater to the two
		header->smaller->greater = header; 
		//connect the pointer to the one
		((struct DummyHeadNode*)pointer)->start = header->smaller; 
		//Connect the two lesser to the temp
		header->smaller = temp; 

	}
	
}



void rotateLeft(void* pointer, struct TreeSetNode *header, enum dataType nodeType, enum dataType valueType){

	if(header->greater == NULL){
		printf("Cannot Rotate Left"); 
		exit(0); 
		
	}
	//CASE II
	if(header->greater->greater != NULL){
		struct TreeSetNode *temp = header->greater->smaller; 
		header->greater->smaller = header; 
		if(nodeType == DUMMY){
			((struct DummyHeadNode*)(pointer))->start = header->greater; 
		}
		else{
			//So, first we must determine if it is bigger or less than the previous option
			//if the 
			//we must figure out if it's pointing using it's smaller or greater node
			//determines if the header is greater than the pointer
			int isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType); 
			//if they're equal, there's an issue with the tree
			if(isGreater == -1){
			//	printf("Values Are Equal [Rotate Left]"); 
				exit(0);
			}
		
			//If the pointer is greater than the header. Ex: pointer: 5 header: 3. That is, the pointer points to the smaller node
			if(isGreater == 0){
				((struct TreeSetNode*)(pointer))->smaller = header->greater;
			}
			//If the header is greater than the pointer. That is the pointer points to the greater node
			else{
				((struct TreeSetNode*)(pointer))->greater = header->greater;
			}
		}
		header->greater = temp; 
	}
	
	//Case 1
	else if(header->greater->smaller != NULL){
		struct TreeSetNode *tempSmall = header->greater->smaller->smaller; 
		struct TreeSetNode *tempBig = header->greater->smaller->greater; 
		//Make the 2 greater connect to the 3 
		header->greater->smaller->greater = header->greater;
		//Make the 2 smaller connect to the 1
		header->greater->smaller->smaller = header; 
		//Make the pointer point to the 2 
		if(nodeType == DUMMY){
			((struct DummyHeadNode*)(pointer))->start = header->greater->smaller; 
		}
		else{
			//So, first we must determine if it is bigger or less than the previous option
			//if the 
			//we must figure out if it's pointing using it's smaller or greater node
			//determines if the header is greater than the pointer
			int isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType); 
			//if they're equal, there's an issue with the tree
			if(isGreater == -1){
			//	printf("Values Are Equal [Rotate Left]"); 
				exit(0);
			}
		
			//If the pointer is greater than the header. Ex: pointer: 5 header: 3. That is, the pointer points to the smaller node
			if(isGreater == 0){
				((struct TreeSetNode*)(pointer))->smaller = header->greater->smaller;
			}
			//If the header is greater than the pointer. That is the pointer points to the greater node
			else{
				((struct TreeSetNode*)(pointer))->greater = header->greater->smaller;
			}
		}
		//Disconnect the 3 from the 2, and connect it to temp big
		header->greater->smaller =  tempBig; 
		//Disconnect the 1 from the 3
		header->greater = tempSmall; 
		
		
	}
	
	//CASE III 
	else{
		struct TreeSetNode *temp = header->greater->smaller; 
		header->greater->smaller = header; 
		//if the pointer is a dummy header node
		if(nodeType == DUMMY){
			((struct DummyHeadNode*)(pointer))->start = header->greater; 
		}
		//if the pointer is a tree set node
		else{
			int isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType); 
			//if they're equal, there's an issue with the tree
			if(isGreater == -1){
			//	printf("Values Are Equal [Rotate Left]"); 
				exit(0);
			}
		
			//If the pointer is greater than the header. Ex: pointer: 5 header: 3. That is, the pointer points to the smaller node
			if(isGreater == 0){
				((struct TreeSetNode*)(pointer))->smaller = header->greater;
			}
			//If the header is greater than the pointer. That is the pointer points to the greater node
			else{
				((struct TreeSetNode*)(pointer))->greater = header->greater;
			}
			
		}
		header->greater = temp;
	
	}
	//3 Cases: 
		/*
	Case 1: 1
	         \
	          3
	         /
	       2 
	*/ 
	/*Case 2:  1
				\
				 2
				  \
				   3 
	*/ 

	/*
	Case 3:   2
	        /  \
	      1     3
	*/ 

	
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
	short greater = (header->greater == NULL)?-1:header->greater->depth;
	short smaller = (header->smaller == NULL)?-1:header->smaller->depth; 
	 
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


int Remove_TreeSet(void* data, void* pointer, struct TreeSetNode* curNode, enum dataType pointerType, enum dataType valueType){
	int found = 0;
	if(curNode == NULL){
		return -1; 
	}
	int isSmaller = compare(data, curNode->data, valueType); 
	//Compares to see if they're the same, only cares if they're the same
	if(isSmaller == -1){
		Removal(pointer, curNode, pointerType, valueType); 
		//Alright, we've removed the node

		return 1; 
	}
	//if they are not the same
	else{
		//if the node to be removed is smaller than the previous one, it'll go into the smaller value
		if(isSmaller){
			found = Remove_TreeSet(data, curNode, curNode->smaller, SET_NODE, valueType);
		
		}

		//if the node to be removed is greater than the previous one, it'll go into the greater value
		else{
			found = Remove_TreeSet(data, curNode, curNode->greater, SET_NODE, valueType); 
			
		
		}
			
	
		
		
	}
	if(found == 1){
		balance(pointer, curNode, pointerType, valueType); 
		DetermineDepth(curNode); 
	
	}
	return found; 

}


/*This actually goes ahead and removes it once it has been found*/  
void Removal(void* pointer, struct TreeSetNode* header, enum dataType pointerType, enum dataType valueType){
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
		isGreater = compare(((struct TreeSetNode*)(pointer))->data, header->data, valueType);
		
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
		
		header->data = Remove_TwoNodeAttatchment(header, (isGreater)?header->greater:header->smaller, isGreater, pointerType, valueType); 
			
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


void* Remove_TwoNodeAttatchment(void* pointer, struct TreeSetNode *curNode, int isGreater, enum dataType pointerType, enum dataType valueType){
	//Keeps the data such that when it is removed, it can still be stored & returned
	void* temp; 
	void* output = NULL; 
	//So this has to start off by being given a pointer and a curNode and an isGreater
	if(isGreater){
		if(curNode->smaller == NULL){
			temp = curNode->data; 
			Removal(pointer, curNode, SET_NODE, valueType); 
			return temp; 
		}
		else{
			output = Remove_TwoNodeAttatchment(curNode, curNode->smaller, isGreater, pointerType, valueType); 
		}
	}
	else{
		if(curNode->greater == NULL){
			temp = curNode->data; 
			Removal(pointer, curNode, SET_NODE, valueType);
			return temp; 
		}
		else{
			output = Remove_TwoNodeAttatchment(curNode, curNode->greater, isGreater, pointerType, valueType); 
		}
	}
	
	balance(pointer, curNode, pointerType, valueType); 
	DetermineDepth(curNode); 

	return output; 
	//There will be two cases, one if it is greater and another if it isn't
	//Base Case: Smaller if smaller is null
	//Greater: If Greater Is Null
	//It must return the value that it finds	
}

/*Goes through and frees the tree set*/ 
void Free_TreeSet(struct TreeSetNode *header){
	if(header == NULL){
		printf("\n[Free_TreeSet]: Empty List\n"); 
	}
	else{
	
		/*Checks if the greater location exists, if it does, it goes into it and repeats this*/ 
		if(header->greater != NULL){ 
			Free_TreeSet(header->greater); 

		}
		/*Checks if the smaller location exists, if it does, it goes into it and checks it's greater and smaller'*/ 
		if(header->smaller != NULL){
			Free_TreeSet(header->smaller); 
	 
		}
		//Now that both the greater and smaller were null, or have been freed, it frees the header
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


