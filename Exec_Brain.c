/*
 *          File: exec_BRAIN.c
 *        Author: Gary S. Jordan
 *		  Eric Payne
 *		  Jered Philippon
 * Last Modified:
 *         Topic:
 * ----------------------------------------------------------------
 *
 */
//gary test
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



PCB* Current_PCB; 			//Current Process control block
WORDBYTES CurrentWord;		//Current 4 byte word read from memory
OPERATOR operator;			//Operator to be chosen from the list
WORDBYTES MemoryContents;	//4 byte word read from memory


/*
 *Invokes the main loop which reads, executes the operations and writes back to memory
 */
int Exec_Brain(int NPID)
{
	//initialize all the PCB variables to 0
	Current_PCB->R=0;
	Current_PCB->SP=0;
	Current_PCB->C=0;
	Current_PCB->IC=0;
	Current_PCB->PID=0;
	Current_PCB->LR=0;
	Current_PCB->BR=0;
	int TDMA=0;
	PCB* PCB_Array=malloc(sizeof(PCB)*NPID);
	int i=0;
	for (i=0;i<NPID;i++)
	{
		PCB_Array[i].PID=i;
		PCB_Array[i].IC=i*100;
	}
	while(1)
	{
		TDMA=0;
        //PID=sched(Current_PCB,0);   //Get next process from ready queue.
		//Current_PCB=PCB_Array[PID);
		while(TDMA<10)
		{
			CurrentWord=GetInstruction(Current_PCB->IC);											//gets instruction
			operator.bytes.byte1=CurrentWord.bytes.byte1;										//give operator 1 a value
			operator.bytes.byte2=CurrentWord.bytes.byte2;										//give operator 1 a value
			Instruction(operator.twobytes, CurrentWord.bytes.byte3, CurrentWord.bytes.byte4);	//Calls Instruction function
			TDMA++;
		}
		//sched(Current_PCB,1);  //Put Current Process on ready queue.
	}
return 0;
}

/*
 *Takes two 1 byte operators standing for ANSI letters:
 *@param rator
 *
 *Also takes two words as the operands:
 *@param rand1
 *@param rand2
 */
void Instruction(u_int16_t rator,u_int8_t rand1,u_int8_t rand2)
{
		Current_PCB->IC++;
		//instruction set
		switch (rator)
		{
			case ISTR_LR:  LoadRegister(rand1, rand2);	break;	// Load Register (LR)
			case ISTR_LL:  LoadLow(rand1, rand2);		break; 	// Load Low (LL)
			case ISTR_LH:  LoadHigh(rand1, rand2);		break; 	// Load High (LH)
			case ISTR_SR:  StoreReg(rand1,rand2); 		break;  // Stack Register (SR)
			case ISTR_SP:  RegToSP(); 					break;	// Copies the register to the Stack Pointer (SP)
			case ISTR_PS:  SPToReg();					break;	// Copies the Stack Pointer to the register (PS)
			case ISTR_PH:  RegToStack();				break;	// Push (PH)
			case ISTR_PP:  StackToReg(Current_PCB->SP); break;	// Pop (PP)
			case ISTR_CE:  CompareEqual(rand1,rand2);	break;  // Compare Equal (CE)
			case ISTR_CL:  CompareLess(rand1,rand2);	break;	// Compare Less (CL)
			case ISTR_BT:  BranchTrue(rand1,rand2);		break;	// Branch Conditional (BT)
			case ISTR_BU:  BranchUnc(rand1,rand2);		break;	// Branch Unconditional (BU)
			case ISTR_GD:  GetData(rand1,rand2); 		break;  // Get Data (GD)
			case ISTR_PD:  PutData(rand1); 				break;  // Put Data (PD)
			case ISTR_AD:  AddToReg(rand1,rand2); 		break;	// Add (AD)
			case ISTR_SU:  RegSubtract(rand1,rand2);	break;	// Subtract (SU)
			case ISTR_MU:  RegMultiply(rand1,rand2);	break;	// Multiply (MU)
			case ISTR_DI:  RegDivide(rand1,rand2);		break;	// Divide (DI)
			case ISTR_AS:  AddStack();					break;	// Add Stack (AS)
			case ISTR_SS:  SubStack();					break; 	// Subtract Stack (SS)
			case ISTR_MS:  MultStack(); 				break;	// Multiply Stack (MS)
			case ISTR_DS:  DivStack();					break;	// Divide Stack (DS)
			case ISTR_NP:								break;	// No-op (NP)
			case ISTR_H:   exit(1);								// Hault (H)
			case ISTR_HN:  exit(1);								// Hault (H)
			case ISTR_SD:  Send(rand1,rand2);			break;	// Send (SD)
			case ISTR_RC:  Rec(rand1,rand2);			break;	// Receive (RC)
			default:									break;
		}
		printstatus();
	return;
}

/*test
 *Push PCB.R(the register) data onto the stack
 */
void RegToStack()
{
	Current_PCB->SP=Current_PCB->SP+1;   								//Consider Adding a subroutine which converts ascii2dec and dec2ascii
	MemoryContents.bytes.byte4=(Current_PCB->R%10)+48;
	MemoryContents.bytes.byte3=((Current_PCB->R/10)%10)+48;
	MemoryContents.bytes.byte2=((Current_PCB->R/100)%10)+48;
	MemoryContents.bytes.byte1=((Current_PCB->R/1000)%10)+48;
	WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10));
}

/*
 *Pop the stack to PCB.R(the register)
 */
void StackToReg()
{
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	Current_PCB->R=MemoryContents.word;
	Current_PCB->SP--;
}

/*
 *Load a place in memory to the register
 *@param rand1
 *
 *@param rand2
 *
 */
void LoadRegister(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1-48,rand2-48);
	Current_PCB->R=MemoryContents.word;
}

/*
 *If the number in the register is equal to the number specified by the parameters, then set toggle to true, else set to false
 *@param rand1
 *
 *@param rand2
 *
 */
void CompareEqual(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1-48,rand2-48);
	if(Current_PCB->R==MemoryContents.word)
	{
		Current_PCB->C = 'T';
	}
	else {
		Current_PCB->C = 'F';
	}
}

/*
 *If the number in the register is less than the number specified by the parameters, then set toggle to true, else set to false
 *@param rand1
 *
 *@param rand2
 *
 */
void CompareLess(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1-48,rand2-48);
	if(Current_PCB->R<MemoryContents.word)
	{
		Current_PCB->C = 'T';
	}
	else {
		Current_PCB->C = 'F';
	}
}

/*
 *If the toggle is true then PBC.IC(instruction counter) gets the value specified
 *@param rand1
 *
 *@param rand2
 */
void BranchTrue(u_int8_t rand1,u_int8_t rand2)
{
	if (Current_PCB->C == 'T')
		Current_PCB->IC = (10*(rand1-48)+(rand2-48));
}

/*
 *PBC.IC(instruction counter) gets the value specified
 *@param rand1
 *
 *@param rand2
 */
void BranchUnc(u_int8_t rand1,u_int8_t rand2)
{
	Current_PCB->IC = (10*(rand1-48)+(rand2-48));
}

/*
 *Adds the number represented by the two parameter words in PCB.R(the register)
 *@param rand1
 *
 *@param rand2
 */
void AddToReg(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1,rand2);
	Current_PCB->R=Current_PCB->R+MemoryContents.word;
}

/*
 *Divides the number in PCB.R(the register) by the number represented by the two words
 *@param rand1
 *
 *@param rand2
 */
void RegDivide(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory (rand1-48,rand2-48);
	Current_PCB->R = (Current_PCB->R) / MemoryContents.word;
}

/*
 *Subtracts the number represented by the two parameter words from PCB.R(the register)
 *@param rand1
 *
 *@param rand2
 */
void RegSubtract(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory (rand1-48,rand2-48);
	Current_PCB->R = (Current_PCB->R) - MemoryContents.word;
}

/*
 *Multiplies the number in PCB.R(the register) by the number represented by the two words
 *@param rand1
 *
 *@param rand2
 */
void RegMultiply(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1-48,rand2-48);
	Current_PCB->R = (Current_PCB->R) * MemoryContents.word;
}

/*
 *Stores what is in PCB.R(the register) to memory
 *@param rand1
 *
 *@param rand2
 */
void StoreReg(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents.bytes.byte4=(Current_PCB->R%10)+48;
	MemoryContents.bytes.byte3=((Current_PCB->R/10)%10)+48;
	MemoryContents.bytes.byte2=((Current_PCB->R/100)%10)+48;
	MemoryContents.bytes.byte1=((Current_PCB->R/1000)%10)+48;
	WriteMemory(MemoryContents.word,rand1-48,rand2-48);
}

/*
 *Gives the value of PCB.R(the register) to where the stack pointer is pointing to
 */
void RegToSP()
{
	Current_PCB->SP = Current_PCB->R;

}

/*
 *Gives the value of where the stack pointer is pointing to PCB.R(the register)
 */
void SPToReg()
{
	Current_PCB->R = Current_PCB->SP;
}

/*
 *Reads data from memory and prints it from location
 *@param rand1
 */
void PutData(u_int8_t rand1)
{
	int i;
	for(i=0; i<10; i++){
		MemoryContents=ReadMemory(rand1-48, i);
		printf("%d ",MemoryContents.word);
	}
	printf("\n");

}

/*
 *Calculates the difference of the top number on the stack and the next number and stores it in place of the top number
 */
void SubStack()
{
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	MemoryContents.word=MemoryContents.word-Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10));

}

/*
 *Multiplies the value of the top number on the stack by the next number and stores it in place of the top number
 */
void MultStack()
{
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	MemoryContents.word=MemoryContents.word*Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10));

}

/*
 *Divides the value of the top number on the stack by the next number and stores it in place of the top number
 */
void DivStack()
{
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	MemoryContents.word=MemoryContents.word/Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10));

}

/*
 *Adds the value of the top number on the stack to the next number and stores it in place of the top number
 */
void AddStack()
{
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10));
	MemoryContents.word=MemoryContents.word+Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10));

}

/*
 *Prints status of memory block and process control block
 */
void printstatus()
{
	printf("PCB Status:  R:%d  SP:%d  IC:%d   C:%c\n", Current_PCB->R,Current_PCB->SP,Current_PCB->IC,Current_PCB->C);
	printf("Current Instr:  %c%c%c%c\n",CurrentWord.bytes.byte1,CurrentWord.bytes.byte2,CurrentWord.bytes.byte3,CurrentWord.bytes.byte4);
	MemoryDump();

}

void LoadHigh(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1-48,rand2-48);
	Current_PCB->R=(Current_PCB->R & 0x00FF);
	Current_PCB->R=(MemoryContents.word|0xFF00)&Current_PCB->R;
}

void LoadLow(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadMemory(rand1-48,rand2-48);
	Current_PCB->R=(Current_PCB->R & 0xFF00);
	Current_PCB->R=(MemoryContents.word|0x00FF) & Current_PCB->R;
}
void Send(u_int8_t rand1,u_int8_t rand2)
{
	//psuedo
	//request to send
	//send when ready, block if not
	//unblock
}
void Rec(u_int8_t rand1,u_int8_t rand2)
{
	//psuedo
	//request message
	//Receive when ready, block if cannot
	//
}






