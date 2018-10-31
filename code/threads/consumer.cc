#include "consumer.h"
#include "list.h"
#include "synch.h"

extern int pcsize;
extern List<int> *pcqueue;
extern Lock *pclock;
extern Condition *PCondition;
extern Condition *CCondition;

Consumer::Consumer(char *a)
{
	while(1){
		pclock->Acquire();
		printf("%s locked\n",a);
		if(pcsize==0) CCondition->Wait();
		//for(int i=0;i<100000000;i++);
		Delay(1);
		int x=pcqueue->Remove();
		pcsize--;
		printf("%s consumed item %d\n",a,x);
		printf("%s unlocked\n",a);
		pclock->Release();
		PCondition->Broadcast();
	}
}
