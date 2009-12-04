/*
 *	  File: exec_BRAIN.c
 *	Author: Gary S. Jordan
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

#include "Disk.h"
#include "RAM.h"
#include "Exec_Brain.h"
#include "sched.h"
#include "SharedMem.h"

//IMPORTANT, MOVED TIME SLICE DEFINE INTO THE .H FILE


#define NPID 100
#define RAM 4*10000

PCB* Current_PCB;			       //Current Process control block
PCB* PCB_Array;
WORDBYTES CurrentWord;			  //Current 4 byte word read from memory
OPERATOR operator;		      	//Operator to be chosen from the list
WORDBYTES MemoryContents;		       //4 byte word read from memory
u_int32_t* MailBox_Start;
u_int32_t ContextSwitchCount=0;

u_int32_t* PostOffice;

/*
 *Invokes the main loop which reads, executes the operations and writes back to memory
 */
int Exec_Brain(int nPID , u_int16_t Program_Length)
{
	//initialize all the PCB variables to 0
	char PID=0xFF;
	PCB_Array=calloc(100,sizeof(PCB));
	PostOffice=calloc(NPID * NPID,4);
	int i;
	for(i=0;i < NPID * NPID;i++) *(PostOffice + i) = 0xFF;


	PCB_Array[numPID].R = 0;
	PCB_Array[numPID].SP = 0;
	PCB_Array[numPID].C = 'F';
	PCB_Array[numPID].IC = 0;
	PCB_Array[numPID].PID = numPID;
	PCB_Array[numPID].LR = Program_Length;
	PCB_Array[numPID].BR = 0;
	PCB_Array[numPID].Block = 0;
	PCB_Array[numPID].MailBox_Start = PostOffice+(i*nPID);
	PCB_Array[numPID].MailBox_End = PostOffice+((i+1)*nPID-1);
	PCB_Array[numPID].WaitID = 0xFF;
	PCB_Array[numPID].TDMA = 0;

	readyq(&(PCB_Array[numPID].PID), 1);

	numPID++;
	PID=readyq(&(Current_PCB->PID),0);
	Current_PCB=&PCB_Array[(int)PID];
	ReadLogical(0,0,0);
//    printstatus();
	while(1)
	{
		while(Current_PCB->TDMA<TDMA_Setting)
		{
			CurrentWord=ReadLogical(0,Current_PCB->IC,Current_PCB->PID);		//gets instruction
			printf("PCB Status:  R:%d  SP:%d  IC:%d C:%c PID:%d\n", Current_PCB->R,Current_PCB->SP,Current_PCB->IC,Current_PCB->C,Current_PCB->PID);
			printf("Current Instr:  %c%c%c%c\n",CurrentWord.bytes.byte1,CurrentWord.bytes.byte2,CurrentWord.bytes.byte3,CurrentWord.bytes.byte4);
			Current_PCB->IC++;
			operator.bytes.byte1=CurrentWord.bytes.byte1;				//give operator 1 a value
			operator.bytes.byte2=CurrentWord.bytes.byte2;						//give operator 1 a value
			Instruction(operator.twobytes, CurrentWord.bytes.byte3, CurrentWord.bytes.byte4);	//Calls Instruction function
			Current_PCB->TDMA++;
			printstatus();

		}
		Current_PCB->TDMA=0;
		if (Current_PCB->Block==0) readyq(&(Current_PCB->PID),1);
		PID=readyq(&(Current_PCB->PID),0);		//Get next process from ready queue.
		Current_PCB=&PCB_Array[(int)PID];
		ContextSwitchCount++;

	}
return 0;
}

/*
 *Takes two 1 byte operators standing for ASCII letters:
 *@param rator
 *
 *Also takes two words as the operands:
 *@param rand1
 *@param rand2control
 */
void Instruction(u_int16_t rator,u_int8_t rand1,u_int8_t rand2)
{

		//instruction set
		switch (rator)
		{
			case ISTR_LR:  LoadRegister(rand1, rand2);      break;  // Load Register (LR)
			case ISTR_LL:  LoadLow(rand1, rand2);           break;  // Load Low (LL)
			case ISTR_LH:  LoadHigh(rand1, rand2);          break;  // Load High (LH)
			case ISTR_SR:  StoreReg(rand1,rand2);           break;  // Stack Register (SR)
			case ISTR_SP:  RegToSP();                       break;  // Copies the register to the Stack Pointer (SP)
			case ISTR_PS:  SPToReg();                       break;  // Copies the Stack Pointer to the register (PS)
			case ISTR_PH:  RegToStack();                    break;  // Push (PH)
			case ISTR_PP:  StackToReg(Current_PCB->SP);     break;  // Pop (PP)
			case ISTR_CE:  CompareEqual(rand1,rand2);       break;  // Compare Equal (CE)
			case ISTR_CL:  CompareLess(rand1,rand2);        break;  // Compare Less (CL)
			case ISTR_BT:  BranchTrue(rand1,rand2);         break;  // Branch Conditional (BT)
			case ISTR_BU:  BranchUnc(rand1,rand2);          break;  // Branch Unconditional (BU)
			case ISTR_GD:  GetData(rand1,rand2,Current_PCB->BR);            break;  // Get Data (GD)
			case ISTR_PD:  PutData(rand1);                  break;  // Put Data (PD)
			case ISTR_AD:  AddToReg(rand1,rand2);           break;  // Add (AD)
			case ISTR_SU:  RegSubtract(rand1,rand2);        break;  // Subtra101ct (SU)
			case ISTR_MU:  RegMultiply(rand1,rand2);        break;  // Multiply (MU)
			case ISTR_DI:  RegDivide(rand1,rand2);          break;  // Divide (DI)
			case ISTR_AS:  AddStack();                      break;  // Add Stack (AS)
			case ISTR_SS:  SubStack();                      break;  // Subtract Stack (SS)
			case ISTR_MS:  MultStack();                     break;  // Multiply Stack (MS)
			case ISTR_DS:  DivStack();                      break;  // Divide Stack (DS)
			case ISTR_NP:                                   break;  // No-op (NP)
			case ISTR_H:   Current_PCB->Block=1;Current_PCB->TDMA=TDMA_Setting; break;                      // Halt (H)
			case ISTR_HN:  Current_PCB->Block=1;Current_PCB->TDMA=TDMA_Setting; break;                      // Halt (H)
			case ISTR_SD:  Send(rand1,rand2);               break;  // Send (SD)
			case ISTR_RC:  Rec(rand1,rand2);                break;  // Receive (RC)
			case ISTR_GP:  GetPID();						break;  // Return Process ID to reg
			case ISTR_FK:  Fork(); 							break;	// Fork a new process
			case ISTR_EX:  Exec(rand1,rand2);				break;	// Execute a new process
			case ISTR_PE:  PE(rand1,rand2); 				break;
			case ISTR_VE:  VE(rand1,rand2); 				break;
			case ISTR_SI:  SI(rand1,rand2); 				break;
			case ISTR_LS:  LS(rand1,rand2); 				break;
			case ISTR_ST:  ST(rand1,rand2); 				break;

			default:								break;
		}
		//printstatus();
	return;
}

/*test
 *Push PCB.R(the register) data onto the stack
 */
void RegToStack()
{
	Current_PCB->SP=Current_PCB->SP+1;				//Consider Adding a subroutine which converts ascii2dec and dec2ascii
	MemoryContents.bytes.byte4=(Current_PCB->R%10)+48;
	MemoryContents.bytes.byte3=((Current_PCB->R/10)%10)+48;
	MemoryContents.bytes.byte2=((Current_PCB->R/100)%10)+48;
	MemoryContents.bytes.byte1=((Current_PCB->R/1000)%10)+48;
	WriteLogical(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
}

/*
 *Pop the stack to PCB.R(the register)
 */
void StackToReg()
{
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	Current_PCB->R=MemoryContents.word;
	Current_PCB->SP--;
}


void LoadRegister(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);
	Current_PCB->R=MemoryContents.word;
}

void CompareEqual(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);
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
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);
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
 *@param rand2101
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
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);
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
	MemoryContents=ReadLogical (rand1-48,rand2-48,Current_PCB->BR);
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
	MemoryContents=ReadLogical (rand1-48,rand2-48,Current_PCB->BR);
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
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);
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
	if (Current_PCB->R <= 9999) {
		MemoryContents.bytes.byte4=(Current_PCB->R%10)+48;
		MemoryContents.bytes.byte3=((Current_PCB->R/10)%10)+48;
		MemoryContents.bytes.byte2=((Current_PCB->R/100)%10)+48;
		MemoryContents.bytes.byte1=((Current_PCB->R/1000)%10)+48;
	}
	else {
		MemoryContents.word = Current_PCB->R;
	}

	WriteLogical(MemoryContents.word,rand1-48,rand2-48,Current_PCB->PID);
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
		MemoryContents=ReadLogical(rand1-48, i,Current_PCB->BR);
		printf("%d ",MemoryContents.word);
	}
	printf("\n");

}

/*
 *Calculates the difference of the top number on the stack and the next number and stores it in place of the top number
 */
void SubStack()
{
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	MemoryContents.word=MemoryContents.word-Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteLogical(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}

/*
 *Multiplies the value of the top number on the stack by the next number and stores it in place of the top number
 */
void MultStack()
{
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	MemoryContents.word=MemoryContents.word*Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteLogical(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}

/*
 *Divides the value of the top number on the stack by the next number and stores it in place of the top number
 */
void DivStack()
{
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	MemoryContents.word=MemoryContents.word/Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteLogical(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}


/*
 *Adds the value of the top number on the stack to the next number and stores it in place of the top number
 */
void AddStack()
{
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	u_int32_t Temp=MemoryContents.word;
	Current_PCB->SP-=1;
	MemoryContents=ReadLogical((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->BR);
	MemoryContents.word=MemoryContents.word+Temp;
	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
	u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
	u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
	MemoryContents.bytes.byte1=Tempbyte1;
	MemoryContents.bytes.byte2=Tempbyte2;
	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;
	WriteLogical(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);


}

/*
 *Prints status of memory block and process control block
 */

void LoadHigh(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);
	Current_PCB->R=(Current_PCB->R & 0x00FF);
	Current_PCB->R=(MemoryContents.word|0xFF00)&Current_PCB->R;
}

void LoadLow(u_int8_t rand1,u_int8_t rand2)
{
	MemoryContents=ReadLogical(rand1-48,rand2-48,Current_PCB->PID);

	u_int8_t Tempbyte4=MemoryContents.word%10+48;
	u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;

	MemoryContents.bytes.byte3=Tempbyte3;
	MemoryContents.bytes.byte4=Tempbyte4;

	Current_PCB->R=(Current_PCB->R & 0xFFFFF0F0);
	Current_PCB->R=(MemoryContents.word & 0x0000FFFF) | Current_PCB->R;
	MemoryContents.word = Current_PCB->R;
}

void Send(u_int8_t rand1,u_int8_t rand2)
{
		u_int16_t Dest_PID;
		Dest_PID=((rand1-48)*10)+rand2-48;					      //Destination of Send

		*(PCB_Array[Dest_PID].MailBox_Start+Current_PCB->PID)   //Determine Value to send based on R and Address
			=(Current_PCB->R+(Current_PCB->PID<<16));

		if ((PCB_Array[Dest_PID].WaitID==Current_PCB->PID)||(PCB_Array[Dest_PID].WaitID==0xEE))	 //If Other process is waiting for this message
		{
				blockq(&(PCB_Array[Dest_PID].PID),0);			   //Take of block queue
				PCB_Array[Dest_PID].Block=0;									//Unblock it
				PCB_Array[Dest_PID].WaitID=0xFF;
				readyq(&(PCB_Array[Dest_PID].PID),1);			   //Put in ready queue
		}
		else Current_PCB->WaitID=Dest_PID;

		blockq(&(Current_PCB->PID),1);					  //Block the sender
		Current_PCB->Block=1;													//Block
		Current_PCB->TDMA=TDMA_Setting;				     //Do no more instructions until message confirmation.


}
void Rec(u_int8_t rand1,u_int8_t rand2)
{
	int i;
	u_int16_t Source_PID;
	u_int8_t flag=0xFF;
	if ((rand1=='X')&&(rand2=='X'))  // If Special Case  XX
	{
			for (i=0;i<((Current_PCB->MailBox_End)-(Current_PCB->MailBox_Start))+1;i++)
			{
				 if (*((Current_PCB->MailBox_Start)+i)!=0xFF)
				 {
					 flag=i;

				 }
			}
	}

	else if (*((Current_PCB->MailBox_Start)+((rand1-48)*10)+(rand2-48))!=0xFF) //If something in mailbox from a the specific process
	{
		Current_PCB->R=*((Current_PCB->MailBox_Start)+((rand1-48)*10)+rand2-48);	// Get Message
		*((Current_PCB->MailBox_Start)+((rand1-48)*10)+rand2-48)=0xFF;									  // Reset Mailbox
		CurrentWord.word=Current_PCB->R;											// Store to a word
		Source_PID=(CurrentWord.bytes.byte2);									   // Pull out Source PID
			rand1=CurrentWord.bytes.byte4/10;					   // Pull out memory address
			rand2=CurrentWord.bytes.byte4%10;
			for (i=0;i<10;i++)
			{
				CopyDisk(rand1,rand2+i,Current_PCB->BR,PCB_Array[Source_PID].BR);  // Copy Memory Values
			}

			blockq(&(PCB_Array[Source_PID].PID),0);							// Remove from blocked queue
			PCB_Array[Source_PID].Block=0;								// Unblock
			readyq(&(PCB_Array[Source_PID].PID),1);							// Place in ready queue.

			CurrentWord.word=Current_PCB->R;
			CurrentWord.bytes.byte1=0;
			CurrentWord.bytes.byte2=0;
			Current_PCB->R=CurrentWord.word;
			PCB_Array[Source_PID].WaitID = 0xFF;
	}
	else									    //If no message from desired process
	{
		blockq(&(Current_PCB->PID),1);		  		//Block
		Current_PCB->Block=1;				   	//Block
		Current_PCB->TDMA=TDMA_Setting;				//Do not run any more processes
		Current_PCB->WaitID=((rand1-48)*10)+rand2-48; // Set whom process is waiting for
		Current_PCB->IC--;					      //Decrement Counter -- If unblocked by appropriate sender will repeat this instruct.

	}
	if (flag!=0xFF)
	{
		Current_PCB->R=*((Current_PCB->MailBox_Start)+flag);      // Get Message
		*((Current_PCB->MailBox_Start)+flag)=0xFF;		// Reset Mailbox
		CurrentWord.word=Current_PCB->R;			  // Store to a word
		Source_PID=(CurrentWord.bytes.byte2);		     // Pull out Source PID
				rand1=CurrentWord.bytes.byte4/10;		  // Pull out memory address
				rand2=CurrentWord.bytes.byte4%10;
				Current_PCB->R=(u_int32_t)CurrentWord.bytes.byte4;
				for (i=0;i<10;i++)
				{
						CopyDisk(rand1,rand2+i,Current_PCB->PID,Source_PID);		// Write over memory values
				}
				blockq(&(PCB_Array[Source_PID].PID),0);						// Remove from blocked queue
				PCB_Array[Source_PID].Block=0;							// Unblock
				readyq(&(PCB_Array[Source_PID].PID),1);						// Place in ready queue.
				Current_PCB->WaitID=0xFF;

	}

}
void GetPID()
{
	Current_PCB->R = Current_PCB->PID;
}
void Fork(void)
{

	#if BESTFIT
	PCB_Array[numPID].BR = RequestMemory(Current_PCB->LR,1);
	#elif NEXTFIT
	PCB_Array[numPID].BR = RequestMemory(Current_PCB->LR,0);
	#endif

//	u_int16_t BaseReg = RequestMemory(Current_PCB->LR);
	u_int16_t i;
	WORDBYTES tmp;
	if (Current_PCB->LR > (RAM/4 - Memory_Num)) {
		Current_PCB->R = 0;						//insufficient memory
	}
	else {
//		PCB_Array[numPID].BR = BaseReg;					//Base register starts at end of last process
		PCB_Array[numPID].Block = 0;					//
		PCB_Array[numPID].C = Current_PCB->C;				//Same truth value
		PCB_Array[numPID].IC = Current_PCB->IC;				//Same instruction counter
		PCB_Array[numPID].LR = Current_PCB->LR;				//Same program size
		PCB_Array[numPID].PID = numPID;					//new PID
		PCB_Array[numPID].R = Current_PCB->PID;				//other PID in new R
		PCB_Array[numPID].MailBox_Start = PostOffice+(100*numPID);	//Mailbox Start assuming 100 processes
		PCB_Array[numPID].MailBox_End = PostOffice+((numPID+1)*100-1);	//Mailbox End assuming 100 processes
		PCB_Array[numPID].WaitID = 0xFF;
		PCB_Array[numPID].TDMA = 0;
		readyq(&(PCB_Array[numPID].PID), 1);

		Current_PCB->R = PCB_Array[numPID].PID;				//calling PCB has new PID in R

		//push all pages held to disk
		for (i= Current_PCB->BR; i < Current_PCB->LR ; i+= pagesize) {
			tmp=ReadLogical(i/10,i%10,Current_PCB->PID);
			WriteLogical(tmp.word,i/10,1%10,numPID);
		}
		//existing fork copy stuff
		for (i=0 ; i < Current_PCB->LR ; i++) {				//copy instructions over
			*(Memory_Start+PCB_Array[numPID].BR+i)= *(Memory_Start+Current_PCB->BR+i);
			Memory_Num++;

		}


		numPID++; 	//increment number of processes
		Current_PCB->TDMA=TDMA_Setting;
	}
}
int Exec(u_int8_t rand1,u_int8_t rand2)
{
	u_int8_t filename[3];
	filename[0] = rand1;
	filename[1] = rand2;
	filename[2] = '\0';

	u_int16_t MemStart;			//start new program mem here
	u_int16_t MemLoc;
	//static int PID

	int FileComplete=0;					//flag
	char buff[64]={0};					//storage for each line of code
	char tempbuff[4];					//more storage
	int lengthbuff[4];					//get program length
	fflush(stdin);
	int fildes1=open((char *)filename,O_RDONLY);	//open that file ... do dah doo doo



	if (fildes1 == -1) {					//error checking for open()
		fprintf(stderr,"Program not loaded properly, check to see if input file exists\n");
		return -1;
	}
	close(fileno(stdin));					//put our file in place of stdin
	dup(fildes1);
	close(fildes1);

	// ProgramWrite with a few mods

	fgets(buff,64,stdin);					//get first line
	if(strncmp(buff,"BRAIN09",7) == 0)			//make sure it's legit
	{
		fgets(buff,64,stdin);
		lengthbuff[0] = (buff[0] - 48) * 1000;		//get program size from second line of file
		lengthbuff[1] = (buff[1] - 48) * 100;
		lengthbuff[2] = (buff[2] - 48) * 10;
		lengthbuff[3] = (buff[3] - 48) * 1;

		ReleaseMemory(Current_PCB->BR,Current_PCB->LR);

		#if BESTFIT
		PCB_Array[numPID].BR = RequestMemory(lengthbuff[0] + lengthbuff[1] + lengthbuff[2] + lengthbuff[3] , 1);
		#elif NEXTFIT
		PCB_Array[numPID].BR = RequestMemory(lengthbuff[0] + lengthbuff[1] + lengthbuff[2] + lengthbuff[3] , 0);
		#endif

		MemStart = Current_PCB->BR;			//start new program mem here
		MemLoc = MemStart;
		Current_PCB->LR = lengthbuff[0] + lengthbuff[1] + lengthbuff[2] + lengthbuff[3];
		Current_PCB->IC = 0;

		while (1)
		{
			fgets(buff,64,stdin);			//get next line
			if(strncmp(buff,"DATA",4)==0)		//End of Program Instructions
			{
				FileComplete=1;
				//close(fileno(stdin));
				break;
			}
			if(strncmp(buff,"BRAIN09",7)==0) {	//Look for New Program
				//PID++;
				//i=0;
				//fgets(buff,64,stdin);

				FileComplete = 1;
				//close(fileno(stdin));
				break;
			}
			if(strncmp(buff,"END",3) == 0) {
				FileComplete = 1;
				//close(fileno(stdin));
				break;
			}

			tempbuff[3]=buff[0];			//Switch the bytes around
			tempbuff[2]=buff[1];
			tempbuff[1]=buff[2];
			tempbuff[0]=buff[3];

			Memory_Start[MemLoc] = *((u_int32_t*)tempbuff);	//put instr in memory
			MemLoc++;				//increment # bytes written
		}
		return 0;
	}
	else	{
		fprintf(stderr,"Brain09 Syntax Error\n");
		return -1;
	}
	// End of ProgramWrite with a few mods
}

void printstatus()
{
//	printf("PCB Status:  R:%d  SP:%d  IC:%d C:%c PID:%d\n", Current_PCB->R,Current_PCB->SP,Current_PCB->IC,Current_PCB->C,Current_PCB->PID);
//	printf("Current Instr:  %c%c%c%c\n",CurrentWord.bytes.byte1,CurrentWord.bytes.byte2,CurrentWord.bytes.byte3,CurrentWord.bytes.byte4);
	RAMDump();


/* Old Stuff
	if (SharedStatus) {
		printf("Shared Memory Status: \n");
		PrintShared();
	}
	if (HoleStatus) {
		HoleStatusDump();
	}
	if (SystemStatus) {
		printf("PCB Status:  R:%d  SP:%d  IC:%d C:%c PID:%d\n", Current_PCB->R,Current_PCB->SP,Current_PCB->IC,Current_PCB->C,Current_PCB->PID);
		printf("Current Instr:  %c%c%c%c\n",CurrentWord.bytes.byte1,CurrentWord.bytes.byte2,CurrentWord.bytes.byte3,CurrentWord.bytes.byte4);
		DiskDump(Current_PCB->BR);
		printf("Context Switches: %d\n",ContextSwitchCount);
	}
*/
}
