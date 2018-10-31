#ifndef MYCONSOLE_H
#define MYCONSOLE_H

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "synch.h"

extern Semaphore *readAvail;
extern Semaphore *writeDone;

class MyConsole
{
private:
	Console *con;
	Lock *lock;
public:
	MyConsole(const char *readFile, const char *writeFile,
	            VoidFunctionPtr readAvail, VoidFunctionPtr writeDone,
	            void* callArg);
	void Write(int buffer, int size, int id);
	int Read(int buffer, int size, int id);
};

#endif
