
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "SharedMem.h"

#define numshared 100
u_int32_t * shared;

void InitShared(void) {
	int i;
	shared=calloc(numshared,4);
	for(i=0;i<numshared;i++) {
		shared[i]=1;
	}
}

void PE(u_int8_t rand1, u_int8_t rand2) {


}

void VE(u_int8_t rand1, u_int8_t rand2) {


}

void SI(u_int8_t rand1, u_int8_t rand2) {


}
