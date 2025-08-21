/*This is the C file for creating and applying the Hash Maps*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../includes/HashMap.h"
#include "../includes/HashFunctions.h"
#include "../includes/TreeSet.h"
#include "../includes/IntLinkedList.h"

#include "../../flwp/includes/UserInput.h"


typedef enum {false, true} bool;


int getNumAdjacencies(int id, struct DataStructures* data){

	return data->I2W->array[id]->numConnections; 
}

void Initialize_HashMaps_fd(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, int fd, int numLetters){
	//Open up the file 
	FILE* wordDoc = fdopen(fd, "r"); 
	//Read the top number from the file
	int numWords = getNumWords(wordDoc);
	//Allocate the structure using the number of words int --> word (wordData)
	Allocate_IntToWord(IntToWord_HashMap, numWords, numLetters);
	//Place the data into the two new hash maps
	Fill_HashMaps(wordDoc, WordToInt_HashMap, IntToWord_HashMap);
	fclose(wordDoc);
}
void Initialize_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, char* path, int numLetters){
	//Open up the file 
	FILE* wordDoc = OpenFile(path); 
	//Read the top number from the file
	int numWords = getNumWords(wordDoc); 
	//Allocate the structure using the number of words int --> word (wordData)
	Allocate_IntToWord(IntToWord_HashMap, numWords, numLetters);  
	//Place the data into the two new hash maps
	Fill_HashMaps(wordDoc, WordToInt_HashMap, IntToWord_HashMap); 
	fclose(wordDoc);
}

struct DummyHeadNode** *Allocate_WordToInt(){
	int totalLetters = 26; 
	int totalVowels = 6; //a e i o u y
	
	int letter; 
	int vowel; 
	
	struct DummyHeadNode** *WordToInt_HashMap = calloc(totalLetters, sizeof(struct DummyHeadNode**)); 
	//First, we allocate a 2D array that can store space for an array of vowels
	for(letter = 0; letter < totalLetters; letter++){
		WordToInt_HashMap[letter] = malloc(totalVowels * sizeof(struct DummyHeadNode*)); 
		//Second, we run through the vowels in each array and set space for a wordStruct 
		for(vowel = 0; vowel < totalVowels; vowel++){
			WordToInt_HashMap[letter][vowel] = malloc(sizeof(struct DummyHeadNode)); 
			WordToInt_HashMap[letter][vowel]->start = NULL; 
			
		}
	
	}
	return WordToInt_HashMap; 
	
}

void Allocate_IntToWord(struct wordDataArray* IntToWord_HashMap, int numWords, int numLetters){
	struct wordData** array = calloc(numWords, sizeof(struct wordData*)); 
	IntToWord_HashMap->array = array; 
	IntToWord_HashMap->numWords = numWords; 
	IntToWord_HashMap->numLetters = numLetters; 



}

struct wordDataArray *Allocate_IntToWordStruct(){
	 struct wordDataArray *IntToWord_Struct = malloc(sizeof(struct wordDataArray)); 
	 return IntToWord_Struct; 
}; 

void Fill_HashMaps(FILE* wordDoc, struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	char line[BUFSIZ]; 
	int id = 0;
    for(int x = 0; x < IntToWord_HashMap->numWords; x++){
        fgets(line, BUFSIZ, wordDoc);
		//So, now we need to make 2 nodes, 1 wordData node, and one wordStruct node
		//The word of the line
		char* currValue = strtok(line, " ");  
		currValue[IntToWord_HashMap->numLetters] = '\0'; 
		//Put the word into word --> int structure
		struct wordData* wordData = Create_WordData(currValue); 
		struct wordStruct* wordStruct = Create_WordStruct(currValue, id); 
		
		
		//The second paramater of the strtol
		char* numBuff; 
		
		int connection; 
		
		currValue = strtok(NULL, " "); 
		//Loops through the numbers
		while(currValue != NULL){
			//It takes the current value converts it to a number, then grabs the next one 
			 
			
			connection = strtol(currValue, &numBuff, 10); 
			//Put the word into the link list
			AddToFront_IntLL(connection, wordData->connectionHeader);
			
			//each time it adds a word as a connection, it updates the number of connections
			wordData->numConnections++; 
			
			currValue = strtok(NULL, " "); 
		}	

		
		
		//Then it will be time to put the words into their respective locations in their data structure
		//First, put it in the spot in the array
		IntToWord_HashMap->array[id] = wordData; 
		
		//Then, put it in its spot in the data list
		int letterIndex = FirstHashFunction(wordData->word[0]);
		int vowelIndex = SecondHashFunction(wordData->word, IntToWord_HashMap); 
		struct DummyHeadNode *treeHeader = WordToInt_HashMap[letterIndex][vowelIndex]; 
		AddNode_TreeSet(wordStruct, treeHeader, treeHeader->start, DUMMY, WORD_STRUCT, IntToWord_HashMap->numLetters);
  	
		id++; 
		
		
		
		
		
	}
	//Obtain a line
		//Obtain the first word 
		//Place the word into the word part of int --> word
		//Take the first letter of that word
		//Take the first vowel of that word
		//Go to the 2nd dimension & add a structure w/ the word & its ID to the AVL Tree
		//Go through the numbers & add them to the connections in the wordData
	
	
}

void Print_WordToInt_HashMap(struct DummyHeadNode** *HashMap){
	int vowels[] = {'a', 'e', 'i', 'o', 'u', 'y'}; 
	
	int l = 0; 
	int v = 0;
	
	int totalLetters = 26; 
	int totalVowels = 6; 
	for(l = 0; l < totalLetters; l++){
		for(v = 0; v < totalVowels; v++){
			printf("\n[%c][%c]: ", l + 97, vowels[v]);
			Print_TreeSet(HashMap[l][v]->start, WORD_STRUCT); 
		}
		printf("\n"); 
	}
	printf("\n"); 
	
}

void Print_IntToWord_HashMap(struct wordDataArray* HashMap){
	int i = 0; 
	for(i = 0; i < HashMap->numWords; i++){
		Print_WordData(HashMap->array[i]);  
		printf("\n"); 
	}
	
	
}



void Print_WordStruct(struct wordStruct* wStruct){
	printf("%s, %d\n", wStruct->word,wStruct->wordID); 
	
}
void Print_WordData(struct wordData* wData){
	printf("%s: ", wData->word);
	Print_IntLL(wData->connectionHeader);  

}

void Free_HashMaps(struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap){
	int numLetters = 26; 
	int numVowels = 6; 
	
	int letter = 0; 
	int vowel = 0; 
	for(letter = 0; letter < numLetters; letter++){
		for(vowel = 0; vowel < numVowels; vowel++){
			Free_TreeSet(WordToInt_HashMap[letter][vowel]->start, WORD_STRUCT); 
			free(WordToInt_HashMap[letter][vowel]); 
		}
		free(WordToInt_HashMap[letter]); 
		
	}
	free(WordToInt_HashMap); 
	
	int i; 
	for(i = 0; i < IntToWord_HashMap->numWords; i++){
		Free_WordData(IntToWord_HashMap->array[i]);
	 
		
	}
	free(IntToWord_HashMap->array); 
	free(IntToWord_HashMap); 
}

int getNumWords(FILE* wordDoc){
	char numWordsStr[BUFSIZ];
	//The first word of the file becomes the first number of words 
	fgets(numWordsStr, BUFSIZ, wordDoc); 
	char* newStr; 
	long numWords = strtol(numWordsStr, &newStr, 10);
	return (int)numWords; 
	
}


struct wordData* Create_WordData(char* word){
	struct wordData* wordData = malloc(sizeof(struct wordData)); 
	wordData->connectionHeader = malloc(sizeof(struct intList)); 
	wordData->connectionHeader->next = NULL; 
	wordData->word = strdup(word);  
	wordData->hintFound = 0;
	wordData->numConnections = 0; 
	wordData->prevID = -1;  
	return wordData; 
} 

void Free_WordData(struct wordData* wordData){
	Free_IntLL(wordData->connectionHeader); 
	free(wordData->word); 
	free(wordData); 
}
struct wordStruct* Create_WordStruct(char* word, int id){
	struct wordStruct* wordStruct = malloc(sizeof(struct wordStruct)); 
	wordStruct->word = strdup(word);
	wordStruct->wordID = id;  
	
	return wordStruct;
}

void Free_WordStruct(struct wordStruct* wordStruct){
	free(wordStruct->word); 
	free(wordStruct); 
	
}

int Convert_WordToInt(char* word, struct DataStructures* data){
	if(word == NULL){
		return -1; 
	}
	
	int index1 = FirstHashFunction(word[0]); 
	int index2 = SecondHashFunction(word, data->I2W);
	struct DummyHeadNode *treeHead = data->W2I[index1][index2]; 
	
	//Check the length of the word
	int i = 0; 
	while(word[i] != '\0'){
		i++; 
	}
	if(i != data->I2W->numLetters){
		return -1; 
	}
	
	//If the front of the tree set does not exist
	if(treeHead->start == NULL){
		return -1; 
	}
	struct TreeSetNode* dataPointer = Search_TreeSet(word, treeHead->start, WORD_STRUCT_CHECK, data->I2W->numLetters); 
	if(dataPointer == NULL){

		return -1; 
		
	}
	int wordID = ((struct wordStruct*)(dataPointer->data))->wordID;  
	return wordID; 
	
	
	
}

char* Convert_IntToWord(int wordID, struct wordDataArray* IntToWord_HashMap){
	
	if(wordID >= IntToWord_HashMap->numWords){
		return NULL; 
	}
	if(wordID < 0){
		return NULL; 
	}
	return IntToWord_HashMap->array[wordID]->word; 
	
	
}

struct intList* getConnections(int id, struct wordDataArray* IntToWord_HashMap){
	return IntToWord_HashMap->array[id]->connectionHeader; 
};

void printOptions(int id, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet){
	struct intList* currOption = IntToWord_HashMap->array[id]->connectionHeader->next;
	printf("%s Options: ", Convert_IntToWord(id, IntToWord_HashMap));
	while(currOption != NULL){
		if(checkIfUsed_WordSet(currOption->data, wordSet) == 0){
			printf("%s ", Convert_IntToWord(currOption->data, IntToWord_HashMap));
		}
		currOption = currOption->next;
	}
	printf("\n");
	
}

FILE *OpenFile(char* path){
	//char* wordDocuments[3] = {"../docs/2.txt", "../docs/3.txt", /*"WordDocuments/t/ab2.txt"};*/"../docs/4.txt"}; 
	/*Four Letter Word Document*/
	FILE *flwd = fopen(path, "r"); 
	
	if(flwd == NULL){
		printf("Could not open file: %s\n", path);
		exit(1);		

	}

	return flwd;
}


struct intList *getConnections_Restrictions(int input, int cap, struct wordDataArray* IntToWord_HashMap){
	int foundWords = 0; 
	/* Create the linked list into which I put the words */
	struct intList *output = malloc(sizeof(struct intList));
	output->size = 0;  
	output->next = NULL; 
	
	
	struct intList* wordOptions = getConnections(input, IntToWord_HashMap); 

	
	//If it can't find it, it crashes
	if(wordOptions == NULL){
		printf("Error: Can't find word [hashMapOutput]");
		exit(0);  
	}
	

	while(wordOptions->next != NULL){
		
		wordOptions = wordOptions->next; 
		int currWord = wordOptions->data; 
		//Search the HashSet
		if(getHintFound(currWord, IntToWord_HashMap) == 0){

			//If it's in the Hash Set it should add it to the back
			AddToBack_IntLL(currWord, output);  	
			//Then it should add it to the Hash Set
			setHintFound(currWord, IntToWord_HashMap);  
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



void setHintFound(int wordID, struct wordDataArray* IntToWord_HashMap){
	IntToWord_HashMap->array[wordID]->hintFound = 1; 
	
}

void removeHintFound(int wordID, struct wordDataArray* IntToWord_HashMap){
	IntToWord_HashMap->array[wordID]->hintFound = 0; 	
}

int getHintFound(int wordID, struct wordDataArray* IntToWord_HashMap){
	return IntToWord_HashMap->array[wordID]->hintFound;  
}


void reset_HashSet(struct wordDataArray* IntToWord_HashMap){
	int i = 0; 
	for(i = 0; i < IntToWord_HashMap->numWords; i++){
		IntToWord_HashMap->array[i]->hintFound = 0; 

	}
}
