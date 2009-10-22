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
		curlyqueue_iterator_jump_to_front(bq);
<<<<<<< /usr/students/znorris/cos431/proj3/brain09/sched.c
		while (*pid != *pPID) {					//move through queue
			pid = curlyqueue_get_value_at_iterator(bq,e);	//looking for specified
			curlyqueue_iterator_step_forward(bq,e);		//PID in the queue
=======

		do
		{
			pid = curlyqueue_get_value_at_iterator(bq,e);
	//		printf("Looking for one and found:  %x\n",pid);
			curlyqueue_iterator_step_backward(bq,e);
>>>>>>> /tmp/sched.c~other.HHhAco
		}
<<<<<<< /usr/students/znorris/cos431/proj3/brain09/sched.c
		curlyqueue_delete_value_at_iterator(bq,e);		//pop that element
		return *pid;						//return the PID
=======
		while (*pid != *pPID);



>>>>>>> /tmp/sched.c~other.HHhAco
	}
	else {
		curlyqueue_enqueue(bq, pPID);
	//	printf("Put onto block: %x\n",pPID);
	}
}
