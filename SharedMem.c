
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "Exec_Brain.h"
#include "SharedMem.h"





#define numshared 100
u_int32_t * shared;
seman * semaphore;

void InitShared(void) {
	int i;

	shared=calloc(numshared,4);
	semaphore = calloc(numshared,8);

	//semaphore =tmp;
	for(i=0;i<numshared;i++) {
		semaphore[i].PID=-1;
		semaphore[i].value=1;
	}
}

void PE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(semaphore[i].value) {
	 	semaphore[i].value=0;
		semaphore[i].PID=Current_PCB->PID;

	}
}

void VE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(semaphore[i].value && (semaphore[i].PID==Current_PCB->PID)) {
	 	semaphore[i].value=1;
		semaphore[i].PID=-1;
	}
}

void SI(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	semaphore[i].value=Current_PCB->R;
}

void LS(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	Current_PCB->R = shared[i];
}

void ST(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	shared[i] = Current_PCB->R;
}
