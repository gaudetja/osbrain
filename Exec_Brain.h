#ifndef _EXECBRAIN_H
#define _EXECBRAIN_H

/*
 *Process control block(P    //request to send
        //send when ready, block if not
        //unblockCB) holds the current register, stack pointer, toggle(flag) and instruction counter
 */
typedef struct ProcessControl
{
	u_int32_t R;
	u_int32_t SP;
	u_int8_t C;
	u_int16_t IC;
	u_int8_t PID;
	u_int32_t BR;
	u_int32_t LR;
	u_int8_t Block;
	u_int32_t* MailBox_Start;
	u_int32_t* MailBox_End;
} PCB;

/*
 *holds two 8 bit
 */



extern PCB* PCB_Array;

typedef struct bytes2
{
	u_int8_t byte2;
	u_int8_t byte1;
} BYTES2;

/*
 *Has one 16 bit word and two 8 bit words using bytes. These are used to "split the instruction up.
 */
typedef union inttobytes
{
	u_int16_t twobytes;
	BYTES2 bytes;
} OPERATOR;

/*control
 *Invokes the main loop which reads, executes the operations and writes back to memory
 */
int Exec_Brain(char NPID);

/*
 *Takes a 8 byte operator standing for two ANSI letter:
 *@param rator
 *
 *Also takes two words as the operands:
 *@param rand1
 *@param rand2
 */
void Instruction(u_int16_t rator,u_int8_t rand1,u_int8_t rand2);

/*
 *Push PCB.R(the register) data onto the stack
 */
void RegToStack();

/*
 *Pop the stack to PCB.R(the register)
 */
void StackToReg();

/*
 *Load a place in memory to the register
 *@param rand1
 *
 *@param rand2
 *
 */
void LoadRegister(u_int8_t rand1,u_int8_t rand2);

/*
 *If the number in the register is equal to the number specified by the parameters, then set toggle to true, else set to false
 *@param rand1
 *
 *@param rand2
 *
 */
void CompareEqual(u_int8_t rand1,u_int8_t rand2);

/*
 *If the number in the register is less than the number specified by the parameters, then set toggle to true, else set to false
 *@param rand1
 *
 *@param rand2
 *
 */
void CompareLess(u_int8_t rand1,u_int8_t rand2);

/*
 *If the toggle is true then PBC.IC(instruction counter) gets the value specified
 *@param rand1
 *
 *@param rand2
 */
void BranchTrue(u_int8_t rand1,u_int8_t rand2);

/*
 *PBC.IC(instruction counter) gets the value specified
 *@param rand1
 *
 *@param rand2
 */
void BranchUnc(u_int8_t rand1,u_int8_t rand2);

/*
 *Adds the number represented by the two parameter words in PCB.R(the register)
 *@param rand1
 *
 *@param rand2
 */
void AddToReg(u_int8_t rand1,u_int8_t rand2);

/*
 *Adds the value of the top number on the stack to the next number and stores it in place of the top number
 */
void AddStack();

/*
 *Multiplies the value of the top number on the stack by the next number and stores it in place of the top number
 */
void MultStack();

/*
 *Calculates the difference of the top number on the stack and the next number and stores it in place of the top number
 */
void SubStack();

/*
 *Divides the value of the top number on the stack by the next number and stores it in place of the top number
 */
void DivStack();

/*
 *Divides the number in PCB.R(the register) by the number represented by the two words
 *@param rand1
 *
 *@param rand2
 */
void RegDivide(u_int8_t rand1,u_int8_t rand2);

/*
 *Subtracts the number represented by the two parameter words from PCB.R(the register)
 *@param rand1
 *
 *@param rand2
 */
void RegSubtract(u_int8_t rand1,u_int8_t rand2);

/*
 *Multiplies the number in PCB.R(the register) by the number represented by the two words
 *@param rand1
 *
 *@param rand2
 */
void RegMultiply(u_int8_t rand1,u_int8_t rand2);

/*
 *Stores what is in PCB.R(the register) to memory
 *@param rand1
 *
 *@param rand2
 */
void StoreReg(u_int8_t rand1,u_int8_t rand2);

/*
 *Gives the value of PCB.R(the register) to where the stack pointer is pointing to
 */
void RegToSP();

/*
 *Gives the value of where the stack pointer is pointing to PCB.R(the register)
 */
void SPToReg();

/*
 *Reads data from memory and prints it from location
 *@param rand1
 */
void PutData(u_int8_t rand1);

/*
 *Prints status of memory block and process control block
 */
void LoadLow(u_int8_t rand1,u_int8_t rand2);

void LoadHigh(u_int8_t rand1,u_int8_t rand2);

void Send(u_int8_t rand1,u_int8_t rand2);
//sends data to PID starting with location in R
void Rec(u_int8_t rand1,u_int8_t rand2);
//gets data from PID and stores starting from location in R


void printstatus();


#define COMBINE(a,b) (((u_int16_t) a << 8) | b )
#define ISTR_LL COMBINE('L','L')
#define ISTR_LR COMBINE('L','R')
#define ISTR_LH COMBINE('L','H')
#define ISTR_SR COMBINE('S','R')
#define ISTR_SP COMBINE('S','P')
#define ISTR_PS COMBINE('P','S')
#define ISTR_PH COMBINE('P','H')
#define ISTR_PP COMBINE('P','P')
#define ISTR_CE COMBINE('C','E')
#define ISTR_CL COMBINE('C','L')
#define ISTR_BT COMBINE('B','T')
#define ISTR_BU COMBINE('B','U')
#define ISTR_GD COMBINE('G','D')
#define ISTR_PD COMBINE('P','D')
#define ISTR_AD COMBINE('A','D')
#define ISTR_SU COMBINE('S','U')
#define ISTR_MU COMBINE('M','U')
#define ISTR_DI COMBINE('D','I')
#define ISTR_AS COMBINE('A','S')
#define ISTR_SS COMBINE('S','S')
#define ISTR_MS COMBINE('M','S')
#define ISTR_DS COMBINE('D','S')
#define ISTR_NP COMBINE('N','P')
#define ISTR_H COMBINE('H',' ')
#define ISTR_HN COMBINE('H',10)
#define ISTR_SD COMBINE('S','D')
#define ISTR_RC COMBINE('R','C')


#endif
