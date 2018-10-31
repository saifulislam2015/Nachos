#include "memorymanager.h"

extern Table *ptable;
extern int timestamp[NumPhysPages];

MemoryManager::MemoryManager(int numPages)
{
	bitmap=new BitMap(numPages);
	lock=new Lock("MemoryManagerLock");
	processMap=new int[NumPhysPages];
	entries=new TranslationEntry*[NumPhysPages];
}

MemoryManager::~MemoryManager()
{
	delete bitmap;
	delete lock;
}

int MemoryManager::AllocPage()
{
	lock->Acquire();
	int x=bitmap->Find();
	lock->Release();
	return x;
}

void MemoryManager::FreePage(int physPageNum)
{
	lock->Acquire();
	bitmap->Clear(physPageNum);
	lock->Release();
}

bool MemoryManager::PageIsAllocated(int physPageNum)
{
	lock->Acquire();
	bool b=bitmap->Test(physPageNum);
	lock->Release();
	return b;
}

int MemoryManager::Alloc(int processNo,TranslationEntry *entry){
	lock->Acquire();
	//printf("just alloc\n");
	int x=bitmap->Find();
	if(x!=-1){
		processMap[x]=processNo;
		entries[x]=entry;
	}
	lock->Release();
	return x;
}


// Random
int MemoryManager::AllocByForce(int processNo,TranslationEntry *entry)
{
	lock->Acquire();
	int t=Random()%NumPhysPages;
	entries[t]->valid=false;
	Thread * th= (Thread *) ptable->Get(processMap[t]);
	th->space->saveIntoSwapSpace(entries[t]->virtualPage);
	lock->Release();
	return t;
}

/*
// LRU
int MemoryManager::AllocByForce(int processNo,TranslationEntry *entry)
{
	lock->Acquire();
	int t= Random()%NumPhysPages;  //random
	//printf("hur\n");
	//int t=0;
	for(int i=1;i<NumPhysPages;i++){
		if(timestamp[i]<timestamp[t]) t=i;
	}
	printf("%d\n",t);
	entries[t]->valid=false;
	//printf("here =========  %d\n",entries[t]->physicalPage);
	Thread * th= (Thread *) ptable->Get(processMap[t]);
	th->space->saveIntoSwapSpace(entries[t]->virtualPage);

	//processMap[t]=processNo;
	//entries[t]=entry;

	lock->Release();
	return t;
}
*/








