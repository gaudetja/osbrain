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

#include "Memory.h"
#include "Exec_Brain.h"
#include "curlyqueue.h"
#include "sched.h"

static curlyqueue_t * rq;			//variables for ready queue
static curlyqueue_t * bq;			//blocked queue
static except_t* e;				//exceptions for curlyqueue

void buildq(void)
{
	rq = curlyqueue_create_queue();		//create queues
	bq = curlyqueue_create_queue();
}

int readyq(u_int8_t* pPID, char io)		//address of PID and I/O
						//input to io: 1 = push
						//	       0 = pop
{
	u_int8_t * pid;
	if (io == 0) {
		pid = curlyqueue_dequeue(rq,e);
		return *pid;
	}
	else {
		curlyqueue_enqueue(rq, pPID);
	}
}

int blockq(u_int8_t * pPID, int io)		//address of PID and I/O
						//input to io: 1 = push
						//	       0 = pop
{
	u_int8_t * pid;
	if (io == 0) {
		//curlyqueue_iterator_jump_to_front(bq,e);
		while (*pid != *pPID) {
			pid = curlyqueue_get_value_at_iterator(bq,e);
			curlyqueue_iterator_step_forward(bq,e);
		}
	}
	else {
		curlyqueue_enqueue(bq, pPID);
	}
}
