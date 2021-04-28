#ifndef seenHints
#define seenHints

#include "ArrayList.h"
#include "WordLinkedList.h"
#include "PathGameComponents.h"
#include "GenericLinkedListNode.h"
#include "TreeStorageNode.h"
#include "BreadthFirstSearch.h"
#include "TreeSet.h"
#include "HashMaps.h"


typedef int bool; 
#define true 1
#define false 0 

struct HintComponents{
	int hintPoints; 
	//Has the user used hint 1
	int hint1Used;  
	//I have to have a way to know which letters are useable
	struct GenericLinkedListNode* letterOptions;
	struct GenericLinkedListNode* letterOptionsHeader;  
	//How expensive the hints are
	int hint1Weight; 
	int hint2Weight; 
	int hint3Weight; 
	//How much does the hint 2 output? 
	int hint2OutputQuantity; 
	
	bool* lettersGiven;
	
	
	
};

struct hint3Struct{
	//This is the word that the user said "I would like to use a hint"
	int word; 
	//This is the minimum number of conncetions between the current word 
	int minConnections; 
	//This is an array of characters
	struct arrayList* letters; 
};

struct HintComponents* init_HintComponents(); 

/*Hint 1: Reveals the minimum number of connections
@param gc --> The Game Components
@return --> Returns what it describes to the user*/
char* hint1(unsigned long long gcLong);
/*Hint 2: Offers the user a word options from their current locations
@param gc --> The number of game components
@param HashMap --> How the game finds the connections
@return --> Returns what it describes to the user*/ 
char* hint2(unsigned long long gcLong, struct wordDataArray* IntToWord_HashMap); 
/*Hint 3: Offers the user a letter that is used be from the first word to the last word
@param gc --> The current game componenents
@param HashMap --> How the user figures out a letter*/
char* hint3(unsigned long long gcLong,  struct wordDataArray* IntToWord_HashMap); 

/*This initializes a new Hint 3 Structure
@param hc --> The hint components
@param currWord --> This is the current word the user is on
@param goal --> This is the word the user searches for
@param HashMap --> This is the map used to get teh connection*/
void init_hint3(struct HintComponents *hc, int currWord, int goal, struct wordDataArray* IntToWord_HashMap); 

/*Converts the Tree Storage Node To An Array List - starts at the back and goes to the front
@Restriction --> Neither adds the first, nor last node. Only adds the ones in between
@Restriction --> It's letter based, it only adds letter's that have not been given 
@param aList --> The Array List that will store the Tree Storage Node
@lettersGiven --> This is the HashSet that determines which letters need to be not be added
@param End --> This is the Tree Storage Nodes whose values shall be added*/
void Convert_TreeStorageNodeArrayList_HintRestrictions(struct arrayList* aList, bool* lettersGiven, struct TreeStorageNode *End, struct wordDataArray* IntToWord_HashMap); 

/*This is the Breadth First Search Who Searches Given A Goal Word
@Restriction --> This does not give a list of words, it outputs an array of letters
@Restriction --> It only contains 2 letters 
@param start --> The word at which the search begins
@param goal --> The word at which the search ends
@param HashSet --> This is a list of letters that have already been given to the user
@param HashMap --> The HashMap that contains all the words and their connections
@param storageType --> Are you using the Tree Set or the Hash Set to store found words
@return --> Returns the connections as an array as well as the size*/
struct arrayList* BreadthFirstSearch_Dest_HintRestrictions(int start, int goal, struct wordDataArray* IntToWord_HashMap, struct hint3Struct *hc, bool* HashSet); 

void free_HintComponents(unsigned long long hcLong, struct wordDataArray* IntToWord_HashMap);

/*Gets the number of digits
@param num --> The number whose digits are being put in question
@return --> Returns the quantity of digits*/
int getDigitQuantity(int num); 

#endif
