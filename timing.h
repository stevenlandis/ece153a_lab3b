/*
 * timing.h
 *
 *  Created on: Nov 18, 2018
 *      Author: stevenlandis
 */

#ifndef SRC_TIMING_H_
#define SRC_TIMING_H_

#include "peripheral.h"

////XTmrCtr timer; // Timer
//extern T_Info T_Data[];
//#define N_DATA 167

extern int totalCount;

int find_data(u32 address);
int find_data_binary(u32 address);

void pollCode();
void toggleProfiling();

void printTimerData();

#endif /* SRC_TIMING_H_ */
