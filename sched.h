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

int readyq(void * pPID, char io);
void * blockq(struct ProcessControl * PCB, int io);
void buildq(void);

#endif
