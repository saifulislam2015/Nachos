#ifndef MEMORYMAHAGER_H
#define MEMORYMAHAGER_H

#include "synch.h"
#include "bitmap.h"
#include "table.h"

class MemoryManager{
public:
	MemoryManager(int numPages);
	~MemoryManager();
	int AllocPage();
	void FreePage(int physPageNum);
	bool PageIsAllocated(int physPageNum);
	int Alloc(int processNo,TranslationEntry *entry);
	int AllocByForce(int processNo,TranslationEntry *entry);
private:
	BitMap *bitmap;
	Lock *lock;
	int *processMap;
	TranslationEntry **entries;
};

#endif
