/*
 * peripheral.h
 *
 *  Created on: Dec 1, 2018
 *      Author: stevenlandis
 */

#ifndef SRC_PERIPHERAL_H_
#define SRC_PERIPHERAL_H_

#include "xparameters.h"
#include "xtmrctr.h"
#include "xgpio.h"
#include "xintc.h"
#include "xspi.h"
#include "lcd.h"

// 1s --> 100000000
#define TIMER_DELAY 2*10000*10000

XIntc per_intc; // Interrupt controller

extern XGpio per_encoder;
//static XGpio per_btns;
extern XGpio per_leds;
extern XGpio per_dc;
extern XSpi per_spi;
extern XTmrCtr per_timer;

void TwistHandler(void *CallbackRef);
void ButtonHandler(void *CallbackRef);
void TimerHandler(void * CallbackRef);

void per_init();
void per_init2();

#endif /* SRC_PERIPHERAL_H_ */
