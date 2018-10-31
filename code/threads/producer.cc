#include "producer.h"
#include "list.h"
#include "synch.h"
#define MAX_SIZE 10

extern int pcsize;
extern List<int> *pcqueue;
extern Lock *pclock;
extern Condition *PCondition;
extern Condition *CCondition;
extern int item;

Producer::Producer(char *a)
{
	int count=0;
	while(1){
		pclock->Acquire();
		printf("%s locked\n",a);
		if(pcsize==MAX_SIZE) PCondition->Wait();
		//for(int i=0;i<100000000;i++);
		Delay(1);
		pcqueue->Append(item);
		printf("%s produced item %d\n",a,item);
		item++;
		pcsize++;
		count++;
		printf("%s unlocked\n",a);
		pclock->Release();
		//if(count=5) break;
		CCondition->Broadcast();

	}
}
