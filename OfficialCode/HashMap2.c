//We need to allocate and fill this hash map at the same time
//My Goal Is To Have One Method That Both Allocates and Fills the Hash Map as it does
//Since it's only a loop of 26 * 6 
//Let's first make it so that it can just output row, columns row columns

//Fix: Starting with # crashes it 
//Fix: 
#define BUFSIZ 255
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "HashMap2.h"
#include "HashSet.h"
#include "HashFunctions.h"
#include "WordLinkedList.h"
#include "UserInput.h"

#define LETTER_COUNT 26
#define VOWEL_COUNT 6

typedef enum {false, true} bool;

extern int numLetters; 

struct DummyHeadNode*** Create_HashMap(char** allWords){
	
	
	
	
	
	struct DummyHeadNode*** HashMap = Allocate_HashMap(); 
	
	//The line the file is on 
	int lineIndex = 0; 
	/*First, Open the file*/
	FILE *flwd = OpenFile(); 
	//This will take each line of the document
	char line[BUFSIZ];
	
	//Reads through all of the lines
	while(fgets(line, BUFSIZ, flwd) != NULL){
	
		bool isRow = true;
		bool noConnections = (strchr(line, ' ')) ? false : true;
	
		char* currWord = (noConnections) ? strtok(line, "\n") : strtok(line, " "); 
		currWord[numLetters] = '\0';
	
		struct DummyHeadNode* header = HashMap[FirstHashFunction(currWord[0])][SecondHashFunction(currWord)]; 
		if(noConnections == true){
			if(allWords != NULL){
			
				allWords[lineIndex] = malloc(numLetters + 1); 
				safeStrcpy(&allWords[lineIndex], currWord, numLetters, numLetters + 1); 
			}
			addRow(currWord, header); 
			lineIndex++; 
		} 
		struct TreeSetNode* treeSet;
		while(!noConnections && currWord != NULL){ 
			
		
			
			
			currWord[numLetters] = '\0';
		

			
			//If it's a row we add a row
			if(isRow){
				if(allWords != NULL){
				
					allWords[lineIndex] = malloc(numLetters + 1); 
					safeStrcpy(&allWords[lineIndex], currWord, numLetters, numLetters + 1); 
				}
				treeSet = addRow(currWord, header); 
				lineIndex++; 
			}
			//If it's a column we add a column
			else{
				addCol(currWord, (struct word*)treeSet->data); 
			}
	
			
			currWord = strtok(NULL, " ");
			isRow = false; 
		}
		
	} 
	//Close the file 
	fclose(flwd);
	return HashMap; 
	
}

struct TreeSetNode* addRow(char* currWord, struct DummyHeadNode* header){
	//We'll want to create a word linked list who will be the data
	//The header of which will store the row word
	struct word* data = malloc(sizeof(struct word)); 
	data->dataMalloc = 1; 
	data->next = NULL;
	data->word = strdup(currWord); 
	
	
	//First, we want to check if it's null
	//if it is null we then want to allocate dummy head node 
	if(header->start == NULL){
		//Allocate the node
		header->start = malloc(sizeof(struct TreeSetNode)); 
		//Set its variables
		header->start->data = data; 
		header->start->depth = 0;
		header->start->greater = NULL;
		header->start->smaller = NULL; 
		return header->start;
	}
	
	//if it's not null then we want to generate it in
	else{
	
		return AddNode_TreeSet((void*)data, header, header->start, DUMMY, WORDLL); 
	}
	//Next Up:
	//Make it so adding a word returns a tree structure
	//Make it so when you add a column you dont have to do a search 
}
//The column is all of a word's connections 
void addCol(char* currWord, struct word* header){
	//First we will generate the node
	//That wi 
	AddToFront_WordLL(strdup(currWord), header, 1); 
	
}

FILE *OpenFile(){
	char* wordDocuments[3] = {"WordDocuments/Two_Letter_Connections.txt", "WordDocuments/Three_Letter_Connections.txt", /*"WordDocuments/Node_Connections.txt"};*/"WordDocuments/Four_Letter_Connections.txt"};
	/*Four Letter Word Document*/
	FILE *flwd = fopen(wordDocuments[numLetters - 2], "r"); 
	if(flwd == NULL){
		printf("Could not open file: %s\n", wordDocuments[numLetters - 2]);
		
	}
	return flwd;
}

struct word* getList(const char* word, struct DummyHeadNode*** HashMap){
	int index1 = FirstHashFunction(word[0]); 
	int index2 = SecondHashFunction((const char*)word); 
	struct DummyHeadNode* tree = HashMap[index1][index2]; 
	struct TreeSetNode* tNode = Search_TreeSet((char*)word, tree->start, WORD4LL); 

	return (strcmp(word, ((struct word*)(tNode->data))->word) == 0) ? (struct word*)tNode->data : NULL; 
	
}


void Print_HashMap(struct DummyHeadNode** (*HashMap)){
	int i, j; 
	for(i = 0; i < LETTER_COUNT; i++){
		for(j = 0; j < VOWEL_COUNT; j++){
			printf("[%d][%d]\n", i, j); 
			Print_TreeSet(HashMap[i][j]->start, WORDLL); 
		}
	}
}

void Remove_HashMap(char* word, struct DummyHeadNode** (*HashMap)){
	struct word* list = getList(word, HashMap);
	list = list->next; 
	while(list != NULL){
		RemoveOnce_HashMap(list->word, word, HashMap); 
		list = list->next; 
	}
	
	
}

void RemoveOnce_HashMap(char* connector, char* removed, struct DummyHeadNode** (*HashMap)){
	struct word* list = getList(connector, HashMap); 
	Remove_WordLL(removed, list); 
	
	
	
}

struct DummyHeadNode*** Allocate_HashMap(){
	
	//Allocates the Hash Map
	struct DummyHeadNode** (*HashMap) = calloc(LETTER_COUNT, sizeof(struct DummyHeadNode));
	int i;
	int j;
	for(i = 0; i < LETTER_COUNT; i++){
		HashMap[i] = calloc(VOWEL_COUNT, sizeof(struct DummyHeadNode)); 
		for(j = 0; j < VOWEL_COUNT; j++){
			//This will allocate the dummy header node 
			HashMap[i][j] = malloc(sizeof(struct DummyHeadNode));
			HashMap[i][j]->start = NULL; 
		}
	}
	
	return HashMap; 
	
}

void Free_HashMap(struct DummyHeadNode*** HashMap){
	int i, j; 
	for(i = 0; i < LETTER_COUNT; i++){
		for(j = 0; j < VOWEL_COUNT; j++){
			//Frees all but the header 
			Free_TreeSet(HashMap[i][j]->start, WORDLL); 
			//Frees the header
			free(HashMap[i][j]); 
		}
		//Frees the whole block
		free(HashMap[i]); 
	}
	free(HashMap); 
	
}

struct word *getList_Restrictions(char* input, struct word*** HashSet, int cap, struct DummyHeadNode **(*HashMap)){
	int foundWords = 0; 
	/* Create the linked list into which I put the words */
	struct word *output = malloc(sizeof(struct word)); 
	output->next = NULL; 
	output->dataMalloc = 0; 
	output->size = 0; 
	 
	
	struct word* wordOptions = getList(input, HashMap); 

	
	//If it can't find it, it crashes
	if(wordOptions == NULL){
		printf("Error: Can't find word [hashMapOutput]");
		exit(0);  
	}
	

	while(wordOptions->next != NULL){
		
		wordOptions = wordOptions->next; 
		char* currWord = wordOptions->word; 
		//Search the HashSet
		if(Search_HashSet(currWord, HashSet) == 0){

			//If it's in the Hash Set it should add it to the back
			AddToBack_WordLL(currWord, output, 0);  	
			//Then it should add it to the Hash Set
			AddToHashSet(currWord, HashSet, 0); 
			//Then the words found should go up should go up
			foundWords++;  
			//If the cap has been met, it should return the output	
			if(foundWords == cap){
				return output; 
			}
		}
	}

	/* Return the Linked List */
	return output; 
	
	
	
	
	
}
