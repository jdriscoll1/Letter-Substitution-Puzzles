#ifndef seenHashFunc
#define seenHashFunc

#include "HashMap.h"
/*
All of the Hash Functions for the Hash Map


*/

/*Hash Map Keys*/
/*First Hash Key --> Uses this to convert the first letter of the word to an array
@param firstLetter --> the first letter of the word
@return --> Returns the first letter - 96 such that it can be used in the array 
*/
int FirstHashFunction(const char firstLetter); 
/*Second Hash Key --> Finds the first vowel in the word
Loops through the word till it sees the vowel 
@param word --> The word that it searches through to find the first vowel 
@return --> a number that represents the vowel such that it can be used in an array 
*/
int SecondHashFunction(const char* word, struct wordDataArray* IntToWord_HashMap);
/*Checks which vowel a letter is
@param letter --> the letter it's checking to see what vowel it is
@param vowels[] --> the array of vowels that is used to check what vowel the letter is
@return --> integer representing the vowel which the letter is 
*/
int VowelSearch(char letter, char vowels[6]);



#endif
