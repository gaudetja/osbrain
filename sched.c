/*
 *          File: sched.c
 *        Author: Gary S. Jordan
 *		  Zach Norris
 *		  Joe Gaudet
 * Last Modified:
 *         Topic:
 * ----------------------------------------------------------------
 *
 */
#include "sched.h"

void * sched(struct ProcessControl * PCB, int rb, int io, curlyqueue_t * queue)
{
	void * pid;
	if (io == 0) {
		pid = curlyqueue_dequeue(queue, PCB);
		return pid;
	}
}
