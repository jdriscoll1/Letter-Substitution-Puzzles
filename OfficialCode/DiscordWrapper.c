#include <jni.h>


JNIEXPORT void JNICALL
Java_flwp_FLWP_javaOutput(JNIEnv * env, jobject obj){

   jclass javaClass = (*env)->GetObjectClass(env, obj); 
   jmethodID mid = (*env)->GetMethodID(env, javaClass, "output", "(Ljava/lang/String;)V");  
   jstring javaString; 
   char* output = "Couer D'Alene";
   javaString =  (*env)->NewStringUTF(env, output); 
   if(mid == NULL){return;}
   printf("In C:\n");
   (*env)->CallVoidMethod(env, obj, mid, javaString); 
}
