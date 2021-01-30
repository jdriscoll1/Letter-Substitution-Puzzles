/********************************
Name: Jordan D
Date: August 17th, 2019
Goal: Be able to pathfind from one word to another through the flwg 

gcc -O -Wall -Wextra -ansi -o flwp FLWP.c
*********************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct word{
	/* A boolean that determines if the word is found */
	int isFound; 
	/*The Data it is holding*/
	char* word; 
	/*The next location in the header*/
	struct word *nextLoc; 
	
}; 
/* The 2D Linked List */
struct wordConnections{
	/* So, this actually has to store a word, and all the words it connects to */
	/*The word that will be saved in each location*/
	char* word; 
	/* Determines if the word has already been found, I would remove it, however, I still need it's previous word */
	int isFound; 

	

	/*next word from the 2D List */
	struct wordConnections *nextRow;
	/* Next word of the 2D List */
	struct wordConnections *nextColumn;
	/* Previous word of the 2D List */
	struct wordConnections *previousColumn; 
	
	
};

struct nodeQueue{
    struct threeLetterConnections *node; 
	struct nodeQueue *nextLoc; 	
}; 

struct threeLetterConnections{
	/*The word it's storing */
	char* word; 
	/*Determines if the word has already been added to the Queue */
	int isFound; 
	/*The next location in the linked list */
	struct threeLetterConnections *nextLoc; 
	/*The previous location in the linked list */
	struct threeLetterConnections *prevLoc; 
	/*The previous three letter connection (TLC)*/
	struct nodeQueue *prevConnection; 
};



struct wordConnections ***CreateCategories(int numLetters, char** wordStorage); 
/* Function that determines where a word belongs in the 2D Array */
int HashFunction(char firstLetter); 
int SecondHashFunction(char* word, int numLetters); 
/*Goes through and searches for a vowel*/
int VowelSearch(char letter, char vowels[6]); 
void FreeCategories(struct wordConnections ***Categories); 
/*In order to make a linked list, I'm going to need a few Methods*/
/*Free Link List*/
void FreeLinkList(struct word *header);
/* Frees up the space in the linked list, only for when it's completed its purpose */
void FreeLinkListTLC(struct threeLetterConnections *header);
/*Add to word Link List*/
void AddLinkList(char* word, struct word *header, int isFound); 
/* Add to back of the Three Letter Connection Link List */
void AddLinkListTLC(char* word, struct threeLetterConnections *header, int isFound);
/*Print Out Link List*/
void PrintLinkList(struct word *header); 
/*Print Out Link List*/
void PrintLinkListTLC(struct threeLetterConnections *header); 
/*Clears Out Link List*/
void ClearLinkList(struct threeLetterConnections *header); 
/* Copies all of the untis from one linked list into another, for linked lists with conflicting structures */
void CopyLinkListTLC(struct threeLetterConnections *receiver, struct word *quarterback); 
/* This will take the value to the front of the link list */
struct threeLetterConnections *ToLinkListFront(struct threeLetterConnections *back); 
/* 2D LINKED LIST METHODS */
/* Add Column <Rightwards> */
void AddColumn2DLL(char* input, struct wordConnections *header); 
/* Add Row <Downwards> */
void AddRow2DLL(char* input, struct wordConnections *header); 
/* Print Out Linked List */
void Print2DLL(struct wordConnections *header); 
/* Free Linked List */
void Free2DLL(struct wordConnections *header);
/* Removes from a 2D Link List */
void Remove2DLL(char* word, struct wordConnections *header);
/* Sets all the words of a specific type in a link list to found */
void Find2DLL(char* word, struct wordConnections *header); 
/* Mallocate Safely */
void *chkmalloc(size_t sz);  
/* This function will accept a word as an input, and output all of the connecting values */
struct word *linkOutput(char* wordInput, struct wordConnections ***Categories, int numLetters); 
/*Adds A Part of the Queue to the Linked List of Nodes */
void AddLinkListQ(struct threeLetterConnections *node, struct nodeQueue *header); 
/*Prints all of words saved in the nodes of the saved nodes of the Queued Linked List*/
void PrintLinkListQ(struct nodeQueue *header); 
/* Frees up the space in the linked list, only for when it's completed its purpose */
void FreeLinkListQ(struct nodeQueue *header); 
/* Frees the Saved Spaces that stores where the words are */
void FreeCategorysWords(char** wordStorage, int loops); 
/* removes the first value of a link list */
struct word *Dequeue(struct word *header);
/*removes the first node of the Queue that stores nodes*/
struct nodeQueue *NodeDequeue(struct nodeQueue *header); 

/* The Pathfinder, using the Breadth-First search algorithm */
struct word *Pathfind(char* start, char* goal, struct wordConnections ***Categories, int numLetters);
/* So, I need to make a method that will remove a word from all of it's locations in the array of 2D linked lists */
void RemoveWord(char* removalWord, struct wordConnections ***Categories, int numLetters);
/*This will put all of the previous nodes on all the word TLCs*/
void ImplementNodeQueue(struct nodeQueue *NodeQueue, struct threeLetterConnections *CurrentWordConnections); 
/*Prints out a Reversed a TLC Linked List*/
void ReverseLinkListTLC(struct threeLetterConnections *header);  
/*What happens after the word is found*/
void WordFound(char* start, struct nodeQueue *NodeQueue, struct threeLetterConnections *CurrentWordConnections, int isComplete); 
int main(){
	/* The Amount of words in each file, File 1, 2, 3 */
	int totalWordQuantity[3] = {132, 7420, 19829}; 
	/* Sets the number of letters that will be used */
	int numLetters = 4; 

	char** wordStorage = malloc(sizeof(char*) * totalWordQuantity[numLetters - 2] * (numLetters + 2));
	
	/* Creates the Categories */
	struct wordConnections ***Categories = CreateCategories(numLetters, wordStorage);   

	Pathfind("zoom", "vest", Categories, numLetters);   
	//free(Categories); 
	FreeCategories(Categories);
	FreeCategorysWords(wordStorage, totalWordQuantity[numLetters-2]);
	
	
	
	return 0; 
}

int HashFunction(char firstLetter){
	return  firstLetter - 97; 
	
}
/*returns the fist vowel in the word*/
int SecondHashFunction(char* word, int numLetters){
	
	char vowels[] = {'a', 'e', 'i', 'o', 'u', 'y'}; 
	/*The parser that goes through each letter in the word*/
	int wordParse = 0; 
	
	/*Loops through until it finds a vowel*/
	for(wordParse = 1; wordParse < numLetters; wordParse++){
		 int vowelCheck = VowelSearch(word[wordParse], vowels);
		 if(vowelCheck != -1){
			 return vowelCheck; 
			 
		 }
		
		
	}
	
	return VowelSearch(word[0], vowels);
}

int VowelSearch(char letter, char vowels[6]){
	int vowelParse = 0; 
	/*Loops through all of the vowels and checks if it the word matches it*/
	for(vowelParse = 0; vowelParse <= 5; vowelParse++){
		if(letter == vowels[vowelParse]){
			/*returns the first vowel in teh word*/
			return vowelParse; 
			
		}
		
		
	}
	return -1; 
}
/* gcc -O -Wall -Wextra -ansi -o flwp FLWP.c */
/*Puts all the words in their proper categories*/
struct wordConnections ***CreateCategories(int numLetters, char** wordStorage){
	int letterConnections = 0;
	char mostConnections[6]; 
	int maxConnections = 0; 
	/*the word that we test*/
	char word[numLetters + 2]; 
	/*how many times we've changed the character*/
	int letterSpot = 0; 
	/*the character that goes through the file*/
	char c; 
	
	/*Opens the four letter word document*/
	FILE *flwd; 
	/**/
	/*The categories which the word is stored in, the first one is the first letter of the word, the second is the first vowel of the word*/
	struct wordConnections ***category = malloc(26* (numLetters + 2) *(sizeof(struct wordConnections))); 
 
	/*I have to make sure to allocate every 2D memory */
	int parse1 = 0;
	int parse2 = 0;  
	/* So, in order for the linked lists to actually work, I need a place to store the words inside of them, the string of the word. So,
			I need an array where I just store the information.
	*/
	 
	/* Now, I need to go through and reap a place in the computer.. for all of these. */
	
	for(parse1 = 0; parse1 < 26; parse1++){
		/* This frees up all of the space of the first category group */
			
		category[parse1] = calloc(6, sizeof(struct wordConnections)); 
		if(category[parse1] == NULL){
			printf("Code Stopped Working"); 
			exit(0);
		}
	
		for(parse2 = 0; parse2 < 6; parse2++){
			 
			 
			/* This allocates the linked list in the array */
			
				category[parse1][parse2] = chkmalloc(sizeof(struct wordConnections)); 
				if(category[parse1][parse2]== NULL){
					printf("Fail"); 
					exit(0); 
				}
		
			/* This makes it so the stuff pre-stored isn't junk */
			category[parse1][parse2]->nextRow = NULL;
			category[parse1][parse2]->nextColumn = NULL;
			category[parse1][parse2]->previousColumn = NULL;
			
			
		}
		
	}
 
	char* wordDocuments[3] = {"OfficialCode/WordDocuments/Two_Letter_Connections.txt", "OfficialCode/WordDocuments/Three_Letter_Connections.txt", "WordDocuments/Four_Letter_Connections.txt"};
	flwd = fopen(wordDocuments[numLetters - 2], "r"); 
	if(flwd == NULL){
		printf("Cold Dog.");
		
	}
	
	

	/* is it the first word of the line */
	int wordCount = 0; 
	int firstWord = 1; 
	/* P is a step ahead of c, because otherwise the words get super messed up. */
	char p = fgetc(flwd); 
	c = p; 
	/* So, this is a temporary word, who will store the row, so all of its category folks will be contained within it */
	char* rowWord = malloc(sizeof(char) * (numLetters + 2)); 
	
	/* First, take the character */
	while(c != EOF){
		
		p = fgetc(flwd);

		/* Add the character to the word */
		word[letterSpot] = c; 
		/* Allows the letter spot to find the next place into the word */
		letterSpot++; 
		
		/* Determines if the character is the placement immediately after the word */
		if((c == ' ' && p != '\n') || c == '\n'){
			
			letterSpot = 0; 
			/* Throws in the \0, converting into a string */
			word[numLetters] = '\0'; 
			
			wordStorage[wordCount] = (char*)malloc(sizeof(char) * (numLetters + 2)); 
			
	
			strcpy(wordStorage[wordCount], strdup(word)); 
					
				
			/* Determine if it's the first word */
			if(firstWord == 1){
				if(letterConnections > maxConnections){
					strcpy(mostConnections, rowWord); 
					maxConnections = letterConnections; 
					
					
				}
				strcpy(rowWord, word); 
				
				
				
				letterConnections = 0; 
				/* Throw it in as a row */
				AddRow2DLL(wordStorage[wordCount], category[HashFunction(word[0])/*First Letter*/][SecondHashFunction(word, numLetters)]/*First Vowel*/); 
				
				
			}
			/* If it's not the first word */
			else{
				letterConnections += 1; 
				AddColumn2DLL(wordStorage[wordCount], category[HashFunction(rowWord[0])/*First Letter*/][SecondHashFunction(rowWord, numLetters)]/*First Vowel*/); 
				
			}
			
			if(c == ' '){
				firstWord = 0; 
				
				
				
			}
			if(c == '\n'){
				firstWord = 1; 
				
			}
			wordCount++; 
			
			
		}
		
		
		c = p; 
		
	}
	
	
	free(rowWord);
	fclose(flwd);
	


	
	
	
	
	return category; 
	
	
	
	
	
}
	/* Goes through all of the categories and frees them */
void FreeCategories(struct wordConnections ***Categories){
	int category1Quantity = 26; 
	int category2Quantity = 5; 

	/*I have to make sure to allocate every 2D memory */
	int parse1 = 0;
	int parse2 = 0; 
 
	for(parse1 = 0; parse1 < category1Quantity; parse1++){ 
		
		for(parse2 = 0; parse2 < category2Quantity; parse2++){
			
			Free2DLL(Categories[parse1][parse2]);

		}
		free(Categories[parse1]); 
		
	}
	free(Categories);
	
	
	
}
/* This adds code to the back of the linked list of the word structure */
void AddLinkList(char* word, struct word *header, int isFound){
	/*So, all it needs to do is starting from the header, loop through the list, and set the next location to the word*/
	while(header->nextLoc != NULL){
		header = header->nextLoc; 
		
	}
	/* Opens up space for the next node */
	header->nextLoc = malloc(sizeof(struct word)); 
	/* Cleans out whatever nonsense is already stored there */
	header->nextLoc->nextLoc = NULL; 
	
	/* Implements the word in the next location */
	header->nextLoc->word = word;
	/* Makes it so the word is most certainly, not found */
	header->nextLoc->isFound = isFound; 	
	
}

/* This adds code to the back of the linked list of the three letter connections structure */
void AddLinkListTLC(char* word, struct threeLetterConnections *header, int isFound){
	
	/*So, all it needs to do is starting from the header, loop through the list, and set the next location to the word*/
	while(header->nextLoc != NULL){
	
		header = header->nextLoc; 
		
	}
	
	/* I create a temporary header in fear that this will be lost */
	struct threeLetterConnections *tempHead = header;  
	/* Opens up space for the next node */
	header->nextLoc = malloc(sizeof(struct threeLetterConnections)); 
	/* Cleans out whatever nonsense is already stored there */
	header->nextLoc->nextLoc = NULL; 
	/* Implements the word in the next location */
	header->nextLoc->word = word;
	header->nextLoc->isFound = isFound; 
	/* Sets this as the headers previous location */
	header->nextLoc->prevLoc = malloc(sizeof(struct threeLetterConnections));
	header->nextLoc->prevLoc = NULL;
	/*Sets this as the headers previous connection */
	header->nextLoc->prevConnection = malloc(sizeof(struct nodeQueue));
	header->nextLoc->prevConnection = NULL;
	header->nextLoc->prevLoc = tempHead; 
	
	
}


void AddLinkListQ(struct threeLetterConnections *node, struct nodeQueue *header){
		/*So, all it needs to do is starting from the header, loop through the list, and set the next location to the word*/
	while(header->nextLoc != NULL){
		header = header->nextLoc; 
		
	}
	/* Opens up space for the next node */
	header->nextLoc = malloc(sizeof(struct nodeQueue)); 
	/* Cleans out whatever nonsense is already stored there */
	header->nextLoc->nextLoc = NULL; 
    header->nextLoc->node = malloc(sizeof(struct nodeQueue)); 
	header->nextLoc->node = NULL; 
	header->nextLoc->node = node; 


	
	
} 

void PrintLinkListQ(struct nodeQueue *header){
	/* This moves the header down, so we don't try to print the first location */
	header = header->nextLoc; 
	
	/* Until we can't find anymore, we will loop through */
	while(header != NULL){
		
		/* Prints out whatever word is stored in the node that we are on */
		printf("%s ", header->node->word);
	    
		/* Moves down a node */
		
		header = header->nextLoc;

		
		
		
	}
	
}

/* Frees up the space in the linked list, only for when it's completed its purpose */
void FreeLinkListQ(struct nodeQueue *header){
	
	/* Looks through and clears out everything until it can't find anymore nodes */
 
	while(header != NULL){

		/* Creates a temporary structure, purely so that we can still have the next location, but we will not malloc this, so we will not have to free it */
		  
		if(header->node != NULL){
			free(header->node); 	  
		}
		
	
		struct nodeQueue tempHeader = *header;  
		/* This actually frees the header */
			
		free(header);
		 
		/* Goes to the next location */
		header = tempHeader.nextLoc; 

	}
}
	
	
/* Frees up the space in the linked list, only for when it's completed its purpose */
void FreeLinkList(struct word *header){
	/* Looks through and clears out everything until it can't find anymore nodes */
	while(header != NULL){
		/* Creates a temporary structure, purely so that we can still have the next location, but we will not malloc this, so we will not have to free it */
		struct word tempHeader = *header;  
		/* This actually frees the header */
		free(header);
		/* Goes to the next location */
		header = tempHeader.nextLoc; 
		
	}
	
	
}

/* Frees up the space in the linked list, only for when it's completed its purpose */
void FreeLinkListTLC(struct threeLetterConnections *header){
	/* Looks through and clears out everything until it can't find anymore nodes */
	while(header != NULL){
		/* Creates a temporary structure, purely so that we can still have the next location, but we will not malloc this, so we will not have to free it */
   		struct threeLetterConnections tempHeader = *header;  
		/*So, now I need to free up all of the locations, these include: 
			struct threeLetterConnections *nextLoc; */
	
	 

		 
		/* This actually frees the header */
	
		free(header);
		
		/* Goes to the next location */
		header = tempHeader.nextLoc; 
		
	}
	
	
}

/* Starting at the back of the link list, this will send you on an elevator of the front, where you will return at the beginning */
struct threeLetterConnections *ToLinkListFront(struct threeLetterConnections *back){
	
	
	while(back->prevLoc != NULL){ 
		 
		back = back->prevLoc; 

		
		
	}
	 
	return back; 
	
	
}
/* Frees up the space in the linked list, only for when it's completed its purpose */
void ClearLinkList(struct threeLetterConnections *header){
	 
	 
	 header = header->nextLoc; 
	/* Until the location isn't null */
	while(header != NULL){
		
		/* Create a temp var */
	 
		struct threeLetterConnections tempHeader = *header; 
		free(header); 
		header = tempHeader.nextLoc; 
		
		
	}
	

	
}

/* This prints out everything in the linked list */
void PrintLinkList(struct word *header){
	/* This moves the header down, so we don't try to print the first location */
	header = header->nextLoc; 
	
	/* Until we can't find anymore, we will loop through */
	while(header != NULL){
		
		/* Prints out whatever word is stored in the node that we are on */
		printf("%s ", header->word);
		/* Moves down a node */
		
		header = header->nextLoc;

		
		
		
	}
	
	
	
}
/* This prints out everything in the linked list */
void PrintLinkListTLC(struct threeLetterConnections *header){
	/* This moves the header down, so we don't try to print the first location */
	header = header->nextLoc; 
	
	/* Until we can't find anymore, we will loop through */
	while(header != NULL){
		
		/* Prints out whatever word is stored in the node that we are on */
		printf("%s ", header->word);
		/* Moves down a node */
		
		header = header->nextLoc;

		
		
		
	}
	
	
	
}

/* Add Column <Rightwards> */
void AddRow2DLL(char* input, struct wordConnections *header){
	
	/* I need to loop through here, beginning at the header, and find the next null area, then I'll fill it with "word" */
	/* Then, I'll set the next Row and it's columns previous & next to null */
		

	while(header->nextRow != NULL){
		/* This OK Statement proves that the header has moved over to the next row */
		/* printf("OK\n"); */
		/* This moves the header to the next row */
		header = header->nextRow; 
		
		
	}
	
	/* Sets some space in memory to the next row */
	header->nextRow = malloc(sizeof(struct wordConnections)); 
	/* Sets the next row to NULL, for 2 reasons: a. It won't be filled in with any random jazz, b. We'll be able to find it in the future */
	header->nextRow->nextRow = NULL; 
	/* Then it sets the word in the current header to the input */
	header->nextRow->word = input;
	header->nextRow->isFound = 0; 
	/* After that, it sets space aside for the next Column */
	header->nextRow->nextColumn = malloc(sizeof(struct wordConnections));
	/* Then it sets the next Column to NULL */
	header->nextRow->nextColumn = NULL;
	/* Then, it allocates space for the next */
	header->nextRow->previousColumn = malloc(sizeof(struct wordConnections)); 	
	/*sets the previous Column to NULL */
	header->nextRow->previousColumn = NULL; 
	
	
} 
/* Add Column <Downwards> */
void AddColumn2DLL(char* input, struct wordConnections *header){
	/* So, what I need to do, is I need to Go through all of the rows, then, when the next row will be null, then. I need to flip through the columns, setting the current one to the previous column */
	
	/* First, scroll through the rows, and gets to the last one */
	while(header->nextRow != NULL){
		/* Goes into the next Row's Location */
		header = header->nextRow; 
		
		
	}
	/* Then, scroll through the columns, setting this current location to the previous location */
	while(header->nextColumn != NULL){
		/* Goes into the next Column's Location */
		header = header->nextColumn; 
		
	}
	/* Creates a temporary header, that will act as the previous header. I need to save its location, otherwise it will be lost. */
	struct wordConnections *tempHeader = header;
	/* This allocates a place in space for the next Column */
	header->nextColumn = malloc(sizeof(struct wordConnections));
	/* Then, it sets the value of the next Column to NULL */
	header->nextColumn->nextColumn = NULL; 
	/* The word in this node is set to the input */
	header->nextColumn->word = input;
	header->nextColumn->isFound = 0; 
	/* And the previous Column is set to the header which I created, just recently */
	header->nextColumn->previousColumn = tempHeader; 
	

}
/* Print Out Linked List */
void Print2DLL(struct wordConnections *header){
	
	
	/* Goes through all of the rows and its columns */
	while(header->nextRow != NULL){
		
		header = header->nextRow; 
		
		printf("%s: ", header->word); 
		/* Goes through all of the words in the column */
		while(header->nextColumn != NULL){
			header = header->nextColumn; 
			printf("%s ", header->word); 
			
			
		}
		/* Goes back to the top of the Row */
		while(header->previousColumn != NULL){
			header = header->previousColumn;
			
		}
		printf("\n");
	}
	
} 
/* Copies all of the untis from one linked list into another, for linked lists with conflicting structures */
void CopyLinkListTLC(struct threeLetterConnections *receiverHead, struct word *quarterbackHead){

		/* All we have to do is we have to go from one link list to another, filling them with values */
		/* First, we take each of them to the next location */
		/* until the quarter back is null, we add the quarter backs values into the receivers  */

		while(quarterbackHead->nextLoc != NULL){

			/* Then, we go to the quarterbacks next position */
			quarterbackHead = quarterbackHead->nextLoc; 
			
			/* Then we set the word */
			/* printf("Word: %s\n", quarterbackHead->word);  */
			
			AddLinkListTLC(quarterbackHead->word, receiverHead, quarterbackHead->isFound); 
			
			
			
		}
	
	
}
/* Copies all of the untis from one linked list into another, for linked lists with conflicting structures */
void CopyLinkList(struct word *receiverHead, struct word *quarterbackHead){

		/* All we have to do is we have to go from one link list to another, filling them with values */
		/* First, we take each of them to the next location */
		/* until the quarter back is null, we add the quarter backs values into the receivers  */

		while(quarterbackHead->nextLoc != NULL){

			/* Then, we go to the quarterbacks next position */
			quarterbackHead = quarterbackHead->nextLoc; 
			
			/* Then we set the word */
			AddLinkList(quarterbackHead->word, receiverHead, quarterbackHead->isFound); 
		}
	
	
}

/* Free Linked List */
void Free2DLL(struct wordConnections *header){
	while(header->nextRow != NULL){
		struct wordConnections tempHeader = *header;  
		free(header);
		/* Goes to the next location */
		header = tempHeader.nextRow; 
		
		/* Goes through all of the words in the column */
		while(header->nextColumn != NULL){
			header = header->nextColumn; 
			
			
			
		}
		/* Goes back to the top of the Row */
		while(header->previousColumn != NULL){
			struct wordConnections tempHeader = *header;  
		
			free(header);
			/* Goes to the next location */
			header = tempHeader.previousColumn; 		
			
			
		}

	}
	
} 
/* Yeehaw!! I got the 2D Linked List To Work In Its Entirety
Coming Up: 
1. I have to implement the 2D linked list into the 2D Array
2. And then, I have to fill said linked list for each value
But for now, before I get ahead of myself, let's make an array of 1D Linked Lists



 */
 /*Checks to make sure that there is space in the computer before it mallocs anything*/
void *chkmalloc(size_t sz) {
void *ret = malloc(sz);
/*if the test malloc doesn't work, then it will say it's out of memory */
if(ret == NULL)
	{
	fprintf(stderr, "Out of memory\n");
	exit(EXIT_FAILURE);
	}
return ret;
}

/* This frees all of the words stored in the links list's words storage space */
void FreeCategorysWords(char** wordStorage, int loops){
	/* The Word Placement in the Array that we are freeing the word */
	int freeNum = 0; 
	/* Loops through and frees all the words in word storage */
	for(freeNum = 0; freeNum < loops; freeNum++){
		/* frees the word that they are at */
		free(wordStorage[freeNum]); 
		
		
	}
	/* Frees the entirety of the array */
	free(wordStorage);
	
	
	
}

/* This function will accept a word as an input, and output all of the connecting values */
struct word *linkOutput(char* wordInput, struct wordConnections ***Categories, int numLetters){
	/* Take the hash function of the word */
	struct wordConnections *header = Categories[HashFunction(wordInput[0])/*First Letter*/][SecondHashFunction(wordInput, numLetters)];
	/* Print2DLL(header);  */
	/* Create the linked list into which I put the words */
	struct word *wordOutput = malloc(sizeof(struct word)); 
	wordOutput->nextLoc = NULL; 
	/* Loop through the array until I find the correct word */
	header = header->nextRow;  
	while(strcmp(header->word, wordInput) != 0){
	
		header = header->nextRow; 
		
	}

	while(header->nextColumn != NULL){
		header = header->nextColumn; 
		AddLinkList(header->word, wordOutput, header->isFound); 	
	}
	/* Let's make a find method, 'cause quite frankly, I'm going to need that later anyways... */	
	/* Print the Linked List */
	/* Return the Linked List */
	return wordOutput; 
	
	
}

/* I need to make the link list's function, Dequeue, whose sole purpose is to: remove the first  */
struct word *Dequeue(struct word *header){
	
	
	/* Create a temporary variable that stores the header->next */
	struct word *tempFirstValue = header->nextLoc; 
	/* So, first, just set the header next, to header next, header next*/

	header->nextLoc = header->nextLoc->nextLoc; 
	/* Then, free the temporary value */
	free(tempFirstValue); 
	return header; 
	
	
	
	
}
/*This removes the first node added in the node dequeue N N N N N --> X N N N N -> N N N N*/
struct nodeQueue *NodeDequeue(struct nodeQueue *header){
	/*Creates a temporary variable that stores the first node, we do this, in order to free it */
	struct nodeQueue *tempFirstValue = header->nextLoc;
	header->nextLoc = header->nextLoc->nextLoc; 
	free(tempFirstValue); 
	return header; 
	


}

/* gcc -O -Wall -Wextra -ansi -o flwp FLWP.c */
/* I'm using the pathfinding algorithm, Breadth-First Search, which essentially will go through everything */
struct word *Pathfind(char* start, char* goal, struct wordConnections ***Categories, int numLetters){
/* So, first I have to make a queue that stores all the words that need to be chained*/
/* Then, I have to make the link list that stores all of the words which the queue's nodes conencts to. Queue Connection */
/* Then, I have to put the starting point into the queue */
/* Then, I have to fill the queue with queue connections */
/* Then, I have to free the queue */
/* This is a boolean which ends the program when the word is found */

/*If the words are the same*/
if(strcmp(start,goal) == 0){
	printf("Those are the same word"); 
	return 0; 
}
/* If the goal has ben found */
int goalFound = 0; 
/*This is the queue that will store all of the words*/ 
struct word *Queue = malloc(sizeof(struct word));  
Queue->nextLoc = NULL;
/* It adds the first word into the Queue */
AddLinkList(start, Queue, 1); 
/*Create the Queue of Nodes that will store themselves as the previous connection*/
struct nodeQueue *NodeQueue = malloc(sizeof(struct nodeQueue)); 
NodeQueue->nextLoc = NULL; 
NodeQueue->node = NULL;
/*Rah Rah!! Free Node Queue From The Oppressive Chains of This Program!!*/ 
/*This makes it so that we are actually able to free nodeQueue*/ 
struct nodeQueue *NodeQueueHeader = NodeQueue; 
/*This makes it so that we are actually able to free Queue*/
struct word *QueueHeader = Queue;   
/*This makes it so that we are actually able to free all of the nodes in the Current Words Connections. This will stores all of CWC's headers, because they are never actually freed. Not Until Today */

/*How many times the word has programmd*/
int timesRun = 0; 

/*Happens until the word isn't found*/
while(goalFound == 0){
	/*The Queue goes into the next location*/
	Queue = Queue->nextLoc;  
	/* We will put the current word's connections. For example, we start with pens. We will put "tens, dens, pins, etc." here */
	struct threeLetterConnections *CurrentWordConnections = malloc(sizeof(struct threeLetterConnections));
	CurrentWordConnections->nextLoc = NULL; 
	CurrentWordConnections->prevLoc = NULL; 
	CurrentWordConnections->prevConnection = NULL; 
	/*This adds the Current Words Connection Header into the CWCHeader linked list so that it can be freed a plus tard */ 
	
	  
	AddLinkListTLC(Queue->word, CurrentWordConnections, 1); 
	CopyLinkListTLC(CurrentWordConnections, linkOutput(Queue->word, Categories, numLetters)); 
	/*If it's not the first time through, then it should be able to throw the thing onto all of the TLC words*/
	
	if(timesRun != 0){
		ImplementNodeQueue(NodeQueue, CurrentWordConnections); 
	}
		
	
	/* Then, I have to go through the that link list and add it to the queue */
	 while(CurrentWordConnections->nextLoc != NULL){
	 	CurrentWordConnections = CurrentWordConnections->nextLoc;
		
		
	
		 /*It'll mention it if it finds the word*/
		if(strcmp(goal, CurrentWordConnections->word) == 0){
			 goalFound = 1; 
			 WordFound(start, NodeQueue, CurrentWordConnections, 1); 
		  
	 
		}


		
		 
		 /*If the word hasn't already been used*/		
		 if(CurrentWordConnections->isFound == 0){
		 
		 	/*We add the word and the node into the queue*/
		 
			 
			AddLinkList(CurrentWordConnections->word, Queue, 1);
			AddLinkListQ(CurrentWordConnections, NodeQueue); 
		
	
			RemoveWord(CurrentWordConnections->word, Categories, numLetters);  
			 
	 	}

	 	timesRun++;

	 	
	}


	/*If it runs out of the words that it is able to find, and can no longer fully connect */
	 if(NodeQueue->nextLoc == NULL){
	 		printf("There is no way for this to fully connect, here is how far it got \n"); 
	
			/*Print out the linked list (backwards)*/ 
			/* ReverseLinkListTLC(word); */ 
			WordFound(start, NodeQueue, CurrentWordConnections, 0);  
			FreeLinkListQ(NodeQueueHeader);
			FreeLinkList(QueueHeader); 
			
			return 0; 	
	 }
	  	NodeQueue = NodeQueue->nextLoc; 
	  
		
	
	
}

	


FreeLinkListQ(NodeQueueHeader);
FreeLinkList(QueueHeader); 
 
return 0;


}



/* method that will make a word become found in all of it's locations in the array of 2D linked lists */
void RemoveWord(char* removalWord, struct wordConnections ***Categories, int numLetters){
	/*It's far easier to try to make a variable that says a variable shouldn't be found, rather than actually removing a word.*/
	/*So, for example, if we have a word test inputted. We want to take it, and in all of it's locations in the hash map, we want to set IsFound to 1 */
	
	/*1. We have to go into the Categories, and every point, set it equal to 1. */
	/* Link List that Captures all of the words that connect to 'test'*/
 	struct word *Connections = malloc(sizeof(struct word)); 
 	Connections->nextLoc = NULL; 
 	CopyLinkList(Connections, linkOutput(removalWord, Categories, numLetters)); 

	while(Connections->nextLoc != NULL){
	 	Connections = Connections->nextLoc; 
	 	/*This is the category of the word that will be found*/
		char* columnTop = Connections->word; 
	 
	 	/*So, in each one of these words, we need a new link list, that will go through and set isFound on removalWord to 1 */
	 	
	 	
		 struct wordConnections *header = Categories[HashFunction(columnTop[0])/*First Letter*/][SecondHashFunction(columnTop, numLetters)]->nextRow;
		/*It searches through all elements of the row to find exactly the word it's looking for*/
		while(strcmp(header->word, columnTop) != 0 && header->nextRow != NULL){
			
		 	header = header->nextRow; 
			  
		 
		 
		}
		/*It searches through all of the columns in the row so now it can find removalWord */
		while(strcmp(header->word, removalWord) != 0 && header->nextColumn != NULL){
			header = header->nextColumn; 
			
		}
		/*When it finds the word in that specific linked list*/
		header->isFound = 1; 
		
	}
 
	FreeLinkList(Connections); 
}
/* This implements the Node Queue (word which previously connected to) to the Three Letter Connections */
void ImplementNodeQueue(struct nodeQueue *NodeQueue, struct threeLetterConnections *CurrentWordConnections){
	 
	 
	
	while(CurrentWordConnections->nextLoc != NULL){
  		CurrentWordConnections = CurrentWordConnections->nextLoc; 	
		CurrentWordConnections->prevConnection = NodeQueue; 
		
		
	}
	 


}
/* This prints out the words in reverse */
void ReverseLinkListTLC(struct threeLetterConnections *header){
	/* First, take it to the end  */
	while(header->nextLoc != NULL){
		header = header->nextLoc; 
	
	}
	/* Then, bring it to the beginning printing out the results  */
	while(header->prevLoc != NULL){
		/*Kind of a Gate Statement, we don't want an arrow on the very last one, because if it doesn't connect all the way, there will just be an arrow connecting to nothing''*/ 
		printf("%s", header->word); 
		header = header->prevLoc; 
		if(header->prevLoc != NULL){
			printf("->"); 
		}
		
	}
}

/*
Now to reverse traverse through the words to figure out the most direct pathway.
So, I'm gonna need a dereferenced pointer that will store the previous connection 
Each time that it goes through and gets added into the queue, the previous point will be added, likely is going to need to have a temporary that stores it before it frees it 
Then, after the word is found, we will loop through & print all of the previous connections until we reach NULL (eventually, we'll make a linked list to print them out in the correct order)



*/
void WordFound(char* start, struct nodeQueue *NodeQueue, struct threeLetterConnections *CurrentWordConnections, int isComplete){
			/*Counts how many times it connects */
			int connections = 1; 
			/*This is what is going to go down the list */
			struct nodeQueue *header = NodeQueue; 
			/* Create the temporary linked list that will store all of the words */
			struct threeLetterConnections *word = malloc(sizeof(struct threeLetterConnections)); 
			word->nextLoc = NULL;  
			word->prevLoc = NULL; 
			/*The previous three letter connection (TLC)*/
			word->prevConnection = NULL; 
		
			while(header != NULL){
				if(header->node != NULL){
					/* printf("%s->", header->node->word);  */
					/*Goal: Create a linked list that we can travel to the end to so that we can print it out backwards*/
					/* Add the header->node->word to the brand new linked list  */
					AddLinkListTLC(header->node->word, word, 1); 
					header = header->node->prevConnection;
				 	connections++; 
				}
				/* if the next node is null, then we gotta get out of here before the whole thing crashes, it'll print out the word  */
				else{
					break;
				}
					
				
			}
	
			 
			AddLinkListTLC(start, word, 1); 
			/*Print out the linked list (backwards)*/
			ReverseLinkListTLC(word);  
			/*If it connects to the goal, it will print it out*/
			if(isComplete == 1){
				printf("->%s", CurrentWordConnections->word); 
			}
			printf("\nThere are %d connections", connections); 
			/* free the TLC linked list  */
			free(word);
}
