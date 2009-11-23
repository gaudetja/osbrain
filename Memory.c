/*
 *	  File:
 *	Author: Gary S. Jordan
 *		Zach Norris
 *		Joe Gaudet
 * Last Modified:
 *	 Topic:
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

#include "curlyqueue.h"
#include "Memory.h"
#include "Exec_Brain.h"
#include "sched.h"

#define RAM 4*10000
#define L2 4*100

extern curlyqueue_t * hq;
extern curlyqueue_t * sq;

//Memory Allocated
u_int32_t* Memory_Start;					//Start of memory block
u_int32_t* Memory_End;					//End of memory block
u_int16_t Memory_Num = 0;				//number of elements in memory block
u_int8_t* Memory_Avail_Current;
u_int8_t* Memory_Avail_Start;
u_int8_t* Memory_Avail_End;
MemBlock* Spaces;
u_int8_t Holes=0;





/*
 *Executes memory allocation from stdin.
 *This stores all input in memory.
 *@param *argv
 *
 *return 0; (always)
 */
int ProgramWrite(u_int16_t* Program_Length)
{
	u_int16_t i = Memory_Num;				//store old Memory_Num for later
	//static int PID
	int FileComplete=0;
	char buff[64]={0};
	char tempbuff[4];
	int lengthbuff[4];
	WORDBYTES CurrentWord;

	Memory_Start = malloc(RAM);
	Memory_Avail_Start = malloc(RAM/4);
	Spaces=malloc(100*7);
	buildq();


	Memory_Avail_End =	Memory_Avail_Start+RAM/4;
	Memory_Avail_Current = Memory_Avail_Start;
	for (i=0;i<RAM/4;i++)
		Memory_Avail_Start[i]=1;

	fgets(buff,64,stdin);					//get first line
	if(strncmp(buff,"BRAIN09",7) == 0)			//make sure it's legit
	{
		fgets(buff,64,stdin);
		lengthbuff[3]=(buff[0]-48)*1000;			//Switch the bytes around
		lengthbuff[2]=(buff[1]-48)*100;
		lengthbuff[1]=(buff[2]-48)*10;
		lengthbuff[0]=(buff[3]-48)*1;

		*Program_Length=lengthbuff[0]+lengthbuff[1]+lengthbuff[2]+lengthbuff[3];
		for (i=0;i<*Program_Length;i++)
			Memory_Avail_Start[i]=0;
		Memory_Avail_Current=Memory_Avail_Start+*Program_Length;


		Spaces[Holes].Size=RAM/4-*Program_Length;
		Spaces[Holes].Location=*Program_Length;
		Spaces[Holes].Num=Holes;
		holesq(&Spaces[Holes], 1);
		Holes++;

		while (1)
		{

			fgets(buff,64,stdin);				//get next line

			if(strncmp(buff,"DATA",4)==0)		//End of Program Instructions
			{
				FileComplete=1;
				break;
			}
			if(strncmp(buff,"BRAIN09",7)==0)	//Look for New Program
			{
				//PID++;
				//i=0;
				//fgets(buff,64,stdin);

				fprintf(stderr,"Brain09 Syntax Error\n");
				break;
			}
			if (strncmp(buff,"END",3) == 0) {
				FileComplete = 1;
				break;
			}
			tempbuff[3]=buff[0];			//Switch the bytes around
			tempbuff[2]=buff[1];
			tempbuff[1]=buff[2];
			tempbuff[0]=buff[3];

			Memory_Start[Memory_Num]=*((u_int32_t*)tempbuff);	//put instr in memory
			Memory_Num++;				//increment # bytes written
			if(Memory_Num>RAM/4)			//check against end of memory
			{
				printf("Insufficient Memory\n");
				return -1;
			}
		}
		//*Program_Length = Memory_Num - i;	//get program length from old Memory_Num val
		Memory_End=&Memory_Start[Memory_Num];
		for (i=0;i<(Memory_End-Memory_Start);i++) 	//For Debugging
		{
			CurrentWord.word=Memory_Start[i];
			printf("%x\n",CurrentWord.word);
		}
		return 0;
	}
	else	{
		fprintf(stderr,"Brain09 Syntax Error\n");
		return -1;
	}
}


/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param IC
 *
 *return CurrentWord; (16byte word)
 */
WORDBYTES GetInstruction(u_int16_t IC,u_int8_t BR)
{
	WORDBYTES CurrentWord;
	CurrentWord.word=Memory_Start[IC+BR];
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
WORDBYTES ReadMemory(u_int8_t rand1,u_int8_t rand2,u_int8_t BR)
{
	int location;
	WORDBYTES ReadWord;

	location=(rand1)*10 + (rand2)+ BR;
	ReadWord.word=Memory_Start[location];
	if (ReadWord.word==0)
	{
		ReadWord.bytes.byte1='0';
		ReadWord.bytes.byte2='0';
		ReadWord.bytes.byte3='0';
		ReadWord.bytes.byte4='0';
	}
	if (ReadWord.bytes.byte1 <= 0x39) {
		u_int32_t Temp1=(ReadWord.bytes.byte1-'0')*1000+
				(ReadWord.bytes.byte2-'0')*100+
				(ReadWord.bytes.byte3-'0')*10+
				(ReadWord.bytes.byte4-'0');
		ReadWord.word=Temp1;
	}


	return ReadWord;

}
void CopyMemory(u_int8_t rand1,u_int8_t rand2,u_int8_t BR1,u_int8_t BR2)
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
void WriteMemory(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t BR)
{
	u_int16_t location=(rand1)*10+(rand2)+BR;
	Memory_Start[location]=Value;
	return;

}

/*
 *Gets memory from data location
 *@param rand1
 */
void GetData(u_int8_t rand1, u_int8_t rand2, u_int8_t BR)
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
		Memory_Start[(rand1-48)*10+i+BR]=*((u_int32_t*)tempbuff);
		printf("Memory After Get Data (%d), %x\n",rand1-48,Memory_Start[rand1*10+rand2+i+BR]);
	}
}

u_int32_t RequestMemory(u_int16_t Req_Length,u_int8_t Mode)
{
	int i=0;				//Initialize Temp Variables
	int SpaceFound=0;
	MemBlock TempMemBlock;
	MemBlock TempMemBlock1;
	u_int32_t Space_A=0;
	if (Mode==0)
	{
		while(!Space_A)		//While Space Not Available
		{
			if (*Memory_Avail_Current==1) //Check Where Current Memory Index is pointing.
			{
				Space_A=1;				//If Memory Available, Check to see if Req_Length is available
				for (i=0;i<Req_Length;i++)
				{
					if (*(Memory_Avail_Current+i)==0)
					{
						Space_A=0;						//If any memory location unavailable, entire block unavailable.
						break;							//Break if a memory slot is not available
					}
				}
			}
			else
				Memory_Avail_Current++;					//If Current Available pointer is un-unuseable, increment
			if (Memory_Avail_Current>Memory_Avail_End)
				Memory_Avail_Current=Memory_Avail_Start;//If Current Avail Exceeds Memory Go to Start
		}

		Space_A=Memory_Avail_Current-Memory_Avail_Start;// Convert Virtual Memory to Disk Memory
		for (i=0;i<Req_Length;i++)						//Once Memory Avail Found - Block it off
		{
			*Memory_Avail_Current=0;					//Set Memory as Unavailable
			Memory_Avail_Current++;
		}
		return Space_A;									//Return Memory Location
	}
	else
	{
		u_int16_t Space_A=0;
		if (curlyqueue_is_empty(hq))					//Queue of Holes
		{
			for (i=0;i<Req_Length;i++)					//If Holes List Empty, take from end of memory
				*(Memory_Avail_Current+i)=0;			//Should not occur
			Space_A=Memory_Avail_Current-Memory_Avail_Start;
			Memory_Avail_Current=Memory_Avail_Current+Req_Length;
		}
		else
		{
			for (i=1;i<=Holes;i++)  //Given amount of holes
			{
				TempMemBlock=holesq(&Spaces[i-1], 0);  //Pull a hole off the queue
				if (TempMemBlock.Size<Req_Length)		 //Check to see if it meets the req length
				{
					Spaces[TempMemBlock.Num]=TempMemBlock;
					holesq(&Spaces[TempMemBlock.Num], 1);			 //If it doesn't, put it back in the holes list
				}
				else
				{
					sizeq(&TempMemBlock, 1);			 //If it does, put it in size queue (which is used to find best fit)
					SpaceFound=1;
				}
			}
			if (SpaceFound==1)							//If a hole was found that works
			{
				sizeq(&TempMemBlock, 0);				//Pull the hole
				while(!curlyqueue_is_empty(sq))			//If holes list not empty
				{
					sizeq(&TempMemBlock1, 0);			//Pull another hole
					if (TempMemBlock1.Size<TempMemBlock.Size) // Compare the two and push the worst fit back.
					{
						holesq(&TempMemBlock, 1);		//TempMemBlock1 is better fit, send back Temp and sort temp1 to temp
						TempMemBlock=TempMemBlock1;
					}
					else
					{
						holesq(&TempMemBlock1, 1);		//TempMemBlock is better, send back temp1 and repeat
					}
				}
				Space_A=TempMemBlock.Location;
				if (TempMemBlock.Size!=Req_Length)      //If not a perfect fit, use the remaining memory space to create a hole
				{
					TempMemBlock.Size=TempMemBlock.Size-Req_Length;
					TempMemBlock.Location=TempMemBlock.Location+Req_Length;
					Spaces[TempMemBlock.Num]=TempMemBlock;
					holesq(&Spaces[TempMemBlock.Num], 1);
				}


			}

			else  //If no memory space found
			{
				exit(1);
			}
		}
		return Space_A;
	}



}
void ReleaseMemory(u_int32_t BaseReg,u_int16_t LimitReg)
{
	int i;
	for (i=0;i<LimitReg;i++)
	*(Memory_Avail_Start+BaseReg+i)=1;

	Spaces[Holes].Size=LimitReg;
	Spaces[Holes].Location=BaseReg;
	Spaces[Holes].Num=Holes;
	holesq(&Spaces[Holes], 1);
	Holes++;
}


/*
 *Prints all 100 bytes of memory on in blocks of 10
 */
void MemoryDump(u_int16_t BR)
{
	int i;
	for (i=0;i<10;i++)
		printf("Memory %03d:%03d   %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x\n",BR+i*10,BR+10+i*10, Memory_Start[BR+0+i*10],Memory_Start[BR+1+i*10],Memory_Start[i*10+2+BR],Memory_Start[i*10+3+BR],Memory_Start[i*10+4+BR],Memory_Start[i*10+5+BR],Memory_Start[i*10+6+BR],Memory_Start[i*10+7+BR],Memory_Start[i*10+8+BR],Memory_Start[i*10+9+BR]);
}
void HoleStatusDump()
{
	printf("Number of Holes: %d\n",Holes);
}

