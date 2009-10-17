#ifndef _MEMORY_H
#define _MEMORY_H
//testing mergermentism
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

/*
 *Executes memory allocation from stdin.
 *This stores all input in memory.
 *@param *argv
 *
 *return 0; (always)
 */
int ProgramWrite(char* argv);

/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param IC
 *
 *return CurrentWord; (16byte word)
 */
WORDBYTES ReadMemory(u_int8_t rand1,u_int8_t rand2);

/*
 *Reads from a specified memory location(rand1 concatinated with rand2)
 *@param rand1
 *@param rand2
 *
 *return ReadWord; (16byte word)
 */
void WriteMemory(u_int32_t Value, u_int8_t rand1,u_int8_t rand2);

/*
 *Write a specified value to memory location(rand1 concatinated with rand2)
 *@param Value
 *@param rand1
 *@param rand2
 */
WORDBYTES GetInstruction(u_int16_t IC);

/*
 *Gets memory from data location
 *@param rand1
 */
void GetData(u_int8_t rand1, u_int8_t rand2);

/*
 *Prints all 100 bytes of memory on in blocks of 10
 */


void MemoryDump();

#endif
