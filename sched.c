/*
 *          File: sched.c
 *        Author: Gary S. Jordan
 *		 		  Zach Norris
 *		  		  Joe Gaudet
 * Last Modified:
 *         Topic:
 * ----------------------------------------------------------------
 *
 */
//#include "Exec_Brain.h"
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

#include "Exec_Brain.h"
#include "sched.h"

PCB* sched(PCB* ControlBlock, int io)
{
/*	void * pid;
	if (io == 0) {
		pid = curlyqueue_dequeue(queue, PCB);
		return pid;
	}*/
}

