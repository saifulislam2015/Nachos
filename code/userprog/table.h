#ifndef TABLE_H
#define TABLE_H

#include "synch.h"
#include "bitmap.h"

class process
{
public:
	int pid;
	void *thread;
	process(int pid,void *thread);
	process();
};

class Table{
public:
	Table(int size) ;
	~Table();
	int Alloc(void *object);
	void *Get(int index);
	void Release(int index) ;
	int tablesize;

private:
	process *arr;
	Lock *lock;
	BitMap *bitmap;
};

#endif
