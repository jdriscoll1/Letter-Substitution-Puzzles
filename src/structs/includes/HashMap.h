#ifndef seenHashMap
#define seenHashMap

#include <stdio.h>

#include "../includes/TreeSet.h"
#include "../includes/WordSet.h"
#include "../../api/includes/FLWG-API.h"

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
	//The same connections laid out back to back, built once when the dictionary is
	//read. Walking the list to reach the n-th adjacency is a chain of pointer hops;
	//searches that index into the adjacencies at random (the Monte Carlo rollout
	//policy) read this instead. Holds numConnections entries, in list order.
	int* connections;
	//Finds if it has been given as a hint
	int hintFound;
	/*Whether the ENGINE may use this word. Nought for almost every word; one
	for the handful the game will not say itself - see docs/offLimits.txt.

	IT IS NOT AN OBSCURITY. The rank says how many people know a word, and
	these are words everybody knows: the cap that keeps out ZOUK lets FUCK
	through, because FUCK is the 299th commonest word in English. Writing it
	here as a rank would also make the game COMPLIMENT a player for finding
	one, since the praise line reads the same number.

	And unlike the cap it never relaxes. The cap is dropped when the dictionary
	has nothing else; this is not.*/
	int offLimits;
	//Finds the number of connections a word has
	int numConnections;
	//Necessary to know which the previous id is in the bfs
	int prevID;
	/*How obscure the word is: its place in a list of the commonest english
	words, so 1 is the commonest word there is and a large number is a word
	almost nobody says. OBSCURITY_UNKNOWN when no ranks file was loaded, which
	is every caller that has not asked for one and every dictionary that does
	not ship with one.

	It is here rather than alongside the word because everything that chooses a
	word - dealing a board, a bot answering, a hint - already has the id and
	reads this array to do its work.*/
	int obscurity;
};

/*A word nobody has ranked. Deliberately the far end of the scale rather than
nought: an unranked word must never look like the commonest word in english to
something choosing the easiest move it can find.*/
/* Bigger than any rank a word can actually have, which is the whole point of
it. It was 99,999 while the ranks came from a list truncated at fifty thousand,
and that worked until the ranks went deeper: ZOUK is 130,566 now, so a cap of
99,999 stopped meaning "no cap" and started quietly excluding words - and WABS,
which appears nowhere in 1.66M words and is the most obscure thing in the
dictionary, sorted as LESS obscure than ZOUK. A sentinel has to sit outside the
range it is a sentinel for. */
#define OBSCURITY_UNKNOWN 9999999

struct wordStruct{
	//The word this structure is holding
	char* word; 
	//This is the word's id number
	int wordID; 
	
	
};

struct wordDataArray{
	int numWords;
	int numLetters;
	struct wordData** array;
	/*The hardest word the engine may deal or play from this dictionary - see
	wordData.obscurity. It lives here rather than beside the game because the
	searches that choose words are handed the word map and nothing else:
	minimax takes a wordDataArray, not a DataStructures, and threading one
	through the recursion to answer a question about a word would be the tail
	wagging the dog.

	OBSCURITY_UNKNOWN is no restriction, which is what a dictionary has until a
	board says otherwise.*/
	int obscurityCap;

};

int getNumAdjacencies(int id, struct DataStructures* data);
/*How obscure a word is - see wordData.obscurity. An id nothing knows about
answers OBSCURITY_UNKNOWN rather than reading off the end of the array.*/
int getObscurity(int id, struct DataStructures* data);
/*Whether a word is past what this dictionary is currently allowed to use. The
form the searches ask, because they hold the word map rather than the game.*/
int isTooObscureForGraph(int id, struct wordDataArray* graph); 

/*Order words the game is about to choose between so the commonest comes first.

A hint is not gated by the cap the way a dealt board is, and deliberately so. A
cap can only ever REMOVE a word, and the ranking is a noisy measure of whether
anybody knows one: TARE sits at 81,847 and DAWS at 79,367, so no line drawn
between them is anything but arbitrary. Sorting uses the same signal where being
wrong is cheap - a word the ranking undersells loses a place in a queue rather
than its existence, and is still offered when it is the best there is.

A hard cap on top of this ordering would also change nothing. If any option is
within the cap then the commonest option is within the cap and wins either way;
if none is, the cap refuses them all and something has to be offered regardless.
It can only ever bite where it would force the answer the ordering already gave.*/
void Sort_ByObscurity(int* ids, int count, struct wordDataArray* graph);

/*A word's neighbours, commonest first, written into out. Stops at max. Returns
how many were written.*/
int Neighbours_ByObscurity(int id, int* out, int max, struct wordDataArray* graph);
void Initialize_HashMaps_fd(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, int fd, int numLetters);

void Initialize_HashMaps(struct DummyHeadNode*** WordToInt_HashMap, struct wordDataArray* IntToWord_HashMap, char* path, int numLetters);

/*Read a ranks file over a dictionary that has already been loaded.

The ranks live in a file of their own, in the same order as the word file, so
rank number i belongs to word number i and nothing has to be looked up. They are
not on the connections lines because every reader of those - here and in the
tests - takes everything after the word as a neighbour index, so a rank put
anywhere on that line is silently read as a neighbour.

Both forms are no-ops when the file cannot be read or is the wrong length: a
missing ranks file leaves every word OBSCURITY_UNKNOWN and a playable game,
rather than no game at all.*/
void Fill_Obscurity(FILE* rankDoc, struct wordDataArray* IntToWord_HashMap);
void Load_Obscurity_fd(struct wordDataArray* IntToWord_HashMap, int fd);

/*Read the words the engine will not use. One word a line, blanks and lines
beginning with a hash ignored, and a word this dictionary does not hold ignored
too - so one list serves every word length.*/
void Load_OffLimits(struct wordDataArray* IntToWord_HashMap, const char* path,
	struct DataStructures* data);
void Load_OffLimits_fd(struct wordDataArray* IntToWord_HashMap, int fd,
	struct DataStructures* data);

/*The same list, already in memory and NUL terminated.
 *
 * WHICH IS WHAT THE APP HAS TO USE. An android asset is handed out as a
 * descriptor into the whole APK, seeked to where the asset starts - so reading
 * it to end-of-file reads the rest of the archive. The ranks loader never
 * noticed because it stops after a known number of lines; this one reads until
 * there is nothing left, and marked eight thousand words off limits on a four
 * thousand word dictionary before anybody looked at the log.*/
void Load_OffLimits_text(struct wordDataArray* IntToWord_HashMap, const char* text,
	struct DataStructures* data);

/*Whether the engine may choose this word. Asked wherever the engine picks a
word for itself and NEVER where it judges one the player typed.*/
int isOffLimitsForGraph(int id, struct wordDataArray* graph);
void Load_Obscurity(struct wordDataArray* IntToWord_HashMap, const char* path); 

struct DummyHeadNode** *Allocate_WordToInt(); 

void Allocate_IntToWord(struct wordDataArray* IntToWord_HashMap, int numWords, int numLetters); 

//The malloc statement for allocating an int to word struct
struct wordDataArray *Allocate_IntToWordStruct(); 

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
int Convert_WordToInt(char* word, struct DataStructures* data); 

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
FILE *OpenFile(char* filePath); 



void setHintFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

void removeHintFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

int getHintFound(int wordID, struct wordDataArray* IntToWord_HashMap); 

void reset_HashSet(struct wordDataArray* IntToWord_HashMap); 

void printOptions(int id, struct wordDataArray* IntToWord_HashMap, struct WordSet *wordSet);


int getNumOptions(int id, struct DataStructures* I2W); 

#endif
