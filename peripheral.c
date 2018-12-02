/*
 * peripheral.c
 *
 *  Created on: Dec 1, 2018
 *      Author: stevenlandis
 */

#include "peripheral.h"

// include files for functions to call in interrupts
#include "timing.h"

XGpio per_encoder;
//static XGpio per_btns;
XGpio per_leds;
XGpio per_dc;
XSpi per_spi;
XTmrCtr per_timer;

void per_init() {
	// 16 leds
	XGpio_Initialize(&per_leds, XPAR_LEDS_DEVICE_ID);

	// encoder
	XGpio_Initialize(&per_encoder, XPAR_ENCODER_DEVICE_ID);

//	// buttons
//	XGpio_Initialize(&per_btns, XPAR_BTNS_DEVICE_ID);

	// lcd dc
	XGpio_Initialize(&per_dc, XPAR_SPI_DC_DEVICE_ID);

	// interrupt controller
	XIntc_Initialize(&per_intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);

	// timer
	XTmrCtr_Initialize(&per_timer, XPAR_AXI_TIMER_0_DEVICE_ID);

	// spi config
	XSpi_Config *spiConfig;	/* Pointer to Configuration data */
	spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID);
	XSpi_CfgInitialize(&per_spi, spiConfig, spiConfig->BaseAddress);

	// reset the spi device to leave it in a known state
	XSpi_Reset(&per_spi);

	// --------------------------------------------
	//  Connect things to the interrupt controller
	// --------------------------------------------

	// register encoder handler
	XIntc_Connect(
		&per_intc,
		XPAR_MICROBLAZE_0_AXI_INTC_ENCODER_IP2INTC_IRPT_INTR,
		TwistHandler,
		&per_encoder
	);
	XIntc_Enable(&per_intc, XPAR_MICROBLAZE_0_AXI_INTC_ENCODER_IP2INTC_IRPT_INTR);

	// register timer handler
	XIntc_Connect(
		&per_intc,
		XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
		TimerHandler,
		&per_timer
	);
	XIntc_Enable(&per_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);

	XIntc_Start(&per_intc, XIN_REAL_MODE);

	// -------------------------
	//  Set peripheral settings
	// -------------------------

	// set up the encoder
	XGpio_InterruptEnable(&per_encoder, 1);
	XGpio_InterruptGlobalEnable(&per_encoder);
	XGpio_SetDataDirection(&per_encoder, 1, 0xFFFFFFFF);

//	// set up the buttons
//	XGpio_InterruptEnable(&per_btns, 1);
//	XGpio_InterruptGlobalEnable(&per_btns);
//	XGpio_SetDataDirection(&per_btns, 1, 0xFFFFFFFF);

	// set up the lcd dc
	XGpio_SetDataDirection(&per_dc, 1, 0x0);

//	// set up the timer
//	XTmrCtr_SetOptions(
//		&per_timer,
//		0,
//		XTC_INT_MODE_OPTION
//	);

//	// timer time
//	XTmrCtr_SetResetValue(&per_timer, 0, 0xFFFFFFFF-TIMER_DELAY);

	// set options for the first timer
	u32 controlReg;
	controlReg = XTmrCtr_GetOptions(&per_timer, 0) | XTC_CAPTURE_MODE_OPTION | XTC_INT_MODE_OPTION;
	XTmrCtr_SetOptions(&per_timer, 0, controlReg);

	// set options for the second timer
	XTmrCtr_SetOptions(&per_timer, 1, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&per_timer, 1, 0xFFFFFFFF-TIMER_DELAY);
	XTmrCtr_SetResetValue(&per_timer, 1, TIMER_DELAY);

	// Setup the control register to enable master mode
	controlReg = XSpi_GetControlReg(&per_spi);
	XSpi_SetControlReg(&per_spi,
			(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
			(~XSP_CR_TRANS_INHIBIT_MASK));

	// Select 1st slave device
	XSpi_SetSlaveSelectReg(&per_spi, ~0x01);

	// initialize the LCD
	initLCD();

	// ---------------------------------
	//  Enable Interrupts on Microblaze
	// ---------------------------------

	// connect interrupt controller to microblaze
	microblaze_register_handler(
			(XInterruptHandler)XIntc_DeviceInterruptHandler,
		(void*)XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
	);

	microblaze_enable_interrupts();
}

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

// Timer
void TimerHandler(void * CallbackRef) {
//	xil_printf("t");

	pollCode();

	// acknowledge that interrupt handled
	u32 controlReg = XTimerCtr_ReadReg(per_timer.BaseAddress, 1, XTC_TCSR_OFFSET);
	XTmrCtr_WriteReg(
		per_timer.BaseAddress,
		1,
		XTC_TCSR_OFFSET,
		controlReg | XTC_CSR_INT_OCCURED_MASK
	);
}
