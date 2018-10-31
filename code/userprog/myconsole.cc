#include "myconsole.h"

MyConsole::MyConsole(const char *readFile, const char *writeFile,
	            VoidFunctionPtr readAvail, VoidFunctionPtr writeDone,
	            void* callArg)
{
	con=new Console(readFile, writeFile,readAvail, writeDone,callArg);
	lock=new Lock("Console Lock");
}

void MyConsole::Write(int buffer, int size, int id)
{
	lock->Acquire();
	for(int i=0;i<size;i++){
		int ch;
		//char t=buffer[i];
		machine->ReadMem(buffer+i,1,&ch);
		con->PutChar((char)ch);
		writeDone->P();
		//printf("%c\n", (char)ch);
	}
	con->PutChar('\n');
	lock->Release();
}
int MyConsole::Read(int buffer, int size, int id)
{
	lock->Acquire();
	for(int i=0;i<size;i++){
		readAvail->P();
		char ch=con->GetChar();
		//char t=buffer[i];
		if(ch=='\n'){
			machine->WriteMem(buffer+i,1,int('\n'));
			lock->Release();
			return i+1;
		}
		else{
			machine->WriteMem(buffer+i,1,(int)ch);

		}
	}
	//machine->WriteMem(buffer+size,1,int('\n'));
	lock->Release();
	return 1;
}

