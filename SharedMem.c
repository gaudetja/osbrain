
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "SharedMem.h"
u_int32_t * Shared;

void InitShared(void) {
	Shared=calloc(100,4);
}
int PE(u_int8_t rand1, u_int8_t rand2) {

}
int VE(u_int8_t rand1, u_int8_t rand2) {

}
int SI(u_int8_t rand1, u_int8_t rand2) {

}
