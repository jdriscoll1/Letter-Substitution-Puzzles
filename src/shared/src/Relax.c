#include <limits.h>

#include "../includes/Relax.h"

struct Band anyBand(void){
	struct Band any = { 0, INT_MAX };
	return any;
}

int isAnything(int round){
	return round >= RELAXATION_ROUNDS - 1;
}

struct Band loosen(struct Band asked, int round){
	if(isAnything(round)){
		return anyBand();
	}

	struct Band wider;
	wider.min = asked.min - round;
	if(wider.min < 0){
		wider.min = 0;
	}

	/* Guarded rather than merely added, because a caller is free to hand in a
	   ceiling of INT_MAX to mean it does not care, and widening that would
	   wrap it round to a floor. */
	if(asked.max > INT_MAX - round){
		wider.max = INT_MAX;
	}
	else {
		wider.max = asked.max + round;
	}

	return wider;
}
