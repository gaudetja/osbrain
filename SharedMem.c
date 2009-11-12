
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "SharedMem.h"
u_int32_t * Shared;

void InitShared(void) {
	Shared=calloc(100,4);
}
void PE(u_int8_t rand1, u_int8_t rand2) {


}
void VE(u_int8_t rand1, u_int8_t rand2) {


}
void SI(u_int8_t rand1, u_int8_t rand2) {


}
