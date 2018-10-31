/*
 * swappage.cc
 *
 *  Created on: May 31, 2017
 *      Author: emon
 */

#include "swappage.h"
#include "system.h"
#include "copyright.h"

void Swappage::pagein(int p)
{
	for(int i=0;i<PageSize;i++){
		machine->mainMemory[p*PageSize+i]=page[i];
	}
}

void Swappage::pageout(int p)
{
	for(int i=0;i<PageSize;i++){
		page[i]=machine->mainMemory[p*PageSize+i];
	}
}


Swappage::Swappage()
{
	page=new char[PageSize];
}
