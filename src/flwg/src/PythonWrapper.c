#include <time.h>
#include <stdlib.h>
/*
Author - Jordan Driscoll
Date - July 17, 2021
Name - PythonWrapper.c
Description - Contains the functions that need to be called from python
*/

void InstantiateRandomizer(){
	srand(time(0)); 
}
