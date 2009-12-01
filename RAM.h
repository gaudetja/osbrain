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

/*
 * This function reads from RAM and returns the contents
 * of RAM_Start at location *location*
 */
WORDBYTES ReadRAM(u_int32_t location);

#endif /* PAGEMEM_H_ */
