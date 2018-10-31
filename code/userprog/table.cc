#include "table.h"

Table::Table(int size)
{
	tablesize=0;
	arr=new process[size];
	bitmap=new BitMap(size);
	lock=new Lock("Process Table Lock");
}
Table::~Table()
{
	delete arr;
	delete bitmap;
	delete lock;
}

process::process(int p,void *t)
{
	pid=p;
	thread=t;
}

process::process(){}

int Table::Alloc(void *object)
{
	lock->Acquire();
	int x=bitmap->Find();
	if(x==-1) {
		lock->Release();
		return x;
	}
	process p(x,object);
	arr[x]=p;
	tablesize++;
	lock->Release();
	return x;
}
void * Table::Get(int index)
{
	lock->Acquire();
	if(!bitmap->Test(index)) {
		lock->Release();
		return NULL;
	}
	void *x= arr[index].thread;
	lock->Release();
	return x;
}
void Table::Release(int index)
{
	lock->Acquire();
	if(bitmap->Test(index)) {
		bitmap->Clear(index);
		tablesize--;
	}
	lock->Release();
}

