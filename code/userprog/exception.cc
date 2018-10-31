// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synch.h"
#include "table.h"
#include "memorymanager.h"
#include "myconsole.h"
#include "machine.h"

extern Table *ptable;
extern Lock *plock;
extern MemoryManager *manager;
extern MyConsole *mycon;

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void processCreator(void *arg)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	machine->Run();
	ASSERT(false);
}

void myExceptions(char * name)
{
	printf("%s\n",name);
	plock->Acquire();
	int s=machine->pageTableSize;
	for(int i=0;i<s;i++){
		if(machine->pageTable[i].valid==true )manager->FreePage(machine->pageTable[i].physicalPage);
	}
	ptable->Release(currentThread->pid);
	plock->Release();
	if(ptable->tablesize==0) interrupt->Halt();
	else currentThread->Finish();
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException)) {
    	if(type==SC_Halt){
    		DEBUG('a', "Shutdown, initiated by user program.\n");
    		interrupt->Halt();
    	}
    	else if(type==SC_Exec){
    		plock->Acquire();
    		int add = machine->ReadRegister(4);
    		char *ff = new char[200];
    		int ch;
    		if(!machine->ReadMem(add,1,&ch)){
    			if(!machine->ReadMem(add,1,&ch)) Exit(0);
    		}
    		int i=0;
    		while( ch!=0 )
    		{
    			ff[i]=(char)ch;
    			add+=1;
				i++;
				if(!machine->ReadMem(add,1,&ch)){
					if(!machine->ReadMem(add,1,&ch)) Exit(0);
				}
    		}

    		plock->Release();
    		ff[i]=(char)0;

    		OpenFile *executable = fileSystem->Open(ff);


    		AddrSpace *space;
    		space = new AddrSpace(executable);
    		Thread * t = new Thread("Inside SC_Exec");
    		t->space = space;
    		t->pid=ptable->Alloc( (void*) t);
    		int processid=t->pid;
    		machine->WriteRegister(2,processid);
    		t->Fork(processCreator,(void*)&processid);

    		IntStatus oldLevel = interrupt->SetLevel(IntOff);
    		int pc;
    		pc=machine->ReadRegister(PCReg);
    		machine->WriteRegister(PrevPCReg,pc);
    		pc=machine->ReadRegister(NextPCReg);
    		machine->WriteRegister(PCReg,pc);
    		pc += 4;
    		machine->WriteRegister(NextPCReg,pc);

    		interrupt->SetLevel(oldLevel);

    	}

    	else if(type==SC_Exit){
    		plock->Acquire();
    		printf("exited value %d\n", machine->ReadRegister(4));
    		int s=machine->pageTableSize;
    		for(int i=0;i<s;i++) {
    			if(machine->pageTable[i].valid==true) manager->FreePage(machine->pageTable[i].physicalPage);
    		}
    		ptable->Release(currentThread->pid);
    		plock->Release();
    		if(ptable->tablesize==0) interrupt->Halt();
    		else currentThread->Finish();
    	}
    	else if(type==SC_Read){
    		plock->Acquire();
    		int add = machine->ReadRegister(4);
    		int size = machine->ReadRegister(5);
    		mycon->Read(add,size,currentThread->pid);
    		//printf("inside read\n");
    		plock->Release();

    		IntStatus oldLevel = interrupt->SetLevel(IntOff);
    		int pc;
    		pc=machine->ReadRegister(PCReg);
    		machine->WriteRegister(PrevPCReg,pc);
    		pc=machine->ReadRegister(NextPCReg);
    		machine->WriteRegister(PCReg,pc);
    		pc += 4;
    		machine->WriteRegister(NextPCReg,pc);

    		interrupt->SetLevel(oldLevel);
    	}
    	else if(type==SC_Write){
    		plock->Acquire();
    		int add = machine->ReadRegister(4);
    		int size = machine->ReadRegister(5);
    		mycon->Write(add,size,currentThread->pid);
    		//printf("inside write\n");
    		plock->Release();

    		IntStatus oldLevel = interrupt->SetLevel(IntOff);

    		int pc;
    		pc=machine->ReadRegister(PCReg);
    		machine->WriteRegister(PrevPCReg,pc);
    		pc=machine->ReadRegister(NextPCReg);
    		machine->WriteRegister(PCReg,pc);
    		pc += 4;
    		machine->WriteRegister(NextPCReg,pc);

    		interrupt->SetLevel(oldLevel);
    	}
    	else {
    		printf("Unexpected user mode exception %d %d\n", which, type);
    		 ASSERT(false);
    	}
    }
    else if(which==PageFaultException ){
    	//myExceptions("PageFaultException");
    	int addr,vpn,physicalPageNo;
    	addr=machine->ReadRegister(39);
    	vpn= addr/PageSize;
    	//vpn=divRoundDown(addr,PageSize);
    	//physicalPageNo = manager->AllocPage();
    	//printf("%d  %d   %d\n",addr,vpn,PageSize);
    	//if(vpn<=pageTableSize){
    	physicalPageNo=manager->Alloc(currentThread->pid,&machine->pageTable[vpn]);
    	if(physicalPageNo==-1){
    		//printf("hur\n");
    		physicalPageNo=manager->AllocByForce(currentThread->pid,&machine->pageTable[vpn]);
    	}
    	currentThread->space->loadIntoFreePage(addr, physicalPageNo);
    	//}
    }
    else if(which== ReadOnlyException ){
    	myExceptions("ReadOnlyException");
    }
    else if(which==BusErrorException ){
    	myExceptions("BusErrorException");
    }
    else if(which==AddressErrorException ){
    	myExceptions("AddressErrorException");
    }
    else if(which==OverflowException ){
    	myExceptions("OverflowException");
    }
    else if(which==IllegalInstrException ){
    	myExceptions("IllegalInstrException");
    }
    else if(which==NumExceptionTypes ){
    	myExceptions("NumExceptionTypes");
    }
    else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	 ASSERT(false);
    }
}
