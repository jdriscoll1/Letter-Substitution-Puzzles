#include <stdlib.h>
#include <time.h>

#include "../../src/structs/includes/WordSet.h"
#include "../../src/flwp/includes/GameFunctions.h"
#include "php_wrapper.h"

int ChooseFirstWord(struct wordDataArray* IntToWord_HashMap){
	srand(time(NULL)); 
	return ChooseStart(IntToWord_HashMap); 	
}

int main(){
	//The Hash Map that can take a word and find its ID
	struct DummyHeadNode** *WordToInt_HashMap;
	
	//The Hash Map that can take an integer and find all sorts of information about it, including the word 
	struct wordDataArray *IntToWord_HashMap; 
	
	char* path = "../../docs/4.txt"; 
	
	//Allocates the Word to Int HashMap
	WordToInt_HashMap = Allocate_WordToInt();
	
	IntToWord_HashMap = Allocate_IntToWordStruct();  
	
	struct WordSet *wordSet = init_WordSet(IntToWord_HashMap->numWords);

	Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path);
 
	int x = ChooseStart(IntToWord_HashMap);

	printf("%d", x); 
}
