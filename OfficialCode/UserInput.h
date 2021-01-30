#ifndef seenInput
#define seenInput

#include "WordLinkedList.h"
#include "2DLinkedList.h"
/*Necessary commands: 
\n Start Game
\n*/ 
/*This takes the input, and returns a character pointer 
@param size --> The size is important to know because fgets automatically puts a \n at the end of the each word, so we need to know exactly where to replace it with a \0
@return --> Returns the user input w/out the \0*/ 
char* Take_Input(int size);
/*This takes the input w/ an unknown size
@return --> Returns the user input*/ 
char* Take_Input_NoSize(); 
/*This asks the user what they'd like the difficulty to be*/ 
enum Difficulty ChooseDifficulty(); 
/*This takes an input, and interprets what to do with it
@param userConnections --> The list of words the user has inputted
@prevWord --> The previous word added 
@param input --> The input to be interpreted
@return case 1 --> If it retuns a word, just a word, then that word will be added to the list
@return case 2 --> If it returns r, for remove, then it removes a word from the list
@return case 3 --> If it returns f, for finish, then it the game ends
@return case 4 --> If it returns u, for undo, then it undoes the previous turn (easy for add, hard for remove)*/ 
char* Interpret_Input(struct word* userConnections, char* prevWord, char* input); 
/*This method checks the input to make sure that it is valid
@param prevWord --> The previous word, the word it is checking the current characters against
@param currWord --> The word who is being judged
@return --> 1) Valid   0) Invalid*/ 
int Check_Input(char* prevWord, const char* currWord, struct wordConnections ***HashMap); 


/*This Converts a word to lowercase*/ 
char* toLowerCase(char* input); 

/*This extracts a substring from a string
@param word --> The string to be extracted from
@param start --> The first letter to be chosen
@param end --> The last letter to be chosen
@param replace --> 1) Frees the word that is inputted 0) Does not free inputted word
@example --> ("Butterfly", 0, 5, 0). Butter
@note --> Does give a '\0'*/ 
char* substr(char* word, int start, int end, int replace); 

/*This is a safer variant of the C-regular strncat*/
int safeStrcat(char** dest, char* src, int destLength, int buff, int start); 
   
#endif



 

