/*
 * swappage.h
 *
 *  Created on: May 31, 2017
 *      Author: emon
 */

#ifndef SWAPPAGE_H
#define SWAPPAGE_H


class Swappage
{
	char * page;
public:
	Swappage();
	void pageout(int p);
	void pagein(int p);
};





#endif /* NACHOS_NACHOS_CODE_USERPROG_SWAPPAGE_H_ */
