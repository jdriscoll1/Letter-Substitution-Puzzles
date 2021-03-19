#ifndef seenInput
#define seenInput

#include "WordLinkedList.h"
#include "TreeSet.h"
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
@return --> 
1 -- Valid
2 - Not long enough
3 - too long
4 - not enough letters in common
5 - word = to prev
6 - word does not exist*/ 
int Check_Input(char* prevWord, const char* currWord, struct DummyHeadNode** (*HashMap)); 


/*This Converts a word to lowercase*/ 
char* toLowerCase(char* input); 

/*Between rounds it asks if you'd like to continue
@return cases: 
@return 0: User chose advance
@return 1: User chose quit 
@return 2: User chose to go back a level
@return 3: User chose same difficulty*/
int ContinueGames(); 

/*This extracts a substring from a string
@param word --> The string to be extracted from
@param start --> The first letter to be chosen
@param end --> The last letter to be chosen
@param replace --> 1) Frees the word that is inputted 0) Does not free inputted word
@example --> ("Butterfly", 0, 5, 0). Butter
@note --> Does give a '\0'*/ 
char* substr(char* word, int start, int end, int replace); 

/*This is a safer variant of the C-regular strncat
@param dest --> The string that will be added to
@param src --> The string whose contents will be appended onto the the dest
@param destLength --> How long hte first string is, program will crash if it tries to add more than safe amount
@paraam buff --> How long hte src string is
@param start --> Which character will begin being added to
@return --> Returns the new start location. Or the index that holds the current null terminator*/
int safeStrcat(char** dest, const char* src, int destLength, int buff, int start); 
   
#endif



 

