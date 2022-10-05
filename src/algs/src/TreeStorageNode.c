#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../includes/TreeStorageNode.h"

#include "../../flwp/includes/UserInput.h"

#include "../../structs/includes/WordLinkedList.h"
#include "../../structs/includes/HashFunctions.h"


extern int numLetters; 

/*Private Method, For The Tree Storage Node, Checks if the word has been found or the limit has been reached for finding a word
@param goal --> The goal word to be found, if null it should check the connection count
@param isEnough --> Checks if the connection count is enough
@return --> Returns a boolean as to whether or not the goal is found*/ 
int isFound_TSN(struct TreeStorageNode* curNode, int goal, int minConnections); 

struct TreeStorageNode *Allocate_TreeStorageNode(int id, int noConnections){
	struct TreeStorageNode *header = malloc(sizeof(struct TreeStorageNode)); 
	header->next = malloc(sizeof(struct TreeStorageNode)); 
	header->next->next = NULL; 
	header->next->id = id; 
	header->next->prev = NULL; 
	header->next->noConnections = noConnections; 
	header->next->depth = 0; 
	return header; 
	
} 
struct TreeStorageNode *Add_TreeStorageNode(int id, struct TreeStorageNode *prev, struct TreeStorageNode *header, int noConnections){
	while(header->next != NULL){
		header = header->next; 
	}
	header->next = malloc(sizeof(struct TreeStorageNode));
	header->next->next = NULL; 
	header->next->prev = prev; 
	header->next->id = id;  
	header->next->noConnections = noConnections;
	header->next->depth = prev->depth + 1;

	return header->next; 

}
struct TreeStorageNode *Copy_WordLLToTreeStorageNode(struct TreeStorageNode *header, struct TreeStorageNode *prev, struct intList *list, int goal, int minConnections){

	//It goes to the next location in the word Queue, skipping past the header
	list = list->next; 
	//Until there is no more links that the current word has
	while(list != NULL){	
		//It adds the word to the tree storage node as well as the Node pointer to which it connects, I need this to return the structure it added
		struct TreeStorageNode *final = Add_TreeStorageNode(list->data, prev, header, 0);  
		
		//Checks if it has found the word
		if(isFound_TSN(final, goal, minConnections)){
			while(header->next != NULL){
				header = header->next; 
			
				 
			}
			
			return header; 
		}
		list = list->next;
	}
 
	return NULL; 

}
//I need a second Copy_WordLLToTreeStorageNode
//How it works, is it's given the header of the prevConnection of the Tree Storage LL
//It's also given the previous location that does not change
//It's given the linkOutput
//It's given teh minimum connections






int isFound_TSN(struct TreeStorageNode* curNode, int goal, int minConnections){
	if(goal == -1){

		return curNode->depth == minConnections; 		
	}
	
	//if they're equal return 1
	//else, return 2
	return(curNode->id == goal)?1:0; 

}



struct TreeStorageNode *ReturnLast_TreeStorageNode(struct TreeStorageNode *header){
	//Until the next node is null, it will keep looping through, and then it will return the last value 
	while(header->next != NULL){
		header = header->next; 
	}
	return header; 
	
}
struct intList *Convert_TreeStorageNodeToIntLL(struct intList *newList, struct TreeStorageNode *End){
	
	if(End->prev != NULL){
		Convert_TreeStorageNodeToIntLL(newList, End->prev); 
	}
	AddToBack_IntLL(End->id, newList); 
	return newList; 
	
	
}
void Convert_TreeStorageNodeToIntArray(int* arr, struct TreeStorageNode *End, int minConnections){
	/*While the location is not equal to null, make the index go down*/
	/*The index starts at minConnection -1 , because if it started at minConection it would go 3:3 2:2 1:1, when it needs to be 3:2 2:1 1:0*/ 
	int index = minConnections;
	struct TreeStorageNode *curNode = End; 
	while(curNode != NULL){ 
		arr[index] = curNode->id; 
		curNode = curNode->prev; 
		index--; 
	} 
	 
	
	
}




void Print_TreeStorageNode(struct TreeStorageNode *header){
	int i = 0; 
	header = header->next; 
	while(header != NULL){
		printf("%d", header->id);
		struct TreeStorageNode *temp = header; 
		while(temp->prev != NULL){
			temp = temp->prev; 
			printf("->%d", temp->id); 
			
		}
		if(header->next != NULL){
			printf("\n"); 
		}
		header = header->next;  
		i++; 
	}
	printf("\n\n"); 


}

/*This takes a node, and outputs all of the words that lead to it getting connected to*/
void Print_TreeStorageReverseConnections(struct TreeStorageNode *End){
	
 
	while(End->prev != NULL){
		printf("%d->", End->id);
		End = End->prev; 
		 
		
	}
	printf("%d", End->id);
	
}
void Free_TreeStorageNode(struct TreeStorageNode *header){
	while(header != NULL){
		struct TreeStorageNode temp = *header;  
		free(header); 
		header = temp.next; 
		
	}
} 
void Remove_TreeStorageNode(int id, struct TreeStorageNode *header, struct WordSet *wordSet){
	struct TreeStorageNode *prev = header;
	header = header->next; 
	 
	while(header != NULL && id != header->id){
		prev = header;
		header = header->next; 
		 
		
	}
	if(header == NULL){
		//printf("\nWord could not be found. [Remove_TreeStorageNode]");
		exit(0); 
	}
	//Once it is found
	prev->next = header->next; 

	markUnused_WordSet(id, wordSet); 
	
	free(header); 
	
}

void RemoveAll_TreeStorageNode(int id, struct TreeStorageNode *header, struct WordSet *wordSet){
	int isFound = 0; 
	struct TreeStorageNode *prev = header; 
	header = header->next; 
	while(header != NULL){
		
		if(id == header->id){
			isFound = 1; 
			prev->next = header->next; 
			free(header); 
			header = prev->next; 
			
		}
		else{
			prev = header; 
			header = header->next; 
		}
 
		
	}
	if(isFound == 0){
		//printf("Word Could Not Be Found [RemoveAll_TreeStorageNode]"); 
		exit(0); 
	}
	else{
		markUnused_WordSet(id, wordSet);  
	}
}
void RemoveFromPoint_TreeStorageNode(struct TreeStorageNode *header, struct WordSet *wordSet){
	while(header != NULL){
		struct TreeStorageNode temp = *header; 
		markUnused_WordSet(header->id, wordSet); 
	
		free(header); 
		header = temp.next; 
		
	}
}

