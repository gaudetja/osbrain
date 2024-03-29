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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "Disk.h"
#include "Exec_Brain.h"
#include "curlyqueue.h"
#include "sched.h"

curlyqueue_t * rq;			//variables for ready queue
static curlyqueue_t * bq;			//blocked queue
curlyqueue_t * hq;			//blocked queue
curlyqueue_t * sq;			//blocked queue
static except_t e;				//exceptions for curlyqueue

void buildq(void)
{
	rq = curlyqueue_create_queue();		//create queues
	bq = curlyqueue_create_queue();
	hq = curlyqueue_create_queue();
	sq = curlyqueue_create_queue();
}

int readyq(u_int8_t* pPID, char io)		//address of PID and I/O
						//input to io: 1 = push
						//	       0 = pop
{
	u_int8_t * pid;
	if (io == 0) {
		pid = curlyqueue_dequeue(rq,&e);
		return *pid;
	}
	else {
		curlyqueue_enqueue(rq, pPID);
	}
	return -1;
}

int blockq(u_int8_t * pPID, int io)					//address of PID and I/O
{									// input to io: 1 = push
	u_int8_t * pid;							// 0 = pop
	if (io == 0) {
		curlyqueue_iterator_jump_to_front(bq);
		do
		{
			pid = curlyqueue_get_value_at_iterator(bq,&e);
			if(*pid != *pPID) curlyqueue_iterator_step_backward(bq,&e);
		}while (*pid != *pPID);
		curlyqueue_delete_value_at_iterator(bq,&e);		//pop that element
		return *pid;						//return the PID
	}
	else {
		curlyqueue_enqueue(bq, pPID);

	}
	return -1;
}
MemBlock holesq(MemBlock * pSpace, int io)		//address of PID and I/O
{						//input to io: 1 = push
	MemBlock * Space;				//	       0 = pop
	if (io == 0) {
		Space = curlyqueue_dequeue(hq,&e);
		return *Space;
	}
	else {
		curlyqueue_enqueue(hq, pSpace);
	}
	return *pSpace;
}
MemBlock sizeq(MemBlock * pSize, int io)		//address of PID and I/O
{							//input to io: 1 = push
	MemBlock * Size;				//	       0 = pop
	if (io == 0) {
		Size = curlyqueue_dequeue(sq,&e);
		return *Size;
	}
	else {
		curlyqueue_enqueue(sq, pSize);
	}
	return *pSize;
}

