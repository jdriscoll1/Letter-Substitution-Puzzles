#ifndef seenGeneric
#define seenGeneric

/*The Generic Linked List Node Header File
Purpose: Now when I make 2D Linked Lists, I will use this (aka what I should have done last year)*/ 

typedef enum listType{
	WORD_LL, TREE_SET_LL, HINT3, INT_LL
}listType;
/*The Structure that links to any linked list*/ 
struct GenericLinkedListNode{
	listType listType; 
    	/*This is generic, therefore as long as you do ( (struct name*)(header->listHeader) )->var it'll be ok'*/ 
	void* listHeader; 
	/*The next position in the set*/ 
	struct GenericLinkedListNode *next; 
	struct GenericLinkedListNode *prev; 

	
};


void Add_GenericLinkedListNode(struct GenericLinkedListNode *header, const enum listType type);

void AddToFront_GenericLinkedListNode(struct GenericLinkedListNode *header, const enum listType type); 

/*Copies a type of information into the generic linked list
@param copy --> The structure to be copied into it
@param header --> The header of the generic linked list
@param intoFront --> Determines if it should be copied into the front, or the back of the linked list
@param type --> The type of linked list to be copied in*/ 
void CopyInto_GenericLinkedListNode(void* copy, struct GenericLinkedListNode *header, int intoFront, const enum listType type); 


/*Goes through the generic node linked list, and based off of it's type of linked list, it will print it out according to that node
@param header --> the header of the Generic Linked List*/ 
void Print_GenericLinkedList(struct GenericLinkedListNode *header); 

/*Returns a specified index
@param index --> the index that is to be looped to, for example, if there are 7 linked lists, and index is 5, it will return a header to the 5th index
@param header --> the start of the linked list of linked lists
@return --> Returns a generic linked list node that points to a linked list
#Note: It always skips past the header, that is, if you return index 0, it will skip past the header*/ 
struct GenericLinkedListNode *ReturnIndex_GenericLinkedListNode(const int index, struct GenericLinkedListNode *header);  

/*This only frees a specified number of links in the Generic Linked List
@Note --> This does not free the header
@param freeAmount --> The amount that should be freed
@param header --> The header of the structure to be freed*/ 
void FreeQuantity_GenericLinkedList(int freeAmount, struct GenericLinkedListNode *header); 

/*Frees the generic linked list link list and everything it points to
@param header --> the front of the linked lists of linked lists*/ 
void Free_GenericLinkedList(struct GenericLinkedListNode *header); 



#endif
