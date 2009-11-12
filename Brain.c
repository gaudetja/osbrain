/*
 *          File:
 *        Author: Gary S. Jordan
 *                Eric Payne
 *                Jered Philippon
 * Last Modified:
 *         Topic:
 * ----------------------------------------------------------------
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


int numPID = 0;
int LoadProgram(char* argv,  u_int16_t* pProgram_Length );

/*
 *Main program that runs LoadProgram and Exec_Brain
 *@param argc
 *@param *argv
 *
 *return 0; (always)
 */
int main(int argc, char* argv[])
{
	u_int16_t* pProgram_Length;
        //Loads Initial Program
        LoadProgram(argv[1] , pProgram_Length);
        //Run the program
        Exec_Brain(numPID , *pProgram_Length);

        return 0;
}

/*
 *Changes stdin to read from file,
 *@param *argv
 *
 *Also loads ProgramWrite(dumps the file to memory)
 */
int LoadProgram(char* argv , u_int16_t* pProgram_Length)
{
        int fildes=open(argv,O_RDONLY);
	if (fildes==-1)
	{
		fprintf(stderr,"Program not loaded properly, check to see if input file exists");
		return -1;
	}
        close(fileno(stdin));
        dup(fildes);
        ProgramWrite(pProgram_Length);
        return 0;
}
