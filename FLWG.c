/***********************************
Name: Jordan Driscoll
Date: July 14, 2019
Description: This allows a peaceful game of the four letter word game

gcc -O -ansi -Wextra -Wall -o flwg FLWG.c

Version Alpha: The game at this point works, it just needs a few tweaks to make it perfect 
***********************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct individualWord{
	/*The Letters of the word*/
	char *word; 
	/*The next location*/
	struct individualWord *nextLocation;
	
};
/*Gets the user input at the beginning of the game, returns the amount of letters the game will begin with*/
int userInput(); 
/*This will lowercase the letters, so that the user can put in capital and lowercase letters*/
char* lowercase(char* word, int letters); 
/*Checks if the player's input is valid*/
int validInput(struct individualWord *header, int numLetters, char* newWord, char* previousWord, int isRobotTurn);
/*This will create the original linked list that steals from the necessary file*/
void createLinkList(FILE *wordFile, struct individualWord *wordList, int numLetters); 
/* Adds something to a linked list */
void wordListAdd(char* wordInsert, struct individualWord *header);
/* Removes a node from linked list */
void wordListRemove(char* word, struct individualWord *header);
/*Goes up and allows anything stored up to be free*/
void freeWordList(struct individualWord *header);
/*The Robot Is Allowed to Begin The Game*/
char* RandomizedBeginning(int letters, FILE *wordFile, int maxWords, struct individualWord *wordList, int isRobotsTurn);
/*Gets the computer to make a move*/
char* ComputerTurn(char* word, struct individualWord *header, int numLetters, int isFirstTurn, int isRobotsTurn); 
/*A method that allows the player to take a turn, it returns the word that the player chose*/\
char* PlayerTurn(int isRobotsTurn, char* newWord, char* currentWord, int correctLength, int isValid, int letters, struct individualWord *wordList);
int main(){
		/* Declares hte time seed */
		srand(time(0));
		/*Allows the player to quit the game*/
		int quit = 0; 
		/* Determines whose turn it is */
		int isRobotsTurn = rand() % 2; 
		/* The Files that it can open */
		char* files[3] = {"WordDocuments/Two_Letters.txt", "WordDocuments/Three_Letters.txt", "WordDocuments/Four_Letters.txt"};
		/* Max Number Of Words There Are */
		int maxWords[] = {29, 589, 2232};
		/* The Amount of letters the game will be played with, for example, the three letter word game */
		int letters = 0; 
		/*The variable that determines if the a players word is valid*/
		int isValid = 0; 
		/*Boolean that determines if they are going to use if the file*/
		int useFile = 0; 
		/*Makes sure that the user inputs a word with the correct amount of letters*/
		int correctLength = 0; 
		/*Determines if the game is paused or quit out of*/
		int isPaused = 0; 
		/*The file where we the store the words at*/
		FILE *wordFile;
		/*The file that saves the progress*/
		FILE *progressFile = fopen("flwg/Saves/progress.txt", "r"); 
		
		/* So, what it will do is it will ask you how many letters would you like to use, then it will randomly either start off the word, or think of another word */
		if(progressFile == NULL){
			printf("Wecome To The Four Letter Word Game!\n");
			progressFile = fopen("flwg/Saves/progress.txt", "w");  
			letters = userInput(); 
			fputc(letters + 48, progressFile); 
			fputc(isRobotsTurn + 48, progressFile); 
		}
		else{
			printf("It appears you already have a file open, would you like to use it?\n"); 
			/*Determines if the input is valid*/
			int validInput = 0; 
			
			while(validInput == 0){
				/*Take input*/
				char input[10]; 
				/*It asks the user how many letters you would like to begin with, or to type help if necessary*/
				/*1. Create a buffer so that the user cannot spam input*/
				char buffer[255]; 
			 
				do{
				/*2. Scans the string into the buffer*/
					scanf("%225s", buffer); 
				}
				/*3. Make sure the string is the right size*/
				while((signed int)strlen(buffer) > 10); 
				/*4. Copy buffer into input*/
				strcpy(input, buffer); 
				char* lowercaseWord = lowercase(input, strlen(input)); 
				strcpy(input, lowercaseWord);
				free(lowercaseWord); 
				/*if the user agrees to it*/
				if(strcmp("y", input) == 0 || strcmp("yes", input) == 0){
					useFile = 1; 
					validInput = 1; 
					
				}
				/*If the user does not agree to it*/
				else if(strcmp("n", input) == 0 || strcmp("no", input) == 0){
					validInput = 1; 
				}
				else{
					printf("Sorry, but I don't know what that means. Try again, if you'd be so kind.\n");
					
				}
				
			}
			
			/*if input is yes, open the file, and go through each letter go through each letter*/
			if(useFile == 1){
				letters = fgetc(progressFile) - 48; 
				 
			}
			/*if the user decides they would not like to resume their previous game*/
			if(useFile == 0){
				printf("Wecome To The Four Letter Word Game!\n");
				progressFile = fopen("flwg/Saves/progress.txt", "w");  
				letters = userInput(); 
				fputc(letters + 48, progressFile); 
				fputc(isRobotsTurn + 48, progressFile);
				
				
			}
	
				/*if letters is 0, 
					count the number of letters, 
						and then when it reaches the \n, set that to letters
				set tempWord to the letter
				if it's a \n
					add an \0 to it 
				then, remove that word from word list
				then close the file
			if the inut is no, do the Welcome to the FLWG, and then letters =, you know
			open up the file with the ability to append to it 
				*/
			
		}
		/*If the user decides to type in quit, it will return letters as -1, thus closing the file*/
		if(letters == -1){
			quit = 1; 
			printf("What a shame!");
			letters = 4; 
			
		}
		/*The word the game is currently on*/
		char currentWord[letters + 2];
		/*The New Word that the user will define on input*/
		char newWord[letters + 2]; 
		/*You will answer, based on that answer he will open a file from an array*/
		wordFile = fopen(files[letters - 2], "r");
		if(wordFile == NULL){
			printf("Shucks! File Could Not Be Found");
			
			return 0; 

		}
		
		
		/*If he generates 1, he will say, "You can begin"*/

		/*Create space for the first node*/
		struct individualWord *wordList = malloc(sizeof(struct individualWord));
		/*Puts the next location to null, but stores the next location*/
		wordList->nextLocation = NULL; 
		/*Creates the linked list, with all of the values*/
		createLinkList(wordFile, wordList, letters);  
		/*Closes the file, since the whole thing has already been read through*/
		fclose(wordFile);
		/*Here is where I have to go through the list and remove things from the word list*/
		if(useFile == 1){
			/*The letter that we're currently on*/
			char c; 
			char word[6];
			int parse = 0; 
			/*Captures the correct amount of letters */
			
			
			while((c = fgetc(progressFile))!= EOF){
				/*The First One Will Have To Learn Whether the Robot Or Player Started The Game*/
				/*if the robot did*/
				if(c == '1'){
					isRobotsTurn = 0; 
					
				}
				/*If the player started the game*/
				else if(c == '0'){
					isRobotsTurn = 0; 
					
				}

				else if(c > '4') {
					/*Sets the word's letter to the parse that it's on*/
					word[parse] = c; 
					
					parse++;
				}
				/*once it reaches the end of the word*/
				if(c == '\n'){
					/*if it has yet to figure out the number of letters, it will set the number of letters to letters*/
					isRobotsTurn++; 
					word[letters] = '\n'; 
					word[letters + 1] = '\0';  
					printf("%s", word);
					wordListRemove(word, wordList); 
					strcpy(currentWord, word); 
					parse = 0; 
					
				}
					
			}
			
			fclose(progressFile); 
			progressFile = fopen("flwg/Saves//progress.txt", "a"); 
		}
		
		
		wordFile = fopen(files[letters - 2], "r");
		printf("%s->%s", RandomizedBeginning(letters, wordFile, maxWords[letters - 2], wordList, 1),  RandomizedBeginning(letters, wordFile, maxWords[letters - 2], wordList, 1)); 

		/*If the robot gets tot start, this will be bypassed if a game is already started*/
		if(isRobotsTurn == 1){
			/*the robot chooses the first word*/
			strcpy(currentWord, RandomizedBeginning(letters, wordFile, maxWords[letters - 2], wordList, isRobotsTurn)); 
			
			if(strcmp("q", currentWord) != 0 && strcmp("p", currentWord) != 0){
				wordListRemove(currentWord, wordList);
				fputs(currentWord, progressFile);
			}
			isRobotsTurn++;
		}
		while(wordList->nextLocation != NULL && quit == 0 && strcmp("q", currentWord) != 0) {
		
			/* if it's the robots turn	*/
			if(isRobotsTurn % 2 == 1){
			
				/* Robot takes his turn */
				strcpy(currentWord,  ComputerTurn(currentWord, wordList, letters, isRobotsTurn, isRobotsTurn));
				

			}
			/*if it's the player's turn*/
			else{
				strcpy(currentWord, PlayerTurn(isRobotsTurn, newWord, currentWord, correctLength, isValid, letters, wordList)); 
	
				/*if it returns quit, then the game is over*/
				if(strcmp(currentWord, "q") == 0){
				
					quit = 1; 
					
					
				}
				if(strcmp(currentWord, "p") == 0){
					quit = 1; 
					isPaused = 1; 
				}
				
				
				
			}
			
			/* Removes the word off of the list, thus making it no longer usable, if the word is not gg, it should work */
			if(isRobotsTurn != 0 && quit != 1 && strcmp("q", currentWord) != 0){
			
				wordListRemove(currentWord, wordList);
				/*puts the wrod into the file*/
				fputs(currentWord, progressFile); 
			}
			
				
				
			
			
			/* Changes whose turn it is*/
			isRobotsTurn ++; 
			
			
			
			
		}
		
		free(currentWord); 
		
		
		
		
		
		
		
		
		
		
		/*Goes through and frees everything, belongs at the end*/
		struct individualWord *temp = wordList->nextLocation; 

		while(temp != NULL){
			
			
			temp = temp->nextLocation; 
		}
		freeWordList(wordList);
	
		
		fclose(wordFile);
		
		fclose(progressFile); 
		/*if it wasn't paused the progress file will be deleted */
		if(isPaused == 0){
			if(remove("flwg/Saves/progress.txt") == 0){
				printf("Sucessfully Sucessful!\n");
			
			}
			else{
				printf("Pathetically Pathetic\n");
				perror("Reason");
			}
		
		
		/* fclose(progressFile);  */
	
		}
		int prompt = 'a';
		printf("\nType 'q' to end program\n");
		for(prompt = 'a'; prompt != 'q' && prompt != EOF; prompt = getchar()){
			
		
		}
		/* printf("prompt = %c...", prompt); */
		return 0; 
		
	
	
}
/*This Method Randomizes The Word Which the Robot Chooses*/
char* RandomizedBeginning(int letters, FILE *wordFile, int maxWords, struct individualWord *wordList, int isRobotsTurn){
	 
	/*Randomizes the words that it chooses*/
	int randNumber; 
	/* Parses through all of the words */
	int wordParser = 0;
	/* Parses through all of the letters */
	int letterParser = 0;
	/*The actual letter which it's parsing*/
	char letter; 
	/*The current word that it's on, the + 4 is because originally we subtract 2 from it for the array, then we need the \n and \0*/
	char* word = malloc (sizeof (char) * (letters + 2)); 
	/*makes sure that the word is valid */
	char wordTester[letters + 2]; 
	wordTester[0] = '!'; 
	while(wordTester[0] == '!'){
		randNumber = rand() % maxWords; 
		/*Goes through the file and steals the word that that is looking for*/
		
		while((letter = fgetc(wordFile)) != EOF && wordParser < randNumber){
			
			
			
			/*Sets the letter to the placement in the word*/
			word[letterParser] = letter; 
			/*goes through each individual letter*/
			letterParser++; 
			/*each time it reaches a new word, it'll reset letter parser and put word parser up 1*/
			if(letter == '\n'){
				/* Converts array into string */
				word[letters + 1] = '\0';  
			
				letterParser = 0; 
				
				wordParser++; 
					
			}
			
			/* It stores all the words */
		}
		/*The Computer Tests if it's actually a word*/
	
		strcpy(wordTester, ComputerTurn(word, wordList, letters, 1, isRobotsTurn));
	
		strcpy(word, wordTester);
	}
	/*All of this will happen while the word is not a valid word, i.e., you can't go anywhere from it (for example, ogre)*/
	
	 printf("%s\n", word);
	/*Then, it returns that string*/
	
	return word;
	
	
}
/*Allows the computer to make a valid move*/
char* ComputerTurn(char* word, struct individualWord *header, int numLetters, int isFirstTurn, int isRobotsTurn){
	
	
	
	char pressEnter[30]; 
	int letterParser = 0; 
	int alignedLetters = 0; 

	/*Reads through the linked list*/
	while(header->nextLocation != NULL){
		
		alignedLetters = 0; 
		header = header->nextLocation; 
		/*If the words have 3 letters in common*/
		
		/*Loop through it letters times*/
		for(letterParser = 0; letterParser < numLetters; letterParser++){

			if(word[letterParser] == header->word[letterParser]){
				
				alignedLetters++; 
				
			}
		
			/*if the word works, then it should be mighty alright, 2 for the \n*/
			if(alignedLetters == (numLetters - 1)){
				
				return header->word; 
				
			}
			
			
			
			
		}
		/*success goes up 1 when the letters are the same*/
		/*if letters == success at the end, it will print and return that word*/
		
	}
	
	/*When it finds a word that has 3 letters in common, it'll return that word*/
	/*If it reaches the end without finding a word, you biggy jazzed*/
	 
	if(isFirstTurn == 1 || isFirstTurn == 0){
	
		return "!";
	}
	if(isRobotsTurn % 2 == 1){
		printf("It seems I have been defeated");
	}
	else{
		printf("Ha ha! I am victorious!");
	}
	pressEnter[0] = ' ';
	pressEnter[1] = ' '; 
	while(pressEnter[0] != 'g' || pressEnter[1] != 'g'){
		printf("\nType gg to close the program\n");
		scanf("%s", pressEnter);
	}
	
	printf("Shutting Down...\n");
	/*If it is the first turn, and the word is illegal, then it will send an ! point saying, do it again*/

	
	return "q"; 
	
	
}

/*Goes through all of the checks of the player*/

/*Adds a node to the word list, will only run before the game begins*/
void wordListAdd(char* wordInsert, struct individualWord *header){
	/*This goes through and gets to the very end of the linked list*/
	while(header->nextLocation != NULL){
		header = header->nextLocation; 
		
	}
	/*Once it gets to the end of the linked list, it goes ahead and creates a new location in memory*/
	header->nextLocation = malloc(sizeof(struct individualWord));
	/*Then, it sets the next location*/
	header->nextLocation->nextLocation = NULL;
	/*Then, the location gets it's word set*/
	header->nextLocation->word = wordInsert; 
/*
	loops until the very end of the linked list
	set the pointer to something
	set the word to something from the list
	then make the next node's pointer null
	 */
	
}
/*Removes a node from the word list, thus keeping the game moving along*/
void wordListRemove(char* word, struct individualWord *header){
	

	/*Loops through list's nodes, looking for the data in the next location*/
	while(strcmp(header->nextLocation->word, word) != 0){
		/*When the words are not equal, it will go through the next location*/
		header = header->nextLocation;
		/* Just in case it's the last one */
		if(header->nextLocation == NULL){
		 
			freeWordList(header);
		
		}
	}
	
	struct individualWord *temp = header->nextLocation;
	 
	
	
	header->nextLocation = temp->nextLocation; 
	free(temp);
	
	
	
	
} 
void freeWordList(struct individualWord *header){
	/*This goes through each of them, and frees all of them*/
	while(header != NULL){
		struct individualWord tempHeader = *header; 
		free(header);
		header = tempHeader.nextLocation; 
		
	}

	
}
/*This creates a link list that stores all of the files, it'll only run once at the very beginning*/
void createLinkList(FILE *wordFile, struct individualWord *wordList, int numLetters){
	/*So, what it has to do is it has to read from the file, then, set each node to every word in the file*/
	/*The letter from the file that is being read*/
	char letter;
	/*The word being added*/
	 
	/*the number of letters we've parced through in teh file*/
	int lettersParsed = 0; 
	char *word = malloc(sizeof(char) * (numLetters + 2));
	/*while letter is not yet to the end of the file*/
	while((letter = fgetc(wordFile)) != EOF){
		
		/*This sets each letter to a point in the array*/
		word[lettersParsed % (numLetters + 1)] = letter;  
		
		/*if we reached the end of the word*/
		if(letter == '\n'){
			
			/*then, we throw on a \0 on the file character*/
			word[numLetters + 1] = '\0';
		
		
			wordListAdd(word, wordList); 
			word = malloc(sizeof(char) * (numLetters + 2));
			/*then, we need to add that word into the list*/
		}
	lettersParsed++; 	
	}
	
	
}
/*Makes sure that the player types valid input*/
int validInput(struct individualWord *header, int numLetters, char* newWord, char* currentWord, int isRobotTurn){
	/*loops through all of the word's letters*/
	int parse = 0;
	/*The numbers of letters that are aligned*/
	int alignedLetters = 0; 
	/*Determines if it's been found in the loop */
	int isFound = 0; 
	/*Then, it will go through the link list and make sure it's there*/
	while(header->nextLocation != NULL){
		if(strcmp(newWord, header->nextLocation->word) == 0){
			isFound = 1; 
			
		}
		header = header->nextLocation; 
		
		
	}
	/*if it reached the end of the list, and the word hasn't been found, then it will return saying such*/
	if(isFound == 0){
		return 2; 
		
	}
	/*Then, it will make sure that 3 of its letters align*/

	/*This cannot happen on the first word, as there is no word to be compared to*/
	if(isRobotTurn !=0){
		/*it will loop through both words in a while loop, and if numLetters - 1 != the added number, then it'll return 3*/
		for(parse = 0; parse < numLetters; parse++){
			if(newWord[parse] == currentWord[parse]){
				alignedLetters++; 
				
			}
			
		}
	
	
		/*If the current numbers aren't aligned*/
		if(alignedLetters < numLetters - 1){
			printf("%d letters must align with the previous word\n", numLetters - 1); 
			return 3; 
			
		}
	
		/*if they outputted the same word as the current word*/
		else if(alignedLetters == numLetters){
			printf("You mustn't print the same word\n");
			return 4; 
			
		}
	}
	
	return 1; 
	
}
char* PlayerTurn(int isRobotsTurn, char* newWord, char* currentWord, int correctLength, int isValid, int letters, struct individualWord *wordList){
	/* 1. Create a buffer string that is of size 255 */
	char buffer[255]; 
	/* the first word that the computer converts the first word into */
	char firstWord[letters + 2]; 
	
	
	
	do{
		
		
		/*Won't print the word on the first turn*/
		if(isRobotsTurn != 0){
			printf("%s", currentWord);
		}
		isValid = 0; 
		/*Resets the 4th and 5th letters so that the error messages can be correct */
		newWord[letters - 1] = 0; 
		newWord[letters] = 0; 
		/* If the length of the word is correct */
		correctLength = 0;
		printf("Take yo turn\n"); 
 
		do{
			/* 2. Scanf String Into Buffer */
			scanf("%255s", buffer); 
			
		}
		/* 3. Check to make sure the string is the right size */
		while((signed int)strlen(buffer) > 10); 
		/* Copy from the buffer into the new word */
		strcpy(newWord, buffer);
		
	

		/*if the player wants to end, it'll break out of the while loop, and set is quit to false*/
		if((newWord[0] == 'G' || newWord[0] == 'g') && (newWord[1] == 'g' || newWord[1] == 'G')){
			char* finalWord = ComputerTurn(currentWord, wordList, letters, 0, isRobotsTurn); 
			if(strcmp("!", finalWord) != 0){
				printf("Such a shame, another word you could have done is: %s", finalWord); 
			}
			else{
				printf("Looks Like I Have Defeated You!");
				
			}
			return "q";
			
		}
		/*if the player wants the game to pause, so that they can return to it later, they can just say so*/
		if(((newWord[0] == 'p') || (newWord[0] == 'P')) && ((newWord[1] == 'a') || (newWord[1] == 'A')) && ((newWord[2] == 'u') || (newWord[2] == 'U')) && ((newWord[3] == 's') || (newWord[3] == 'S')) && ((newWord[4] == 'e') || (newWord[4] == 'E'))){
			printf("Game Paused");
			return "p"; 
			
		}
		/*if that final letter is a letter*/
	
		if((int)newWord[letters] ==  0){
			/*if the word is too short*/ 
			if(((int)newWord[letters - 1] >= 97 && (int)newWord[letters - 1] <= 122) || ((int)newWord[letters - 1] >= 65 && (int)newWord[letters - 1] <= 90)){
				char* lowercaseWord = lowercase(newWord, letters); 
		
				strcpy(newWord, lowercaseWord);
				free(lowercaseWord); 
				/*Makes it so that the code can keep going, no longer enslaved by the while loop*/
				correctLength = 1; 
				/*Adds a new line to the end of the word, so that it can be comapared with the rest of the file*/
				newWord[letters] = '\n';
				/* Makes it a string */
				newWord[letters + 1] = '\0';
			
				/*It'll check if it's valid*/

				isValid = validInput(wordList, letters, newWord, currentWord, isRobotsTurn);
				
				if(isRobotsTurn == 0 && isValid == 1){
					/*removes the previous word from the list, so that the robot can't just do the ole no changeroni*/
					wordListRemove(newWord, wordList);
					strcpy(firstWord, ComputerTurn(newWord, wordList, letters, isRobotsTurn, isRobotsTurn)); 
				
					
					/*if the robot can find a word on the first try, then it is valid, otherwise, it is not*/
					if(strcmp("!", ComputerTurn(newWord, wordList, letters, isRobotsTurn, isRobotsTurn)) != 0){
					
						isValid = 1; 
						
					}
					else{
						printf("The Word Must Be Mendable\n");
						isValid = 0;
							
					}
				}
			
				/*It'll set current word to the new word, only if the new word is valid*/
				if(isValid == 1){
					/*Sets current word to new word*/
					strcpy(currentWord, newWord);
				}
			
			}
			/*If there aren't enough letters in the word*/
			else{
				printf("That is less than %d letters\n", letters);
				
			}
		}
		
		/*if there are too many letters in the word*/
		else{
			printf("That exceeds %d letters\n", letters);
		}
		/*if word can't be found*/
		 if(isValid == 2){
			 printf("Invalid Word.\n");
			 
		 }
			
		}
		/*makes sure that the word is corrent not just in length, but in letterage*/
		while(correctLength == 0 || isValid != 1); 
		/*Returns the current Word*/
		return currentWord; 
}
/*This will lowercase the letters, so that the user can put in capital and lowercase letters*/
char* lowercase(char* word, int letters){
	
	char* lowercaseWord = malloc(sizeof(char) * letters + 2);
	/*This parses through each word*/
	int parser = 0; 
	/*Until it reaches the null terminator*/
	for(parser = 0; parser < letters; parser++){
			if(word[parser] <= 90 && word[parser] >= 65){
				lowercaseWord[parser] = word[parser] + 32; 
				
			} 
			else{
				lowercaseWord[parser] = word[parser]; 
				
			}
			
	}
	lowercaseWord[letters] = '\0'; 
	 
	
	return lowercaseWord; 
		
}

int userInput(){
 
	char input[10]; 
	/*It asks the user how many letters you would like to begin with, or to type help if necessary*/
	/*1. Create a buffer so that the user cannot spam input*/
	char buffer[255]; 
	printf("Type the amount of letters you would like to begin with, or help if necessary.\n"); 
	do{
	/*2. Scans the string into the buffer*/
	scanf("%225s", buffer); 
	}
	/*3. Make sure the string is the right size*/
	while((signed int)strlen(buffer) > 10); 
	/*4. Copy buffer into input*/
	strcpy(input, buffer); 
	char* lowercaseWord = lowercase(input, strlen(input)); 
	strcpy(input, lowercaseWord);
	free(lowercaseWord); 
	/*if the user asks for help, */
	if(strcmp(input, "help") == 0 ){
		printf("\nWelcome to the four letter word game!\n\nHow it works, either you or I will start off by outputting a word with four letters.\nOnce my turn is taken, you will have to change the word\n\nIn order to do so, you will take only one of the three letters, and change it, thus creating a new word.\n\nHence the name, you may neither remove nor add additional letters.\n\nSand:\nAcceptable Words: Land, Send, Said, Sane.\nUnacceptable Words: And, Seed, Sandy.\n\nProhibited Words:\n-Proper Nouns (Sears, John)\n-Abbreviations (Misc., tmrw)\n-Acronyms (PETA, NATO)\n-Slang (bae, yeet)\n\nThe game also works with three letter words, and two letter words!\n\n\nIf you cannot think of another word to type, type \"gg\".\n\nTo pause the game for later, type Pause\n"); 
		/*Yup.. pretty much just restarts the method lol*/
		return userInput(); 
	}
	/*If the User Outputs 4*/
	else if(strcmp("four", input) == 0 || strcmp("4", input) == 0 ){
		
		return 4; 
	}
	/*If the user outputs 3*/
	else if(strcmp("three", input) == 0 || strcmp("3", input) == 0 ){
		
		return 3; 
	}
	/*if the user outputs 2*/
	else if(strcmp("two", input) == 0 || strcmp("2", input) == 0 ){
		return 2; 
	}
	else if(strcmp("quit", input) == 0){
		return -1; 
		
	}
	else{
		printf("\nWord Must Be Between 2 to 4 Letters\n"); 
		return userInput(); 
	}
	/*All of the potentials that it could be*/
	return 4; 
}


	

