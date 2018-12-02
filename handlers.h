/*
 * handlers.h
 *
 *  Created on: Dec 1, 2018
 *      Author: stevenlandis
 */

#ifndef SRC_HANDLERS_H_
#define SRC_HANDLERS_H_

#include "peripheral.h"

// Encoder
void TwistHandler(void *CallbackRef) {
	u32 data = XGpio_DiscreteRead(&per_encoder, 1);
	XGpio_DiscreteWrite(&per_leds, 1, data);

//	updateEncoderState(data);
	// start the timer
//	XTmrCtr_Start(&per_timer, 0);

	// mark interrupt as handled
	XGpio_InterruptClear(&per_encoder, 0xFFFFFFFF);
}

void TimerHandler(void * CallbackRef) {
	// handler code
//	QActive_post((QActive*)(&AO_Lab2A), TIMER_END);

	xil_printf("t");

//	AO_Lab2A.volume_on_screen = 0;
//	clearVolume();
//
//	AO_Lab2A.mode_on_screen = 0;
//	clearMode();

	// acknowledge that interrupt handled
	u32 controlReg = XTimerCtr_ReadReg(per_timer.BaseAddress, 0, XTC_TCSR_OFFSET);
	XTmrCtr_WriteReg(
		per_timer.BaseAddress,
		0,
		XTC_TCSR_OFFSET,
		controlReg | XTC_CSR_INT_OCCURED_MASK
	);
}

#endif /* SRC_HANDLERS_H_ */
