#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "WordLinkedList.h"
#include "TreeStorageNode.h"
#include "HashSet.h"
#include "2DLinkedList.h"
#include "HashFunctions.h"


/*Private Method, For The Tree Storage Node, Checks if the word has been found or the limit has been reached for finding a word
@param goal --> The goal word to be found, if null it should check the connection count
@param isEnough --> Checks if the connection count is enough
@return --> Returns a boolean as to whether or not the goal is found*/ 
int isFound_TSN(struct TreeStorageNode* curNode, char* goal, int minConnections); 

struct TreeStorageNode *Allocate_TreeStorageNode(char* word){
	struct TreeStorageNode *header = malloc(sizeof(struct TreeStorageNode)); 
	header->next = malloc(sizeof(struct TreeStorageNode)); 
	header->next->next = NULL; 
	header->next->word = word; 
	header->next->prev = NULL; 
	header->next->depth = 0; 
	return header; 
	
} 
struct TreeStorageNode *Add_TreeStorageNode(char* word, struct TreeStorageNode *prev, struct TreeStorageNode *header, int depth){
	while(header->next != NULL){
		header = header->next; 
	}
	header->next = malloc(sizeof(struct TreeStorageNode));
	header->next->next = NULL; 
	header->next->prev = prev; 
	header->next->word = word;  
	header->next->depth = depth; 
	return header->next; 

}
struct TreeStorageNode *Copy_WordLLToTreeStorageNode(struct TreeStorageNode *header, struct TreeStorageNode *prev, struct word *list, char* goal, int minConnections){

	//It goes to the next location in the word Queue, skipping past the header
	list = list->next; 
	//Until there is no more links that the current word has
	while(list != NULL){	
		//It adds the word to the tree storage node as well as the Node pointer to which it connects, I need this to return the structure it added
		struct TreeStorageNode *final = Add_TreeStorageNode(list->word, prev, header, prev->depth + 1);  
		
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






int isFound_TSN(struct TreeStorageNode* curNode, char* goal, int minConnections){
	if(goal == NULL){
		//printf("\nCur Node Depth: %d, Word: %s", curNode->depth, curNode->word); 
		return curNode->depth == minConnections; 		
	}
	
	//if they're equal return 1
	//else, return 2
	return(strcmp(curNode->word, goal)== 0)?1:0; 

}


struct TreeStorageNode *Search_TreeStorageNode(char* word, struct TreeStorageNode *header){
	header = header->next; 
	while(header != NULL && strcmp(word, header->word) != 0){
		header = header->next; 
		
	}
	if(header == NULL){
		printf("\nCould Not Find %s [Search_TreeStorageNode]", word);
		return NULL; 
	}
	return header; 
}



struct TreeStorageNode *SearchPrev_TreeStorageNode(char* nextWord, struct TreeStorageNode *header){
	/*In case we want the word before the word it's searching for, this replicates having a prevLoc node on each Tree Storage Node*/ 
	while(strcmp(nextWord, header->next->word) != 0){
		header = header->next;  
		if(header->next == NULL){
			printf("\nCouldn't Find Word [SearchPrev_TreeStorageNode]");
			exit(0); 
		}
		
	}
	return header;
	
}
struct TreeStorageNode *ReturnLast_TreeStorageNode(struct TreeStorageNode *header){
	//Until the next node is null, it will keep looping through, and then it will return the last value 
	while(header->next != NULL){
		header = header->next; 
	}
	return header; 
	
}
struct word *Convert_TreeStorageNodeToWordLL(struct word *newList, struct TreeStorageNode *End){
	
	if(End->prev != NULL){
		Convert_TreeStorageNodeToWordLL(newList, End->prev); 
	}
	AddToBack_WordLL(End->word, newList, 0); 
	return newList; 
	
	
}
void Convert_TreeStorageNodeTo2DArray(char** arr, struct TreeStorageNode *End, int minConnections){
	/*While the location is not equal to null, make the index go down*/
	/*The index starts at minConnection -1 , because if it started at minConection it would go 3:3 2:2 1:1, when it needs to be 3:2 2:1 1:0*/ 
	int index = minConnections;
	struct TreeStorageNode *curNode = End; 
	while(curNode != NULL){ 
		strcpy(*(arr + index), curNode->word); 
		curNode = curNode->prev; 
		index--; 
	} 
	 
	
	
}



void Print_TreeStorageNode(struct TreeStorageNode *header){
	header = header->next; 
	while(header != NULL){
		printf("%s", header->word);
		if(header->next != NULL){
			printf("->"); 
		}
		header = header->next;  
	}


}
void Free_TreeStorageNode(struct TreeStorageNode *header){
	while(header != NULL){
		struct TreeStorageNode temp = *header;  
		free(header); 
		header = temp.next; 
		
	}
} 
void Remove_TreeStorageNode(char* word, struct TreeStorageNode *header, struct word **(*HashSet)){
	struct TreeStorageNode *prev = header;
	header = header->next; 
	 
	while(header != NULL && strcmp(word, header->word) != 0){
		prev = header;
		header = header->next; 
		 
		
	}
	if(header == NULL){
		//printf("\nWord could not be found. [Remove_TreeStorageNode]");
		exit(0); 
	}
	//Once it is found
	prev->next = header->next; 
	if(HashSet != NULL){
		Remove_HashSet(header->word, HashSet); 
	}
	free(header); 
	
}

void RemoveAll_TreeStorageNode(char* word, struct TreeStorageNode *header, struct word **(*HashSet)){
	int isFound = 0; 
	struct TreeStorageNode *prev = header; 
	header = header->next; 
	while(header != NULL){
		
		if(strcmp(word, header->word) == 0){
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
	Remove_HashSet(word, HashSet); 
	}
}
void RemoveFromPoint_TreeStorageNode(struct TreeStorageNode *header, struct word *QueueHeader, struct word **(*HashSet)){
	while(header != NULL){
		struct TreeStorageNode temp = *header; 
		if(HashSet != NULL){
			Remove_HashSet(header->word, HashSet); 
		} 
		free(header); 
		header = temp.next; 
		
	}
}

