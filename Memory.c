/*
 *          File:
 *        Author: Gary S. Jordan
 *		  Eric Payne
 *		  Jered Philippon
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
#define RAM 4*10000
#define L2 4*100

//Memory Allocated
static u_int32_t* Memory_Start;
static u_int32_t* Memory_End;


/*
 *Executes memory allocation from stdin.
 *This stores all input in memory.
 *@param *argv
 *
 *return 0; (always)
 */
int ProgramWrite(char* argv)
{
	int i=0;
	int PID=0;
	int FileComplete=0;;
	Memory_Start=malloc(RAM);
	char buff[64]={0};
	char tempbuff[4];
	WORDBYTES CurrentWord;

	fgets(buff,64,stdin);
	if(strncmp(buff,"BRAIN09",7)==0)
	{
		while(1)
		{

			fgets(buff,64,stdin);
			if(strncmp(buff,"DATA",4)==0)
			{
				FileComplete=1;
				break;
			}
			if(strncmp(buff,"BRAIN09",7)==0)
			{
				PID++;
				i=0;
			}
			tempbuff[3]=buff[0];
			tempbuff[2]=buff[1];
			tempbuff[1]=buff[2];
			tempbuff[0]=buff[3];

			Memory_Start[(PID)*100+i]=*((u_int32_t*)tempbuff);
			i++;
				if(i>RAM/4)
				{
					printf("Program Too large");
					exit(1);
				}
		}
		Memory_End=&Memory_Start[(PID)*100+i];
		for (i=0;i<(Memory_End-Memory_Start);i++) //For Debugging
		{
			CurrentWord.word=Memory_Start[i];
			printf("%x\n",CurrentWord.word);
		}
	}
	else
		fprintf(stderr,"You Suck\n");
	return PID;
}


/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param IC
 *
 *return CurrentWord; (16byte word)
 */
WORDBYTES GetInstruction(u_int16_t IC)
{
	WORDBYTES CurrentWord;
	CurrentWord.word=Memory_Start[IC];
	return CurrentWord;
}

/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param rand1
 *@param rand2
 *CurrentWord.word=Memory_Start[i];
			printf("%x\n",CurrentWord.word);
 *return ReadWord; (16byte word)
 */
WORDBYTES ReadMemory(u_int8_t rand1,u_int8_t rand2,u_int8_t PID)
{
	int location;
	WORDBYTES ReadWord;

	location=(rand1)*10 + (rand2)+ PID*100;
	ReadWord.word=Memory_Start[location];


	u_int32_t Temp1=(ReadWord.bytes.byte1-48)*1000+
			(ReadWord.bytes.byte2-48)*100+
			(ReadWord.bytes.byte3-48)*10+
			(ReadWord.bytes.byte4-48);
	ReadWord.word=Temp1;
	return ReadWord;

}

/*
 *Write a specified value to memory location(rand1 concatinated with rand2)
 *@param Value
 *@param rand1
 *@param rand2
 */
void WriteMemory(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID)
{
	u_int16_t location=(rand1)*10 + (rand2)+PID*100;
	Memory_Start[location]=Value;
	return;

}

/*
 *Gets memory from data location
 *@param rand1
 */
void GetData(u_int8_t rand1, u_int8_t rand2)
{
	char buff[1024]={0};
	char tempbuff[3];
	int i;
	fgets(buff,1024,stdin);
	if(strncmp(buff,"END",3)==0)
		return;

	for (i=9;i>=0;i--)
	{
		tempbuff[3]=buff[i*4];
		tempbuff[2]=buff[i*4+1];
		tempbuff[1]=buff[i*4+2];
		tempbuff[0]=buff[i*4+3];
		Memory_Start[(rand1-48)*10+i]=*((u_int32_t*)tempbuff);
		printf("Memory After Get Data (%d), %x\n",rand1-48,Memory_Start[50+i]);
	}

}

/*
 *Prints all 100 bytes of memory on in blocks of 10
 */
void MemoryDump(u_int16_t PID)
{
	int i;
	for (i=0;i<10;i++)
		printf("Memory %2d:%2d   %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x\n",i*10+PID*100,i*10+9+PID*100, Memory_Start[i*10+0+PID*100],Memory_Start[i*10+1+PID*100],Memory_Start[i*10+2+PID*100],Memory_Start[i*10+3+PID*100],Memory_Start[i*10+4+PID*100],Memory_Start[i*10+5+PID*100],Memory_Start[i*10+6+PID*100],Memory_Start[i*10+7+PID*100],Memory_Start[i*10+8+PID*100],Memory_Start[i*10+9+PID*100]);
}

