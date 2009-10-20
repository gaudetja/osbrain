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

int readyq(void * pPID, char io, curlyqueue_t * rq);
void * blockq(struct ProcessControl * PCB, int io, curlyqueue_t * bq);

#endif
