#ifndef _SHAREDMEM_H
#define _SHAREDMEM_H

extern PCB * Current_PCB;
extern PCB * PCB_Array;

typedef struct sema {
	int value;
	int PID;
	u_int8_t waiting [100];
	u_int8_t * head;
	u_int8_t * tail;
}seman;

void InitShared(void);
void PE(u_int8_t rand1, u_int8_t rand2);
void VE(u_int8_t rand1, u_int8_t rand2);
void SI(u_int8_t rand1, u_int8_t rand2);
void LS(u_int8_t rand1, u_int8_t rand2);
void ST(u_int8_t rand1, u_int8_t rand2);
void enqwait(int whichsem, u_int8_t * pPID);
u_int8_t * deqwait(int whichsem);


#endif
