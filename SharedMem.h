#ifndef _SHAREDMEM_H
#define _SHAREDMEM_H

extern PCB * Current_PCB;

typedef struct sema {
	int value;
	int PID;
	int waiting [100];
	int * head;
	int * tail;
}seman;

void InitShared(void);
void PE(u_int8_t rand1, u_int8_t rand2);
void VE(u_int8_t rand1, u_int8_t rand2);
void SI(u_int8_t rand1, u_int8_t rand2);
void LS(u_int8_t rand1, u_int8_t rand2);
void ST(u_int8_t rand1, u_int8_t rand2);
void enqwait(int whichsem);
int deqwait(int whichsem);


#endif
