#ifndef _SHAREDMEM_H
#define _SHAREDMEM_H

extern PCB * Current_PCB;

typedef struct sema {
	int value;
	int PID;
}seman;

void InitShared(void);
void PE(u_int8_t rand1, u_int8_t rand2);
void VE(u_int8_t rand1, u_int8_t rand2);
void SI(u_int8_t rand1, u_int8_t rand2);
void LS(u_int8_t rand1, u_int8_t rand2);
void ST(u_int8_t rand1, u_int8_t rand2);



#endif
