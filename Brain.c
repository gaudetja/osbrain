/*
 *          File:
 *        Author: Gary S. Jordan
 *                Eric Payne
 *                Jered Philippon
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



int LoadProgram(char* argv);

/*
 *Main program that runs LoadProgram and Exec_Brain
 *@param argc
 *@param *argv
 *
 *return 0; (always)
 */
int main(int argc, char* argv[])
{
        //Loads Initial Program
        int NPID=LoadProgram(argv[1]);

        //Run the program
        Exec_Brain(NPID);

        return 0;
}

/*
 *Changes stdin to read from file,
 *@param *argv
 *
 *Also loads ProgramWrite(dumps the file to memory)
 */
int LoadProgram(char* argv)
{
        int fildes=open(argv,O_RDONLY);
	if (fildes==-1)
	{
		fprintf(stderr,"Program not loaded properly, check to see if input file exists");
		exit(1);
	}
        close(fileno(stdin));
        dup(fildes);
        int NPID=ProgramWrite(argv);
        return NPID;
}


