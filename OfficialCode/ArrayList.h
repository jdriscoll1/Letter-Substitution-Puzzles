#ifndef seenArrayList
#define seenArrayList
//String, Character, Int, Double, TreeStorageNode
enum alistType{
		STR, CHARACTER, NUM, DOUBLE, TSN 
};

struct arrayList{
	//What does this 
	void* list; 
	//How big is it to begin with? 
	size_t initSize; 
	//How much should it move if it does need to grow or shrink
	size_t moveSize; 
	//What is it's current size? 
	size_t currSize;
	//This is EXACTLY how big it is 
	size_t currPrecision;  
	
	enum alistType type;  
	
};

//Initialize it
struct arrayList* init_ArrayList(size_t initSize, size_t moveSize, enum alistType type); 

//Add to it
void add_ArrayList(void* data, struct arrayList* aList, enum alistType type); 

//If you want to add a string to it 
void addString_ArrayList(const char* str, struct arrayList* aList); 
//Prints out the array list
void print_ArrayList(struct arrayList* aList, enum alistType type); 
//Remove a certain number from the back of it 
void removeBackStr_ArrayList(int length, struct arrayList* aList); 
//Frees the array list
void free_ArrayList(struct arrayList* aList); 

//Search for index
void* searchIndex_ArrayList(int index, struct arrayList* aList); 

//Search for a piece of information
int searchData_ArrayList(void* data, struct arrayList* aList); 
#endif


//How am I going to get strings to work? 
//Well, I already have the string move to back method
//So, then, what I do. Well, really, I don't want to think of it as a string, I instead want to think of it as an array of characters
//I'll start off with 
//First let's get it compatable with chars
//Then, I'll get a basic string to work, Well I'll start off by defining a linked list of strings, then I'll move them in 

