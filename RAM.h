/*
 * PageMem.h
 *
 *      Author: Zach Norris
 *      	Gary Jordan
 *      	Joe Gaudet
 */

#ifndef RAM_H_
#define RAM_H_


typedef struct {
	u_int32_t value;
	u_int32_t count;
	char v; //1 for valid, 0 otherwise
}cell;


/*
 * This function reads from RAM and returns the contents
 * of RAM_Start at location *location*
 */
void Init_PageMem(int n, int m);
WORDBYTES ReadRAM(u_int32_t location);

/*
 * This function writes value *Value* to location *Physical_Address*
 */
void WriteRAM(u_int32_t Value, u_int32_t Physical_Address);
WORDBYTES WriteLogical(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID);
WORDBYTES ReadLogical(u_int8_t rand1,u_int8_t rand2, u_int8_t PID);
void CopyRAM(u_int8_t rand1,u_int8_t rand2,u_int8_t BR1,u_int8_t BR2);

#endif /* PAGEMEM_H_ */
