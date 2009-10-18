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

static char* Queue_Start;
static char* Queue_End;
static char* Queue_Current;


void BuildQueue(char NPID)
{
        Queue_Start=malloc(NPID);
        Queue_End=Queue_Start+NPID;
        Queue_Current=Queue_Start;
}

u_int8_t sched(PCB* ControlBlock, int io)
{
		PCB* test=&PCB_Array[1];
 //       Queue_Current++;
   //     if (ControlBlock->B==1);
     //   return PID;
}

