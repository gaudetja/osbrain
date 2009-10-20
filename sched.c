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
#include "sched.h"

static curlyqueue_t * rq;
static curlyqueue_t * bq;

int buildq(void)
{
	rq = curlyqueue_create_queue();
	bq = curlyqueue_create_queue();
}

int readyq(void * pPID, char io)
{
	void * pid;
	if (io == 0) {
		pid = curlyqueue_dequeue(rq);
		return *pid;
	}
	else {
		curlyqueue_enqueue(rq, pPID);
	}
}

int blockq(void * PID, int io, curlyqueue_t * bq)
{
	void * pid;
	if (io == 0) {
		
	}
	else {
		curlyqueue_enqueue(bq, pPID);
	}
}
