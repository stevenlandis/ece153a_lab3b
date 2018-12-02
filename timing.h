/*
 * timing.h
 *
 *  Created on: Nov 18, 2018
 *      Author: stevenlandis
 */

#ifndef SRC_TIMING_H_
#define SRC_TIMING_H_

#include "peripheral.h"

struct T_InfoTag {
	int addr;
	char* name;
	int count;
};
typedef struct T_InfoTag T_Info;

//XTmrCtr timer; // Timer
extern T_Info T_Data[];
#define N_DATA 167

extern int totalCount;

int find_data(u32 address);

void pollCode();

void printTimerData();

#endif /* SRC_TIMING_H_ */
