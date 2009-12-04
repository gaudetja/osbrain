/*
 * PageMem.h
 *
 *      Author: Zach Norris
 *      	Gary Jordan
 *      	Joe Gaudet
 */

#ifndef RAM_H_
#define RAM_H_
#include "Exec_Brain.h"
extern PCB * Current_PCB;
extern PCB * PCB_Array;

typedef struct {
	u_int32_t framenumber;
	u_int32_t count;
	char v; //1 for valid, 0 otherwise
}cell;

void InsertPage(u_int32_t Logical_Address);
void Init_PageMem(int n, int m);
/*
 * This function reads from RAM and returns the contents
 * of RAM_Start at location *location*
 */
WORDBYTES ReadRAM(u_int32_t location);

/*
 * This function writes value *Value* to location *Physical_Address*
 */
void WriteRAM(u_int32_t Value, u_int32_t Physical_Address);
void WriteLogical(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID);
WORDBYTES ReadLogical(u_int8_t rand1,u_int8_t rand2, u_int8_t PID);
u_int32_t RemovePages(u_int32_t Base,u_int32_t Limit);
u_int32_t RemoveLeastUsedPage(void);
void RAMDump();
void printfaults(void);
#endif /* PAGEMEM_H_ */















