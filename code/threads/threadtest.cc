// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "list.h"
#include "synch.h"
#include "producer.h"
#include "consumer.h"

int pcsize;
List<int> *pcqueue;
Lock *pclock;
Condition *PCondition;
Condition *CCondition;
int item;

void ProducerThread(void *name)
{
	char* a=(char*) name;
	new Producer(a);
}

void ConsumerThread(void *name)
{
	char* a=(char*) name;
	new Consumer(a);
}

void PCTest()
{
	item=0;
	pcsize=0;
	pcqueue=new List<int>;
	pclock=new Lock("ProducerConsumerLock");
	PCondition=new Condition("ProducerCondition",pclock);
	CCondition=new Condition("ConsumerCondition",pclock);
	for ( int k=1; k<=5; k++) {
		char* threadname = new char[100];
	    sprintf(threadname, "Producer %d", k);
	    Thread* newThread = new Thread (threadname);
	    newThread->Fork (ProducerThread, (void*)threadname);
	}
	for ( int k=1; k<=5; k++) {
		char* threadname = new char[100];
		sprintf(threadname, "Consumer %d", k);
		Thread* newThread = new Thread (threadname);
		newThread->Fork (ConsumerThread, (void*)threadname);
	}

}


//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

void
SimpleThread(void* name)
{
	/*
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
	printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
        //currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
     */
    PCTest();
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    for ( int k=1; k<=10; k++) {
      char* threadname = new char[100];
      sprintf(threadname, "Hilo %d", k);
      Thread* newThread = new Thread (threadname);
      newThread->Fork (SimpleThread, (void*)threadname);
    }
    
    SimpleThread( (void*)"Hilo 0");
}

