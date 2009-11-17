#ifndef _SCHED_H
#define _SCHED_H
/*
 *          File: sched.h
 *        Author: Gary S. Jordan
 *		  Zach Norris
 *		  Joe Gaudet
 * Last Modified:
 *         Topic:
 * ----------------------------------------------------------------
 *
 */

int readyq(u_int8_t * pPID, char io);
int blockq(u_int8_t * PID, int io);
MemBlock holesq(MemBlock * pSpace, int io);
MemBlock sizeq(MemBlock * pSpace, int io);
void buildq(void);

#endif
