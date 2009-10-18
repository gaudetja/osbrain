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

static ReadyQueue;

u_int8_t sched(PCB* ControlBlock, int io)
{
			switch (io)
			{
				case 0:
				{


					break;
				}
				case 1:		break;
				default:	{fprintf(stderr,"Something Broke"); break;}
			}
		return 0;
}

