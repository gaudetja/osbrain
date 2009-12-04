#ifndef _DISK_H
#define _DISK_H

//This is used to convert 32byte words to 4 8byte words
typedef struct bytes4
{
	u_int8_t byte4;
	u_int8_t byte3;
	u_int8_t byte2;
	u_int8_t byte1;
} BYTES4;

//This is used with bytes4 to translate 32byte words to 4 8byte words
typedef union wordbytes
{
	u_int32_t word;
	BYTES4 bytes;
} WORDBYTES;


typedef struct Space
{
	u_int16_t Size;
	u_int32_t Location;
	u_int8_t Num;

} MemBlock;





/*
 *Executes memory allocation from stdin.
 *This stores all input in memory.
 *@param *argv
 *
 *return 0; (always)
 */
int ProgramWrite(u_int16_t* pProgram_Length);

/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param IC
 *
 *return CurrentWord; (16byte word)
 */

WORDBYTES ReadDisk(u_int8_t rand1,u_int8_t rand2, u_int8_t PID);

/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param rand1
 *@param rand2
 *
 *return ReadWord; (16byte word)
 */
void WriteDisk(u_int32_t Value, u_int8_t rand1,u_int8_t rand2, u_int8_t PID);

/*
 *Write a specified value to memory location(rand1 concatinated with rand2)
 *@param Value
 *@param rand1
 *@param rand2
 */


void GetData(u_int8_t rand1, u_int8_t rand2, u_int8_t PID);

/*
 *Prints all 100 bytes of memory on in blocks of 10
 */

void CopyDisk(u_int8_t rand1,u_int8_t rand2,u_int8_t BR1,u_int8_t BR2);
void DiskDump(u_int16_t PID);
void HoleStatusDump(void);

u_int32_t RequestMemory(u_int16_t Req_Length, u_int8_t Mode);
void ReleaseMemory(u_int32_t BaseReg,u_int16_t LimitReg);

#endif
