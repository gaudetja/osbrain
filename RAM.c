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
cell * PageTable;

int numpages;
int pagesize;




void Init_PageMem(int n, int m)
{
	int i;
	RAM_Start = calloc(n,m);
	RAM_End = &RAM_Start[n*m-1];
	PageTable = calloc(n,sizeof(cell));
	numpages = n;
	pagesize = m;
	for (i=0;i<n*m;i++) {
		PageTable[i]->v=0;
		PageTable[i]->value=0;
	}
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
	WORDBYTES returnval;


	return returnval;

}

WORDBYTES ReadLogical(u_int8_t rand1,u_int8_t rand2, u_int8_t PID)
{
	/*int i = rand1*10+rand2;
	WORDBYTES returnval;
	if (PageTable[i]->v==1) {
		//ReadRam();
	}
	else {
		//ReadDisk();
	}
	return returnval;*/
}

void InsertPage(u_int32_t Logical_Address)
{
	Memory_Start[(Logical_Address / pagesize) * pagesize];
}

void RemovePage()
{

}

void WriteRAM(u_int32_t Value, u_int32_t Physical_Address)
{
	RAM_Start[Physical_Address]=Value;
}
void CopyRAM(u_int8_t rand1,u_int8_t rand2,u_int8_t BR1,u_int8_t BR2)
{
//      int temp1=rand1*10+rand2+PID1*100;
//      int temp2=rand1*10+rand2+PID2*100;
//      printf("Memory Address %d :%d\n ",temp1,Memory_Start[rand1*10+rand2+PID1*100]);
//      printf("Memory Address %d :%d\n ",temp2,Memory_Start[rand1*10+rand2+PID2*100]);
	Memory_Start[rand1*10+rand2+BR1]=Memory_Start[rand1*10+rand2+BR2];
//      printf("Memory Address %d :%d\n ",temp1,Memory_Start[rand1*10+rand2+PID1*100]);
//      printf("Memory Address %d :%d\n ",temp2,Memory_Start[rand1*10+rand2+PID2*100]);
}

/*
 *Write a specified value to memory location(rand1 concatinaReadWord.bytes.byte3-48ted with rand2)
 *@param Value
 *@param rand1
 *@param rand2
 */
void DeletePage()
{


}
