#ifndef seenHashFunc
#define seenHashFunc
/*
All of the Hash Functions for the Hash Map


*/

/*Hash Map Keys*/
/*First Hash Key --> Uses this to convert the first letter of the word to an array
@param firstLetter --> the first letter of the word
@return --> Returns the first letter - 96 such that it can be used in the array 
*/
int FirstHashFunction(char firstLetter); 
/*Second Hash Key --> Finds the first vowel in the word
Loops through the word till it sees the vowel 
@param word --> The word that it searches through to find the first vowel 
@return --> a number that represents the vowel such that it can be used in an array 
*/
int SecondHashFunction(char* word);
/*Checks which vowel a letter is
@param letter --> the letter it's checking to see what vowel it is
@param vowels[] --> the array of vowels that is used to check what vowel the letter is
@return --> integer representing the vowel which the letter is 
*/
int VowelSearch(char letter, char vowels[6]);

/*The purpose of this is to make it so that when it is time to take a linked list from the large 2D array, it will be easier
@param word --> The word that the array contains
@param HashSet --> The Hash Set that the word is trying to find its linked list
@return 1D Linked List --> given pies, it would return the array with [p][i], this is a linked list that the words are stored in*/ 
void* HashSetArray(char* word, void* **(*HashSet));  

#endif
