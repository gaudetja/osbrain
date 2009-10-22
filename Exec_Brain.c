/*
 *          File: exec_BRAIN.c
 *        Author: Gary S. Jordan
 *                Eric Payne
 *                Jered Philippon
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
#include "Exec_Brain.h"
#include "sched.h"



PCB* Current_PCB; 					     //Current Process control block
PCB* PCB_Array;
WORDBYTES CurrentWord;          		//Current 4 byte word read from memory
OPERATOR operator;                      //Operator to be chosen from the list
WORDBYTES MemoryContents;       		//4 byte word read from memory
u_int32_t* MailBox_Start;

/*
 *Invokes the main loop which reads, executes the operations and writes back to memory
 */
int Exec_Brain(char NPID)
{
        //initialize all the PCB variables to 0
		NPID++;

        char PID=0xFF;
        PCB_Array=calloc(NPID,sizeof(PCB));
        u_int32_t* PostOffice=calloc(NPID*NPID,4);
        int i=0;
        for(i=0;i<NPID*NPID;i++)
        	*(PostOffice+i)=0xFF;
        buildq();
        for (i=0;i<NPID;i++)
        {
                PCB_Array[i].R=0;
                PCB_Array[i].SP=0;
                PCB_Array[i].C='F';
                PCB_Array[i].IC=0;
                PCB_Array[i].PID=i;
                PCB_Array[i].LR=(i+1)*100-1;
                PCB_Array[i].BR=0;
                PCB_Array[i].Block=0;
                PCB_Array[i].MailBox_Start=PostOffice+(i*NPID);
                PCB_Array[i].MailBox_End=PostOffice+((i+1)*NPID-1);
                PCB_Array[i].WaitID=0xFF;
                PCB_Array[i].TDMA=0;
                readyq(&(PCB_Array[i].PID), 1);
		}

        while(1)
        {
                PID=readyq(&(Current_PCB->PID),0);   //Get next process from ready queue.
                Current_PCB=&PCB_Array[(int)PID];
                while(Current_PCB->TDMA<10)
                {
                        CurrentWord=GetInstruction(Current_PCB->IC,Current_PCB->PID);                                                                                    //gets instruction
                        operator.bytes.byte1=CurrentWord.bytes.byte1;                                                                           //give operator 1 a value
                        operator.bytes.byte2=CurrentWord.bytes.byte2;                                                                           //give operator 1 a value
                        Instruction(operator.twobytes, CurrentWord.bytes.byte3, CurrentWord.bytes.byte4);       //Calls Instruction function
                        Current_PCB->TDMA++;
                        Current_PCB->IC++;
                }
                Current_PCB->TDMA=0;
                if (Current_PCB->Block==0)
                	readyq(&(Current_PCB->PID),1);
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
                        case ISTR_PP:  StackToReg(Current_PCB->SP);     break;      // Pop (PP)
                        case ISTR_CE:  CompareEqual(rand1,rand2);       break;  // Compare Equal (CE)
                        case ISTR_CL:  CompareLess(rand1,rand2);        break;  // Compare Less (CL)
                        case ISTR_BT:  BranchTrue(rand1,rand2);         break;  // Branch Conditional (BT)
                        case ISTR_BU:  BranchUnc(rand1,rand2);          break;  // Branch Unconditional (BU)
                        case ISTR_GD:  GetData(rand1,rand2,Current_PCB->PID);            break;  // Get Data (GD)
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
                        case ISTR_H:   Current_PCB->Block=1;Current_PCB->TDMA=10; break;                      // Halt (H)
                        case ISTR_HN:  Current_PCB->Block=1;Current_PCB->TDMA=10; break;                      // Halt (H)
                        case ISTR_SD:  Send(rand1,rand2);               break;  // Send (SD)
                        case ISTR_RC:  Rec(rand1,rand2);                break;  // Receive (RC)
                        default:                                                                        break;
                }
                printstatus();
        return;
}

/*test
 *Push PCB.R(the register) data onto the stack
 */
void RegToStack()
{
        Current_PCB->SP=Current_PCB->SP+1;                                                              //Consider Adding a subroutine which converts ascii2dec and dec2ascii
        MemoryContents.bytes.byte4=(Current_PCB->R%10)+48;
        MemoryContents.bytes.byte3=((Current_PCB->R/10)%10)+48;
        MemoryContents.bytes.byte2=((Current_PCB->R/100)%10)+48;
        MemoryContents.bytes.byte1=((Current_PCB->R/1000)%10)+48;
        WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
}

/*
 *Pop the stack to PCB.R(the register)
 */
void StackToReg()
{
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        Current_PCB->R=MemoryContents.word;
        Current_PCB->SP--;
}

/*101
 *Load a place in memory to the register
 *@param rand1
 *
 *@param rand2
 *
 */
void LoadRegister(u_int8_t rand1,u_int8_t rand2)
{
        MemoryContents=ReadMemory(rand1-48,rand2-48,Current_PCB->PID);
        Current_PCB->R=MemoryContents.word;
}

/*
 *If the number in the register is equal to the number specified by the parameters, then set toggle to true, else set to false
 *@param rand1ffff2fb0
 *
 *@param rand2
 *101
 */
void CompareEqual(u_int8_t rand1,u_int8_t rand2)
{
        MemoryContents=ReadMemory(rand1-48,rand2-48,Current_PCB->PID);
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
        MemoryContents=ReadMemory(rand1-48,rand2-48,Current_PCB->PID);
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
                Current_PCB->IC = (10*(rand1-48)+(rand2-48))-1;
}


/*
 *PBC.IC(instruction counter) gets the value specified
 *@param rand1
 *
 *@param rand2
 */
void BranchUnc(u_int8_t rand1,u_int8_t rand2)
{
        Current_PCB->IC = (10*(rand1-48)+(rand2-48))-1;
}

/*
 *Adds the number represented by the two parameter words in PCB.R(the register)
 *@param rand1
 *
 *@param rand2
 */
void AddToReg(u_int8_t rand1,u_int8_t rand2)
{
        MemoryContents=ReadMemory(rand1,rand2,Current_PCB->PID);
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
        MemoryContents=ReadMemory (rand1-48,rand2-48,Current_PCB->PID);
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
        MemoryContents=ReadMemory (rand1-48,rand2-48,Current_PCB->PID);
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
        MemoryContents=ReadMemory(rand1-48,rand2-48,Current_PCB->PID);
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
        WriteMemory(MemoryContents.word,rand1-48,rand2-48,Current_PCB->PID);
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
                MemoryContents=ReadMemory(rand1-48, i,Current_PCB->PID);
                printf("%d ",MemoryContents.word);
        }
        printf("\n");

}

/*
 *Calculates the difference of the top number on the stack and the next number and stores it in place of the top number
 */
void SubStack()
{
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        u_int32_t Temp=MemoryContents.word;
        Current_PCB->SP-=1;
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        MemoryContents.word=MemoryContents.word-Temp;
        u_int8_t Tempbyte4=MemoryContents.word%10+48;
        u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
        u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
        u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
        MemoryContents.bytes.byte1=Tempbyte1;
        MemoryContents.bytes.byte2=Tempbyte2;
        MemoryContents.bytes.byte3=Tempbyte3;
        MemoryContents.bytes.byte4=Tempbyte4;
        WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}

/*
 *Multiplies the value of the top number on the stack by the next number and stores it in place of the top number
 */
void MultStack()
{
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        u_int32_t Temp=MemoryContents.word;
        Current_PCB->SP-=1;
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        MemoryContents.word=MemoryContents.word*Temp;
        u_int8_t Tempbyte4=MemoryContents.word%10+48;
        u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
        u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
        u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
        MemoryContents.bytes.byte1=Tempbyte1;
        MemoryContents.bytes.byte2=Tempbyte2;
        MemoryContents.bytes.byte3=Tempbyte3;
        MemoryContents.bytes.byte4=Tempbyte4;
        WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}

/*
 *Divides the value of the top number on the stack by the next number and stores it in place of the top number
 */
void DivStack()
{
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        u_int32_t Temp=MemoryContents.word;
        Current_PCB->SP-=1;
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        MemoryContents.word=MemoryContents.word/Temp;
        u_int8_t Tempbyte4=MemoryContents.word%10+48;
        u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
        u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
        u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
        MemoryContents.bytes.byte1=Tempbyte1;
        MemoryContents.bytes.byte2=Tempbyte2;
        MemoryContents.bytes.byte3=Tempbyte3;
        MemoryContents.bytes.byte4=Tempbyte4;
        WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}


/*
 *Adds the value of the top number on the stack to the next number and stores it in place of the top number
 */
void AddStack()
{
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        u_int32_t Temp=MemoryContents.word;
        Current_PCB->SP-=1;
        MemoryContents=ReadMemory((Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);
        MemoryContents.word=MemoryContents.word+Temp;
        u_int8_t Tempbyte4=MemoryContents.word%10+48;
        u_int8_t Tempbyte3=(MemoryContents.word/10)%10+48;
        u_int8_t Tempbyte2=(MemoryContents.word/100)%10+48;
        u_int8_t Tempbyte1=(MemoryContents.word/1000)%10+48;
        MemoryContents.bytes.byte1=Tempbyte1;
        MemoryContents.bytes.byte2=Tempbyte2;
        MemoryContents.bytes.byte3=Tempbyte3;
        MemoryContents.bytes.byte4=Tempbyte4;
        WriteMemory(MemoryContents.word,(Current_PCB->SP/10)%10,(Current_PCB->SP%10),Current_PCB->PID);

}

/*
 *Prints status of memory block and process control block
 */
void printstatus()
{
        printf("PCB Status:  R:%d  SP:%d  IC:%d C:%c PID:%d\n", Current_PCB->R,Current_PCB->SP,Current_PCB->IC,Current_PCB->C,Current_PCB->PID);
        printf("Current Instr:  %c%c%c%c\n",CurrentWord.bytes.byte1,CurrentWord.bytes.byte2,CurrentWord.bytes.byte3,CurrentWord.bytes.byte4);
        MemoryDump(Current_PCB->PID);


}

void LoadHigh(u_int8_t rand1,u_int8_t rand2)
{
        MemoryContents=ReadMemory(rand1-48,rand2-48,Current_PCB->PID);
        Current_PCB->R=(Current_PCB->R & 0x00FF);
        Current_PCB->R=(MemoryContents.word|0xFF00)&Current_PCB->R;
}

void LoadLow(u_int8_t rand1,u_int8_t rand2)
{
        MemoryContents=ReadMemory(rand1-48,rand2-48,Current_PCB->PID);
        Current_PCB->R=(Current_PCB->R & 0xFF00);
        Current_PCB->R=(MemoryContents.word|0x00FF) & Current_PCB->R;
}
void Send(u_int8_t rand1,u_int8_t rand2)
{
		u_int16_t Dest_PID;
		Dest_PID=((rand1-48)*10)+rand2-48;						//Destination of Send

		*(PCB_Array[Dest_PID].MailBox_Start+Current_PCB->PID)   //Determine Value to send based on R and Address
			=(Current_PCB->R+(Current_PCB->PID<<16));

        if ((PCB_Array[Dest_PID].WaitID==Current_PCB->PID)||(PCB_Array[Dest_PID].WaitID==0xEE))		//If Other process is waiting for this message
        {
        	blockq(&(PCB_Array[Dest_PID].PID),0);				//Take of block queue
        	PCB_Array[Dest_PID].Block=0;						//Unblock it
        	readyq(&(PCB_Array[Dest_PID].PID),1);				//Put in ready queue
        }
        blockq(&(Current_PCB->PID),1);							//Block the sender
		Current_PCB->Block=1;									//Block
		Current_PCB->TDMA=10;									//Do no more instructions until message confirmation.


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
        	Current_PCB->R=*((Current_PCB->MailBox_Start)+((rand1-48)*10)+rand2-48);  	// Get Message
        	*((Current_PCB->MailBox_Start)+((rand1-48)*10)+rand2-48)=0xFF;										// Reset Mailbox
        	CurrentWord.word=Current_PCB->R;											// Store to a word
        	Source_PID=(CurrentWord.bytes.byte2);										// Pull out Source PID
			rand1=CurrentWord.bytes.byte4/10;                                           // Pull out memory address
			rand2=CurrentWord.bytes.byte4%10;
			for (i=0;i<10;i++)
			{
				CopyMemory(rand1,rand2+i,Current_PCB->PID,Source_PID); 	// Copy Memory Values
			}

			blockq(&(PCB_Array[Source_PID].PID),0);										// Remove from blocked queue
			PCB_Array[Source_PID].Block=0;												// Unblock
			readyq(&(PCB_Array[Source_PID].PID),1);										// Place in ready queue.
        }
        else										//If no message from desired process
        {
        	blockq(&(Current_PCB->PID),1);			//Block
        	Current_PCB->Block=1;					//Block
        	Current_PCB->TDMA=10;					//Do not run any more processes
        	Current_PCB->WaitID=((rand1-48)*10)+rand2-48; // Set whom process is waiting for
        	Current_PCB->IC--;						//Decrement Counter -- If unblocked by appropriate sender will repeat this instruct.

        }
        if (flag!=0xFF)
        {
        	Current_PCB->R=*((Current_PCB->MailBox_Start)+flag);  							// Get Message
        	*((Current_PCB->MailBox_Start)+flag)=0xFF;										// Reset Mailbox
        	CurrentWord.word=Current_PCB->R;											// Store to a word
        	Source_PID=(CurrentWord.bytes.byte2);										// Pull out Source PID
			rand1=CurrentWord.bytes.byte4/10;                                           // Pull out memory address
			rand2=CurrentWord.bytes.byte4%10;
			for (i=0;i<10;i++)
			{
				CopyMemory(rand1,rand2+i,Current_PCB->PID,Source_PID);			// Write over memory values
			}
			blockq(&(PCB_Array[Source_PID].PID),0);										// Remove from blocked queue
			PCB_Array[Source_PID].Block=0;												// Unblock
			readyq(&(PCB_Array[Source_PID].PID),1);										// Place in ready queue.
			Current_PCB->WaitID=0xFF;

        }
        else
        {
        	blockq(&(Current_PCB->PID),1);			//Block
        	Current_PCB->Block=1;					//Block
        	Current_PCB->TDMA=10;					//Do not run any more processes
        	Current_PCB->WaitID=0xEE; // Set whom process is waiting for
        	Current_PCB->IC--;
        }
}
