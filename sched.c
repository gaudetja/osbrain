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




void EnQueue(char PID)
{

//  Add PID to Queue

}

char DeQueue()
{

//  Get Next PID

//	return PID;
}


u_int8_t sched(char PID)
{
	int Next_PID;
	if (PID!=101)
		EnQueue(PID);
	Next_PID=DeQueue();


}
void Block(int PID)
{

}
void Unblock(int PID)
{

}

