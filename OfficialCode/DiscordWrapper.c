#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "WordLinkedList.h"
#include "HashMap.h"
#include "Arrays.h"
#include "GenericLinkedListNode.h"
#include "GameFunctions.h"
#include "PathGameComponents.h"


extern int numLetters; 

void javaOutput(JNIEnv * env, jobject obj, char* output, jobject textChannel); 

/*This the method that interprets the input
@param env --> This points to the JNI enviroment that calls the JNI methods
@param obj --> This is the class from which we were called (FLWP)
@param gameComponentsLong --> This is the long value of the game Components structure
@param textChannel --> This is the text channel from which the input was sent
@return --> 0) The goal has not been found 1) The goal has not been met 2) The user has quit*/
int InterpretInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong hashMapLong,  const char* input, jobject textChannel);

//These are all of the components of the game 
 

void javaOutput(JNIEnv * env, jobject obj, char* output, jobject textChannel){

 	
   jclass javaClass = (*env)->GetObjectClass(env, obj); 
   jmethodID mid = (*env)->GetMethodID(env, javaClass, "output", "(Ljava/lang/String;Ljava/lang/Object;)V");  
   if(mid == NULL){return;}
   jstring javaString; 
   javaString =  (*env)->NewStringUTF(env, output); 
  
 
   (*env)->CallVoidMethod(env, obj, mid, javaString, textChannel);
	
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_CreateHashMap(JNIEnv * env, jobject obj){
	 
	 srand(time(0)); 
	struct wordConnections **(*HashMap) = AllocateHashMap();
	return (jlong)HashMap; 
	
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_CreateWordStorage(JNIEnv * env, jobject obj, jlong HashMap){
	char** wordStorage = FillHashMap((struct wordConnections***)HashMap, 0);
	return (jlong)wordStorage; 
	
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_CreateAllWordsList(JNIEnv * env, jobject obj){
	char** allWords = ExtrapolateAllWords();
	return (jlong)allWords; 
	
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_InstantiateGame(JNIEnv * env, jobject obj, jlong allWords, jlong hashMapLong, int minConnections, jobject textChannel){
	

	struct GameComponents *gameComponents = InitializeGameComponents((char**)allWords, (struct wordConnections***)hashMapLong, minConnections);
	
	char outputGoal[255]; 
	snprintf(outputGoal, sizeof(outputGoal), "Your task is to start with %s and arrive at %s", gameComponents->shortestPath[0], gameComponents->shortestPath[minConnections]); 
	javaOutput(env, obj, outputGoal, textChannel); 
	
	return (jlong)gameComponents; 
	
}

JNIEXPORT void JNICALL
Java_flwp_FLWP_DeleteGame(JNIEnv * env, jobject obj, jlong gameComponentsLong){
	FreeGameComponents((struct GameComponents*)gameComponentsLong);




}


JNIEXPORT int JNICALL
Java_flwp_FLWP_TakeInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong hashMapLong, jstring text, jobject textChannel){
	//So, now we have the input -- in the Java portion we make sure it's not too long

	int j = 0; 

	//We have to first convert it to a char* 
	const char* input; 
	input = (*env)->GetStringUTFChars(env, text, NULL); 
	if(input == NULL){
		exit(0); 
	}
	int isGoal = InterpretInput(env, obj, gameComponentsLong, hashMapLong, input, textChannel);

		//If the user wins
		//We want to go on to the next round 
		//We want to ask the user if they would like to advance to the next level
		//If they say "yes" -- We want to advance them to the next level
		//If they say "no" -- We want to have them leave the game. Meaning that FIRST we should have the leave game feature 


	
	//We have to Free the char* object
	(*env)->ReleaseStringUTFChars(env, text, input); 
	return isGoal; 
}

int InterpretInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong hashMapLong, const char* input, jobject textChannel){
	struct GameComponents* gc = (struct GameComponents*)gameComponentsLong; 
	struct wordConnections **(*HashMap) = (struct wordConnections***)hashMapLong; 
	//If the string starts with a - 
	//Remove the word from the list
	int isValid = 1; 
	//If it starts with a -, and it's less than numLetters - 2, it's going to remove the word
	if(input[0] == '-' && strlen(input) <= numLetters + 2){
		//Remove word cannot have a constant char* because it needs it to change to remove the -. That is, there needs to be a word that removes it
		char* removeWord = malloc(sizeof(numLetters + 2)); 
		//Copies the const char* into the non const char*
		strcpy(removeWord, input);
		//Throws on a \0 in the end, for safe measures
		removeWord[numLetters + 2] = '\0'; 
		//Removes the word
		RemoveWord_Struct(gc, removeWord, 0); 
		//Frees the removeWord
		free(removeWord);  
	}
	
	//If the user wants to undo the previous move
	else if(strcmp(input, "u") == 0){
		Undo_Struct(gc);  
	
		
	}
	//if the user wants to redo the previous move
	else if(strcmp(input, "r") == 0){
		Redo_Struct(gc); 
	}
	
	//If the user queries for their goal
	else if(strcmp(input, "g") == 0){
		char goalOutput[50]; 
		snprintf(goalOutput, 50, "Your Goal: %s", gc->shortestPath[gc->minConnections]); 
		javaOutput(env, obj, goalOutput, textChannel);
		return 0; 
	
	}	
	//If user ends the game
	else if (strcmp(input, "q") == 0 || strcmp(input, "finish") == 0){
		javaOutput(env, obj, "Better Luck Next Time.", textChannel); 
		return -1; 
		
	
	}

	//Otherwise, it'll add a word
	else{
		
		isValid = AddWord_Struct(gc, input, HashMap); 
		//The user won
		if(strcmp(input, gc->shortestPath[gc->minConnections]) == 0 && isValid == 1){
			
			return 1; 
		
		}	
		
	}
	char* output = (isValid == 1)?toString_WordLL(gc->storage->next->listHeader, LINKED):"Invalid Input";  
		
	javaOutput(env, obj, output, textChannel); 
		
	if(isValid == 1){
		free(output); 
	}
	return 0; 

	
	
}
//Make sure to: 
//Avoid O(n) time for toString_WordLL -- don't have that run every time, only on remove
//Better IsValid Messages
//Convert the FLWG in C to use the PathGame Components
//Once I've done this, I'm ready to get to Yen K's Algorithm and plan out phase II 
//Change strcpy, strcmp, and others to safe version of it 

//Goal: Take an input, and base it on the games
/*Here are the cases: 
- Adding a word
- Removing a word
- Undoing a turn
- Redoing a turn
- Ending the Game*/
