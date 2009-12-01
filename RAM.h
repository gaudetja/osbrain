/*
 * PageMem.h
 *
 *      Author: Zach Norris
 *      	Gary Jordan
 *      	Joe Gaudet
 */

#ifndef PAGEMEM_H_
#define PAGEMEM_H_


typedef struct {
	u_int32_t value;
	char v;
}cell;


/*
 * This function reads from RAM and returns the contents
 * of RAM_Start at location *location*
 */
WORDBYTES ReadRAM(u_int32_t location);

/*
 * This function writes value *Value* to location *Physical_Address*
 */
void WriteRAM(u_int32_t Value, u_int32_t Physical_Address);
WORDBYTES WriteLogical(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID);
WORDBYTES ReadLogical(u_int8_t rand1,u_int8_t rand2, u_int8_t PID);


#endif /* PAGEMEM_H_ */
