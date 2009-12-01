/*
 * PageMem.h
 *
 *  Created on: Nov 30, 2009
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
void WriteRAM(u_int32_t Value, u_int32_t Physical_Address);


#endif /* PAGEMEM_H_ */
