#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "WordLinkedList.h"
#include "HashMap2.h"
#include "Arrays.h"
#include "GenericLinkedListNode.h"
#include "GameFunctions.h"
#include "PathGameComponents.h"
#include "Hints.h"
#include "UserInput.h"

#define SIZE 255


extern int numLetters; 

void javaOutput(JNIEnv * env, jobject obj, char* output, jobject textChannel); 

/*This the method that interprets the input
@param env --> This points to the JNI enviroment that calls the JNI methods
@param obj --> This is the class from which we were called (FLWP)
@param gameComponentsLong --> This is the long value of the game Components structure
@param textChannel --> This is the text channel from which the input was sent
@return --> 0) The goal has not been found 1) The goal has not been met 2) The user has quit*/
int InterpretInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong hashMapLong,  const char* input, int strLength, jobject textChannel);

//These are all of the components of the game 
void validOutput(JNIEnv * env, jobject obj, int valid, jobject textChannel); 


int InterpretInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong hashMapLong, const char* input, int strLength, jobject textChannel){
	struct GameComponents* gc = (struct GameComponents*)gameComponentsLong; 
	struct DummyHeadNode **(*HashMap) = (struct DummyHeadNode***)hashMapLong; 
	//If the string starts with a - 
	//Remove the word from the list
	int isValid = 1; 
	
	//The hint taken is equal to 0 originally
	int hintTaken = 0; 
	
	//If it starts with a -, and it's less than numLetters - 2, it's going to remove the word
	if(input[0] == '-' && strLength <= numLetters + 2){
		//Remove word cannot have a constant char* because it needs it to change to remove the -. That is, there needs to be a word that removes it
		char* removeWord = malloc(numLetters + 2); 
		//Copies the const char* into the non const char*
		safeStrcpy(&removeWord, input, numLetters + 2, numLetters + 2); 
		//Throws on a \0 in the end, for safe measures
		removeWord[numLetters + 2] = '\0'; 
		//Removes the word
		RemoveWord_Struct(gc, removeWord, 0); 
		//Frees the removeWord
		free(removeWord);  
	}
	else if(input[0] == '?'){
		char* testInput = substr((char*)input, 1, numLetters + 1, 0);
		int inDic = inDictionary(testInput, HashMap); 
		if(inDic == 1){
			javaOutput(env, obj, "That is a valid word\n", textChannel); 
		}
		else{
			javaOutput(env, obj, "That is not a valid word\n", textChannel); 
		}
		free(testInput); 
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
		snprintf(goalOutput, 50, "Your Goal: %s", gc->goal); 
		javaOutput(env, obj, goalOutput, textChannel);
		return -1; 
	
	}	
	//If user ends the game
	else if (strcmp(input, "q") == 0 || strcmp(input, "finish") == 0){
		javaOutput(env, obj, "Better Luck Next Time.", textChannel); 
		return -2; 
		
	
	}
	//Next for the hitns
	else if(strcmp(input, "1") == 0){
		char* output =  hint1(gameComponentsLong);
		javaOutput(env, obj, output, textChannel); 
		free(output); 
		hintTaken = 1; 
 
	}
	
	else if(strcmp(input, "2") == 0){
		char* output =  hint2(gameComponentsLong, (struct DummyHeadNode***)hashMapLong);
		javaOutput(env, obj, output, textChannel); 
		free(output);
		hintTaken = 1;  
	}
	
	else if(strcmp(input, "3") == 0){
		char* output =  hint3(gameComponentsLong, (struct DummyHeadNode***)hashMapLong);
		javaOutput(env, obj, output, textChannel); 
		free(output); 
		hintTaken = 1; 

	}
	
	else if(strcmp(input, "p") == 0){
		char goalOutput[50]; 
		snprintf(goalOutput, 50, "Hint Points Available: %d", gc->hc->hintPoints); 
		javaOutput(env, obj, goalOutput, textChannel);
		
	}

	//Otherwise, it'll add a word
	else{
		
		isValid = AddWord_Struct(gc, input, HashMap); 
		//The user won
		if(strcmp(input, gc->goal) == 0 && isValid == 1){
			
			return getScore(gc); 
		
		}	
		
	}
	
	char* output = gc->aList->list;
	 
	//c is the official "current" command. So it should output the game without outputting an error message. 
	if(strcmp(input, "c") != 0){
		//Outputs the error message based on isValid 
		validOutput(env, obj, isValid, textChannel);
	}
	//Outputs the new string 
	javaOutput(env, obj, output, textChannel); 
		
	if(hintTaken == 1){
		return gc->hc->hintPoints + 101; 
		
	}
	
	return -1; 

	
	
}

void validOutput(JNIEnv * env, jobject obj, int valid, jobject textChannel){
	if(valid != 1){
		switch(valid){
			case(2):
				javaOutput(env, obj, "Input is Too Short", textChannel); 
				break;
				
			case(3):
				javaOutput(env, obj, "Input is Too Long", textChannel); 
				break; 
			case(4):
				javaOutput(env, obj, "Not Enough Letters In Common", textChannel); 
				break; 
			case(5):
				javaOutput(env, obj, "Word is equal to prev", textChannel); 
				break; 
			case(6):
				javaOutput(env, obj, "Word does not exist", textChannel); 
				break; 
			
			
			
		}
	}
	//It's too long
	//It's too short
	//It does not have the minimum character number in common 
	//
}






void javaOutput(JNIEnv * env, jobject obj, char* output, jobject textChannel){

 	jclass cls = (*env)->GetObjectClass(env, obj); 
    
	jmethodID mid = (*env)->GetStaticMethodID(env, cls, "output", "(Ljava/lang/String;Ljava/lang/Object;)V");
    if (mid == NULL) {
  	  printf("Method ID Not Found"); 
      exit(0);  /* method not found */
    }
   jstring javaString; 
   javaString =  (*env)->NewStringUTF(env, output); 
   (*env)->CallStaticVoidMethod(env, cls, mid, javaString, textChannel);
   
   

	
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_CreateHashMap(JNIEnv * env, jobject obj){
	 
	 srand(time(0)); 
	struct DummyHeadNode **(*HashMap) = Create_HashMap();
	return (jlong)HashMap; 
	
}


JNIEXPORT long JNICALL
Java_flwp_FLWP_CreateAllWordsList(JNIEnv * env, jobject obj){
	char** allWords = ExtrapolateAllWords();
	return (jlong)allWords; 
	
}

JNIEXPORT long JNICALL
Java_flwp_FLWP_InstantiateGame(JNIEnv * env, jobject obj, jlong allWords, jlong hashMapLong, int minConnections, int hintPoints, jobject textChannel){
	
	srand(time(0));
	struct GameComponents *gameComponents = InitializeGameComponents((char**)allWords, (struct DummyHeadNode***)hashMapLong, minConnections);
	gameComponents->hc->hintPoints = hintPoints; 
	char outputGoal[255]; 
	snprintf(outputGoal, sizeof(outputGoal), "Your task is to start with %s and arrive at %s\nYou have %d hint points.\nYou are on round %d", gameComponents->start, gameComponents->goal, gameComponents->hc->hintPoints, minConnections - 1); 
	javaOutput(env, obj, outputGoal, textChannel); 
	
	return (jlong)gameComponents; 
	
}

JNIEXPORT void JNICALL
Java_flwp_FLWP_DeleteGame(JNIEnv * env, jobject obj, jlong gameComponentsLong){
	FreeGameComponents((struct GameComponents*)gameComponentsLong);




}

JNIEXPORT void JNICALL
Java_flwp_FLWP_ResetRound(JNIEnv * env, jobject obj, jlong gameComponentsLong, jobject textChannel){
	struct GameComponents *gc = (struct GameComponents*)gameComponentsLong; 
	ResetGameComponents(gc);
	char outputGoal[255]; 
	snprintf(outputGoal, sizeof(outputGoal), "Your task is to start with %s and arrive at %s\nYou have %d hint points.\nYou are on round %d", gc->start, gc->goal, gc->hc->hintPoints, gc->minConnections - 1); 
	javaOutput(env, obj, outputGoal, textChannel); 





}



JNIEXPORT int JNICALL
Java_flwp_FLWP_TakeInput(JNIEnv * env, jobject obj, jlong gameComponentsLong, jlong hashMapLong, jstring text, int strLength, jobject textChannel){
	//So, now we have the input -- in the Java portion we make sure it's not too long
	struct GameComponents *gc = (struct GameComponents*)gameComponentsLong; 
	int j = 0; 

	//We have to first convert it to a char* 
	const char* input; 
	input = (*env)->GetStringUTFChars(env, text, NULL); 
	if(input == NULL){
		exit(0); 
	}
	int score = InterpretInput(env, obj, gameComponentsLong, hashMapLong, input, strLength, textChannel);
		//If the user wins
		//We want to go on to the next round 
		//We want to ask the user if they would like to advance to the next level
		//If they say "yes" -- We want to advance them to the next level
		//If they say "no" -- We want to have them leave the game. Meaning that FIRST we should have the leave game feature 


	
	//We have to Free the char* object
	(*env)->ReleaseStringUTFChars(env, text, input); 
	
	//This sends the score back
	//if it's greater than 0 -- it's the score of the game
	//If it's -1 -- the user did not win -- it's normal 
	//If it's -2 -- the user quit 
	//if it's > 100 -- the hintpoints changed
	return score; 
	
	//Tomorrow make it so that it can deal with hint points
}



JNIEXPORT long JNICALL
Java_flwp_FLWP_StaticCall(JNIEnv * env, jobject obj){
	 
	jclass cls = (*env)->GetObjectClass(env, obj); 
    
	jmethodID mid = 
         (*env)->GetStaticMethodID(env, cls, "print", "()V");
     if (mid == NULL) {
         printf("Method ID Not Found"); 
		 exit(0);  /* method not found */
     }
     printf("In C\n");
     (*env)->CallStaticVoidMethod(env, cls, mid);

}




//Make sure to: 


//~<word> --> Checks if a word is in the dictionary 
//Add sark into the dictionary
//Add sade into the dictionary 
//Add bonk into the dictionary
//Make the final round
//Fix AVL Tree

//PHASE I -- COMPLETED

//Phase II Begins
//Get the Four Letter Word Game to Be Playable 


//Once I've done this, I'm ready to get to Yen K's Algorithm and plan out phase II 
//Change strcpy, strcmp, and others to safe version of it 

