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

#include "sched.h"

int * RAM_Start;
int * RAM_End;
cell * PageTable;

int numpages;
int pagesize;
int Size_PT;



void Init_PageMem(int n, int m)
{
	int i;
	RAM_Start = calloc(n,m);
	RAM_End = &RAM_Start[n*m-1];
	PageTable = calloc(10000/m+1 ,sizeof(cell));
	Size_PT= 10000/m+1;
	numpages = n;
	pagesize = m;
	for (i=0;i<n*m;i++) {
		PageTable[i].v=0;
		PageTable[i].framenumber=0;
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

void WriteLogical(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID)
{


}

WORDBYTES ReadLogical(u_int8_t rand1,u_int8_t rand2, u_int8_t PID)
{
	int i = rand1*10+rand2;
	WORDBYTES returnval;
	if (PageTable[(i/numpages)].v==1) {
		returnval = ReadRAM(PageTable[i].framenumber+i%numpages);
	}
	else {
		//returnval = ReadDisk(rand1,rand2, Current_PCB.BR);
		//InsertPage(i);
	}
	return returnval;
}

/*void InsertPage(u_int32_t Logical_Address)
{
	int PhysNum = 0;
	int PageNum = 0;
	while (PageNum < numpages) {
		if ((PageTable[PageNum].value == PhysNum) && PageTable[PageNum].v) {
			PhysNum++;
			PageNum = -1;
		}
		PageNum++;
	}
	PageTable[(Logical_Address / pagesize) * pagesize].v = 1;
	PageTable[(Logical_Address / pagesize) * pagesize].value = PhysNum;
}*/



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
u_int32_t RemovePage(void)
{
	int i=0;
	u_int32_t Smallest;
	u_int32_t Temp;
	u_int32_t frame;
	int Index_Smallest=0;
	Smallest=PageTable[i].count;
	for (i=1;i<Size_PT;i++)
	{
		Temp=PageTable[i].count;
		if (Temp<Smallest)
		{
			Smallest=Temp;
			Index_Smallest=i;
		}
	}
	frame=PageTable[Index_Smallest].framenumber;
	for(i=0;i<pagesize;i++)
		Memory_Start[frame*pagesize+i]=RAM_Start[frame*pagesize+i];
	PageTable[Index_Smallest].v=0;
	return frame;


}
