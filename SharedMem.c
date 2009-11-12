
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "SharedMem.h"

struct sema {
	int value;
	int PID;
};



#define numshared 100
u_int32_t * shared;
//sema semaphore;

void InitShared(void) {
	int i;
	shared=calloc(numshared,4);
	//semaphore=calloc(numshared,sizeof(semaphore));

	for(i=0;i<numshared;i++) {
	//	semaphore[i]=1;
	}
}

void PE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	//if(semaphore->value) {
i++;
	//}
}
void VE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	i++;
}
void SI(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	i++;
}
void LS(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	i++;
	//Current_PCB->R = shared[i];
}
void ST(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	i++;
	//shared[i] = Current_PCB->R;
}
