// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include "memorymanager.h"
#include "table.h"
#include "myconsole.h"
MemoryManager *manager;
Lock *memorylock;
Table *ptable;
Lock *plock;
MyConsole *mycon;
int counter;
int timestamp[NumPhysPages];

static void ReadAvail(void* arg) { readAvail->V(); }
static void WriteDone(void* arg) { writeDone->V(); }


//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(const char *filename)
{
	manager=new MemoryManager(NumPhysPages);
	memorylock=new Lock("Memory Lock in address space");
	ptable=new Table(64);
	plock=new Lock("Process lock");
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;
    counter=0;
    for(int i=0;i<NumPhysPages;i++) timestamp[i]=1<<30;

    readAvail=new Semaphore("readAvai",0);
    writeDone = new Semaphore("write done", 0);
    mycon=new MyConsole(NULL,NULL,ReadAvail, WriteDone,0);


    if (executable == NULL) {
    	printf("Unable to open file %s\n", filename);
    	return;
    }
    space = new AddrSpace(executable);    
    currentThread->space = space;

    int mun=ptable->Alloc(currentThread);
    currentThread->pid=mun;



    //delete executable;			// close file

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

Console *console;
Semaphore *readAvail;
Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------



//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (const char *in, const char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
