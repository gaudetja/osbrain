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

static curlyqueue_t * rq;
static curlyqueue_t * bq;
static except_t* e;

void buildq(void)
{
	rq = curlyqueue_create_queue();
	bq = curlyqueue_create_queue();
}

int readyq(u_int8_t* pPID, char io)
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

int blockq(u_int8_t * pPID, int io)
{
	u_int8_t * pid;
	if (io == 0) {
		curlyqueue_iterator_jump_to_front(bq,e);
		while (*pid != *pPID) {
			pid = curlyqueue_get_value_at_iterator(bq,e);
			
		}
	}
	else {
		curlyqueue_enqueue(bq, pPID);
	}
}
