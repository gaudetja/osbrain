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
#include "Disk.h"
#include "curlyqueue.h"
#include "RAM.h"
#include "Exec_Brain.h"
#include "sched.h"

int * RAM_Start;
int * RAM_End;





void Init_PageMem(int n, int m)
{
	RAM_Start = calloc(n,m);
}

WORDBYTES ReadRAM(u_int32_t location)
{
	WORDBYTES ReadWord;

	ReadWord.word = RAM_Start[location];
	if (ReadWord.word == 0) {
		ReadWord.bytes.byte1 = '0';
		ReadWord.bytes.byte2 = '0';
		ReadWord.bytes.byte3 = '0';
		ReadWord.bytes.byte4 = '0';
	}
	if (ReadWord.bytes.byte1 <= 0x39) {
		u_int32_t Temp1 = (ReadWord.bytes.byte1 - '0') * 1000 +
				  (ReadWord.bytes.byte2 - '0') * 100 +
				  (ReadWord.bytes.byte3 - '0') * 10 +
				  (ReadWord.bytes.byte4 - '0');
		ReadWord.word = Temp1;
	}
	return ReadWord;
}

WORDBYTES WriteLogical(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID)
{
;


}

WORDBYTES ReadLogical(u_int8_t rand1,u_int8_t rand2, u_int8_t PID)
{
;

}

void WriteRAM(u_int32_t Value, u_int32_t Physical_Address)
{
	RAM_Start[Physical_Address]=Value;
	return;
}

