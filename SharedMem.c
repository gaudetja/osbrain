
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "Exec_Brain.h"
#include "SharedMem.h"
#include "Memory.h"
#include "sched.h"





#define numshared 100
u_int32_t * shared;
seman * semaphore;

/*void test(void);
void test(void)
{
	semaphore
}*/
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
		printf("There are only 0-99 available semaphores! you tried to  %d",i);
		exit(0);
	}
	if (semaphore[i].value==1) {
	 	semaphore[i].value=0;
		semaphore[i].PID=Current_PCB->PID;
	}
	else if (semaphore[i].value==0) {
		enqwait(i,&(Current_PCB->PID)); //add to wait list for semaphore
		blockq(&(Current_PCB->PID),1);  //add to block q
		Current_PCB->Block=1;
		Current_PCB->TDMA=TDMA_Setting;
	}
	else printf("This semaphore has a value other than 1 or 0");
}

void VE(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	u_int8_t * PID;
	if(i>99) {
		printf("There are only 0-99 available semaphores! you tried to release %d",i);
		exit(0);
	}
	printf("semaphore[i].value = %d\n", semaphore[i].value);
	printf("semaphore[i].PID = %d\n",semaphore[i].PID);
	printf("Current_PCB->PID = %d\n",Current_PCB->PID);
	if(semaphore[i].value == 1) {
		printf("This semaphore is not currently held by anyone.");
		exit(0);
	}
	else if (semaphore[i].PID ==-1) {
		printf("this semaphore has not been checked out by a PID but the value is 1");
	}
	else if (semaphore[i].PID == Current_PCB->PID) {
		if (semaphore[i].head==semaphore[i].tail){
		 	semaphore[i].value=1;
			semaphore[i].PID=-1;
		}
		if (semaphore[i].head!=semaphore[i].tail) {
			PID = deqwait(i);
			blockq(PID,0);  //take off the block q
			readyq(PID,1);  //add to ready q
			PCB_Array[*PID].Block=0;
			semaphore[i].PID = *PID;
		}
	}
	else {
		printf("This semaphore is held by someone else");
		exit(0);
	}
}

void SI(u_int8_t rand1, u_int8_t rand2) {
	int i = 10*(rand1-48)+(rand2-48);
	if(i>99) {
		printf("There are only 0-99 available semaphores! you tried to initialize %d",i);
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
void enqwait(int whichsem, u_int8_t * pPID) {
	if (semaphore[whichsem].tail==(semaphore[whichsem].waiting+99) && semaphore[whichsem].head!=semaphore[whichsem].waiting) {
		semaphore[whichsem].tail=0;
	}
	if (semaphore[whichsem].tail==(semaphore[whichsem].waiting+99) && semaphore[whichsem].head==semaphore[whichsem].waiting) {
		 printf("somehow you managed to fill up the wait queue for this semaphore");
		 exit(0);
	}
	else semaphore[whichsem].tail++;
}
u_int8_t * deqwait(int whichsem) {
	u_int8_t * returnval = NULL;
	if (semaphore[whichsem].head==semaphore[whichsem].tail) {
		printf("nothing to dequeue in the wait list for semaphore %d",whichsem);
		exit(0);
	}
	else {
		returnval = semaphore[whichsem].head;
		if(semaphore[whichsem].head==semaphore[whichsem].waiting+99) {
			semaphore[whichsem].head=semaphore[whichsem].waiting;
		}
		else semaphore[whichsem].head++;
	}

	return returnval;
}

