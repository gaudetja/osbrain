#include "SharedMem.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

u_int32_t * Shared;

void InitShared(void) {
	Shared=calloc(100,4);
}
int PE(void) {

}
int VE(void) {

}
int SI(void) {

}
