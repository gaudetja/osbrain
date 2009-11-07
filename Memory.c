/*
 *          File:
 *        Author: Gary S. Jordan
 *                Zach Norris
 *                Joe Gaudet
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
static u_int16_t Memory_Num = 0;


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
        static int PID=0;
        int FileComplete=0;
        u_int16_t Program_Length;
        Memory_Start=malloc(RAM);
        char buff[64]={0};
        char tempbuff[4];
        WORDBYTES CurrentWord;

        fgets(buff,64,stdin);
        if(strncmp(buff,"BRAIN09",7)==0)
        {
                while (strncmp(buff,"DATA",4) != 0) {
                	fgets(buff,64,stdin);
                	Program_Length++;
                	if (Program_Length > (RAM / 4 - Memory_Num)) {
                		printf("Insufficient Memory");
                		return -1;
                	}
                }
        	while (1)
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
                                fgets(buff,64,stdin);
                        }
                        tempbuff[3]=buff[0];
                        tempbuff[2]=buff[1];
                        tempbuff[1]=buff[2];
                        tempbuff[0]=buff[3];

                        Memory_Start[Memory_Num]=*((u_int32_t*)tempbuff);
                        i++;
                        Memory_Num++;
			if(Memory_Num>RAM/4)
			{
				printf("Insufficient Memory");
				return -1;
			}
                }
                Memory_End=&Memory_Start[Memory_Num];
                for (i=0;i<(Memory_End-Memory_Start);i++) //For Debugging
                {
                        CurrentWord.word=Memory_Start[i];
                        printf("%x\n",CurrentWord.word);
                }
        }
        else
                fprintf(stderr,"Brain09 Syntax Error\n");
        return PID;
}


/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param IC
 *
 *return CurrentWord; (16byte word)
 */
WORDBYTES GetInstruction(u_int16_t IC,u_int8_t PID)
{
        WORDBYTES CurrentWord;
        CurrentWord.word=Memory_Start[IC+PID*100];
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
        if (ReadWord.word==0)
        {
                ReadWord.bytes.byte1='0';
                ReadWord.bytes.byte2='0';
                ReadWord.bytes.byte3='0';
                ReadWord.bytes.byte4='0';
        }

        u_int32_t Temp1=(ReadWord.bytes.byte1-'0')*1000+
                        (ReadWord.bytes.byte2-'0')*100+
                        (ReadWord.bytes.byte3-'0')*10+
                        (ReadWord.bytes.byte4-'0');
        ReadWord.word=Temp1;
        return ReadWord;

}
void CopyMemory(u_int8_t rand1,u_int8_t rand2,u_int8_t PID1,u_int8_t PID2)
{
//      int temp1=rand1*10+rand2+PID1*100;
//      int temp2=rand1*10+rand2+PID2*100;
//      printf("Memory Address %d :%d\n ",temp1,Memory_Start[rand1*10+rand2+PID1*100]);
//      printf("Memory Address %d :%d\n ",temp2,Memory_Start[rand1*10+rand2+PID2*100]);
        Memory_Start[rand1*10+rand2+PID1*100]=Memory_Start[rand1*10+rand2+PID2*100];
//      printf("Memory Address %d :%d\n ",temp1,Memory_Start[rand1*10+rand2+PID1*100]);
//      printf("Memory Address %d :%d\n ",temp2,Memory_Start[rand1*10+rand2+PID2*100]);
}
/*
 *Write a specified value to memory location(rand1 concatinaReadWord.bytes.byte3-48ted with rand2)
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
void GetData(u_int8_t rand1, u_int8_t rand2, u_int8_t PID)
{
        char buff[1024]={0};
        char tempbuff[4];
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
                Memory_Start[(rand1-48)*10+i+PID*100]=*((u_int32_t*)tempbuff);
                printf("Memory After Get Data (%d), %x\n",rand1-48,Memory_Start[rand1*10+rand2+i+PID*100]);
        }

}

/*
 *Prints all 100 bytes of memory on in blocks of 10
 */
void MemoryDump(u_int16_t PID)
{
        int i;
        for (i=0;i<10;i++)
                printf("Memory %02d:%02d   %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x\n",i*10+PID*100,i*10+9+PID*100, Memory_Start[i*10+0+PID*100],Memory_Start[i*10+1+PID*100],Memory_Start[i*10+2+PID*100],Memory_Start[i*10+3+PID*100],Memory_Start[i*10+4+PID*100],Memory_Start[i*10+5+PID*100],Memory_Start[i*10+6+PID*100],Memory_Start[i*10+7+PID*100],Memory_Start[i*10+8+PID*100],Memory_Start[i*10+9+PID*100]);
}

