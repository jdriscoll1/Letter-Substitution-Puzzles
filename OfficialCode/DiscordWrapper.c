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
int InterpretInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong HashMapLong,  const char* input, jobject textChannel);

//These are all of the components of the game 
 

void javaOutput(JNIEnv * env, jobject obj, char* output, jobject textChannel){

   jclass javaClass = (*env)->GetObjectClass(env, obj); 
   jmethodID mid = (*env)->GetMethodID(env, javaClass, "output", "(Ljava/lang/String;Ljava/lang/Object;)V");  
   jstring javaString; 
   javaString =  (*env)->NewStringUTF(env, output); 
   if(mid == NULL){return;}
   printf("In C:\n");
   (*env)->CallVoidMethod(env, obj, mid, javaString, textChannel); 
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_CreateHashMap(JNIEnv * env, jobject obj){
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
Java_flwp_FLWP_InstantiateGame(JNIEnv * env, jobject obj, jlong allWords, jlong HashMap, int minConnections, jobject textChannel){
	
 
	struct GameComponents *gameComponents = InitializeGameComponents((char**)allWords, (struct wordConnections***)HashMap, minConnections);
	
	char outputGoal[256]; 
	snprintf(outputGoal, sizeof(outputGoal), "Your task is to start with %s and arrive at %s", gameComponents->shortestPath[0], gameComponents->shortestPath[minConnections]); 
	javaOutput(env, obj, outputGoal, textChannel); 
	return (jlong)gameComponents; 
	
}

JNIEXPORT void JNICALL
Java_flwp_FLWP_DeleteGame(JNIEnv * env, jobject obj, jlong gameComponentsLong){
	FreeGameComponents((struct GameComponents*)gameComponentsLong);




}

JNIEXPORT void JNICALL
Java_flwp_FLWP_BeginningOutput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jobject textChannel){
	struct GameComponents* gameComponents = (struct GameComponents*)gameComponentsLong; 
	/*Output in Java the help line*/
	  
	javaOutput(env, obj, "Help Line!", textChannel); 
	
	
	
}

JNIEXPORT jstring JNICALL
Java_flwp_FLWP_TakeInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong HashMap, jstring text, jobject textChannel){
	//So, now we have the input -- in the Java portion we make sure it's not too long
	
	//We have to first convert it to a char* 
	const char* input; 
	input = (*env)->GetStringUTFChars(env, text, NULL); 
	if(input == NULL){
		return NULL; 
	}
	int isGoal = InterpretInput(env, obj, gameComponentsLong, HashMap, input, textChannel); 
	//Now we want to go through the cases
	
	//We have to Free the char* object
	(*env)->ReleaseStringUTFChars(env, text, input); 
}

int InterpretInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong HashMapLong, const char* input, jobject textChannel){
	struct GameComponents* gc = (struct GameComponents*)gameComponentsLong; 
	struct wordConnections **(*HashMap) = (struct wordConnections***)HashMap; 
	//If the string starts with a - 
	//Remove the word from the list
	if(input[0] == '-'){
		javaOutput(env, obj, "Removal", textChannel); 
	}
	
	//If the user wants to undo the previous move
	else if(strcmp(input, "u") == 0){
		javaOutput(env, obj, "Undo", textChannel);
	}
	//if the user wants to redo the previous move
	else if(strcmp(input, "r") == 0){
		javaOutput(env, obj, "Redo", textChannel);
	}
	
	//If the user queries for their goal
	else if(strcmp(input, "g") == 0){
		javaOutput(env, obj, "Goal", textChannel);
	}	
	//If user does not end the game
	else if (strcmp(input, "q") != 0 && strcmp(input, "finish") != 0){
		//Checks if the input is valid
		int isValid = AddWord_Struct(gc, input, HashMap); 
		char* output = (isValid == 1)?toString_WordLL(gc->userConnections, LINKED):"Invalid Input";  
		
		javaOutput(env, obj, output, textChannel); 
		
		if(isValid == 1){
			free(output); 
		}
	}
	
	
}


//Goal: Take an input, and base it on the games
/*Here are the cases: 
- Adding a word
- Removing a word
- Undoing a turn
- Redoing a turn
- Ending the Game*/
