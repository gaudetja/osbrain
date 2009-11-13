
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
	semaphore = calloc(numshared,sizeof(semaphore));

	//semaphore =tmp;
	for(i=0;i<numshared;i++) {
		semaphore[i].PID=-1;
		semaphore[i].value=1;
		semaphore[i].head = semaphore[i].waiting;
		semaphore[i].tail = semaphore[i].waiting;
	}
}

void PE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(i>99) {
		printf("There are only 0-99 available semaphores! you tried to access %d",i);
		exit(0);
	}
	if(semaphore[i].value) {
	 	semaphore[i].value=0;
		semaphore[i].PID=Current_PCB->PID;

	}
}

void VE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(i>99) {
		printf("There are only 0-99 available semaphores! you tried to access %d",i);
		exit(0);
	}
	if(semaphore[i].value && (semaphore[i].PID==Current_PCB->PID)) {
	 	semaphore[i].value=1;
		semaphore[i].PID=-1;
	}
//	else()
}

void SI(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(i>99) {
		printf("There are only 0-99 available semaphores! you tried to access %d",i);
		exit(0);
	}
	semaphore[i].value=Current_PCB->R;
}

void LS(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(i>99) {
		printf("You should not try to access shared memory outside of limits!");
		exit(0);
	}
	Current_PCB->R = shared[i];
}

void ST(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(i>99) {
		printf("You should not try to access shared memory outside of limits!");
		exit(0);
	}
	shared[i] = Current_PCB->R;
}
void enqwait(int whichsem) {
	if (semaphore[whichsem].tail==(semaphore[whichsem].waiting+99) && semaphore[whichsem].head!=semaphore[whichsem].waiting) {
		semaphore[whichsem].tail=0;
	}
	if (semaphore[whichsem].tail==(semaphore[whichsem].waiting+99) && semaphore[whichsem].head==semaphore[whichsem].waiting) {
		 printf("somehow you managed to fill up the wait queue for this semaphore");
		 exit(0);
	}
	else semaphore[whichsem].tail++;
}
int deqwait(int whichsem) {

	return 0;
}

