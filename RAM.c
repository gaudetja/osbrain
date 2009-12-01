/*
 * PageMem.c
 *
 *      Author: Zach Norris
 *      	Gary Jordan
 *      	Joe Gaudet
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

#include "curlyqueue.h"
#include "RAM.h"
#include "Exec_Brain.h"
#include "sched.h"

int * RAM_Start;
int * RAM_End;



Init_PageMem(int n, int m)
{
	RAM_Start = calloc(n,m);


}

Load_Logical()
{



}

Store_Logical()
{


}

