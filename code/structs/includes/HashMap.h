#ifndef seenHashMap
#define seenHashMap

#include <stdio.h>

#include "../includes/TreeSet.h"

/*This is the Map that will take a word and convert it to an integer*/

//This is the structure that is used in the big array
//It is used to convert an integer to a word
//To find a word's connections
//And to see if it is used in a hash map
//Therefore it is the wordInformationStruct
struct wordData{
	//Necessary for number to word conversions
	char* word; 
	//Necessary to find word's connections
	struct intList* connectionHeader; 
	//Finds if it has been used in algorithm
	int algFound; 
	//Finds if it has been given as a hint
	int hintFound; 
};

struct wordStruct{
	//The word this structure is holding
	char* word; 
	//This is the word's id number
	int wordID; 
	
	
};

struct wordDataArray{
	int numWords; 
	struct wordData** array; 
	
};

void Initialize_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 

struct DummyHeadNode** *Allocate_WordToInt(); 

void Allocate_IntToWord(struct wordDataArray* IntToWord_HashMap, int numWords); 

/*This takes two hash maps and a file, and reaps from the file its contents and puts it into the two hash maps*/
void Fill_HashMaps(FILE* wordDoc, struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 

//Creates an empty word data node
struct wordData* Create_WordData(char* word);

//Creates an empty word struct node 
struct wordStruct* Create_WordStruct(char* word, int id);

//Prints out a word data node
void Print_WordData(struct wordData* wData);

//Prints out a word structure node
void Print_WordStruct(struct wordStruct* wStruct);  

//Outputs the whole Int to word hash map
void Print_IntToWord_HashMap(struct wordDataArray* HashMap); 

//Outptus the whole word to int hash map
void Print_WordToInt_HashMap(struct DummyHeadNode** *HashMap); 

//Outputs the whole word to int hash map
void Print_WordStruct(struct wordStruct* wordStruct); 

//Frees a word data node
void Free_WordData(struct wordData* wordData); 

//Frees a word structure node
void Free_WordStruct(struct wordStruct* wordStruct); 

/*This takes both of the hash maps word--> int and int --> word and frees them
@param WordToInt_HashMap --> The HashMap that takes a word and converts it to an int
@param IntToWord_HashMap --> The HashMap that takes an int and converts it to a word
@param numWords --> The number of words that are needed to be freed
*/
void Free_HashMaps(struct DummyHeadNode** *WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap); 

int getNumWords(FILE* wordDoc); 

/*Takes a word, and outputs its ID
@param word --> The word to be converted
@param WordToInt_HashMap --> The Hash Map that takes a word and converts it to an integer
@return --> Returns the ID of the word*/
int Convert_WordToInt(char* word, struct DummyHeadNode*** WordToInt_HashMap); 

/*Takes an ID, and outputs the associated word*/
char* Convert_IntToWord(int wordID, struct wordDataArray* IntToWord_HashMap); 

struct intList* getConnections(int id, struct wordDataArray* IntToWord_HashMap); 

/*This finds all of hte connections with a few restrictions
@Restriction 1 --> The word cannot have already been found
@Restriction 2 --> It can only find a certain number of connections
@param input --> This is the word whose connections will be found
@param cap --> This is the maximum number of connections it can find
@param IntToWord_HashMap --> This will give all of the words connections
@return --> Returns a list of connections*/
struct intList *getConnections_Restrictions(int input, int cap, struct wordDataArray* IntToWord_HashMap); 

/*This opens a file*/ 
FILE *OpenFile(); 

//Getters & Setters for the hash set
void setAlgFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

int getAlgFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

void removeAlgFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

void setHintFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

void removeHintFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

int getHintFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

void reset_HashSet(struct wordDataArray* IntToWord_HashMap); 

#endif
