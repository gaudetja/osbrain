/*
 *          File: sched.c
 *        Author: Gary S. Jordan
*                                Zach Norris
*                                Joe Gaudet
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

static PCB* Queue_Start;
static PCB* Queue_End;
static PCB* Queue_Current;


void BuildQueue(char NPID)
{
        Queue_Start=&PCB_Array[0];
        Queue_Start--;
        Queue_End=&PCB_Array[(int)NPID];
        Queue_Current=Queue_Start;
}

u_int8_t sched(PCB* ControlBlock, int io)
{
		Queue_Current++;
		while(Queue_Current->Block!=0)
		{
			Queue_Current++;
			if (Queue_Current==Queue_End)
				Queue_Current=Queue_Start;
		}
		return Queue_Current->PID;

}

