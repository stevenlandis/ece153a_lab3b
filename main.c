/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include <mb_interface.h>

#include "xparameters.h"
#include <xil_types.h>
#include <xil_assert.h>

#include <xio.h>
#include "xtmrctr.h"
#include "xintc.h"
#include "fft.h"
#include "trig.h"
#include "note.h"
#include "stream_grabber.h"

#include "timing.h"

//#define PROFILE_ON

#define SAMPLES 1024 // AXI4 Streaming Data FIFO has size 512
#define DOWN_SAMPLES 4
//#define SAMPLES 128 // AXI4 Streaming Data FIFO has size 512
#define M 8 //2^m=samples
#define CLOCK 100000000.0 //clock speed

//int int_buffer[4*SAMPLES];
static float q[SAMPLES/DOWN_SAMPLES];
static float w[SAMPLES/DOWN_SAMPLES];

XIntc per_intc; // Interrupt controller

//void print(char *str);

//void read_fsl_values(float* q, int n) {
//   int i;
////   unsigned int x;
//   stream_grabber_start();
//   stream_grabber_wait_enough_samples(512);
//   for(i = 0; i < n; i+=4) {
////      int_buffer[i] = stream_grabber_read_sample(i);
//      // xil_printf("%d\n",int_buffer[i]);
////      x = int_buffer[i];
//      //q[i] = 3.3*x/67108864.0; // 3.3V and 2^26 bit precision.
//      q[i/4] = .000000049173832*stream_grabber_read_sample(i);
//      //q[i] = .0000000492*x;
//
//   }
//}

void read_fsl_values(float* q, int n) {
   int i;
   unsigned int x;
//   stream_grabber_wait_enough_samples(512);
   stream_grabber_wait_enough_samples(SAMPLES);
   for(i = 0; i < n; i+=DOWN_SAMPLES) {
//      int_buffer[i] = stream_grabber_read_sample(i);
//      int_buffer[i+1] = stream_grabber_read_sample(i+1);
//      int_buffer[i+2] = stream_grabber_read_sample(i+2);
//      int_buffer[i+3] = stream_grabber_read_sample(i+3);
      // xil_printf("%d\n",int_buffer[i]);
//      x = (stream_grabber_read_sample(i)+ stream_grabber_read_sample(i+1)+ stream_grabber_read_sample(i+2)+stream_grabber_read_sample(i+3))/4;
	   x = stream_grabber_read_sample(i);
	   q[i/DOWN_SAMPLES] = .000000049173832*x;
   }
}

int main() {
   float sample_f;
   int l;
   int ticks; //used for timer
   uint32_t Control;
   float frequency; 
   float tot_time; //time to run program
   init_platform();

   //set up timer
   XTmrCtr_Initialize(&timer, XPAR_AXI_TIMER_0_DEVICE_ID);

   // set options for the first timer
   Control = XTmrCtr_GetOptions(&timer, 0) | XTC_CAPTURE_MODE_OPTION | XTC_INT_MODE_OPTION;
   XTmrCtr_SetOptions(&timer, 0, Control);

#ifdef PROFILE_ON
   // set up the interrupt handler
   XIntc_Initialize(&per_intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);

   // register timer handler
   XIntc_Connect(
   		&per_intc,
   		XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
   		TimerHandler,
   		&timer
   	);
   	XIntc_Enable(&per_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
   	XIntc_Start(&per_intc, XIN_REAL_MODE);

   	// set options for the second timer
   	   XTmrCtr_SetOptions(&timer, 1, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
   	   XTmrCtr_SetResetValue(&timer, 1, 0xFFFFFFFF-TIMER_DELAY);
//   	   XTmrCtr_SetResetValue(&timer, 1, TIMER_DELAY);

	// connect interrupt controller to microblaze
	microblaze_register_handler(
			(XInterruptHandler)XIntc_DeviceInterruptHandler,
		(void*)XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID
	);

	microblaze_enable_interrupts();
#endif

   xil_printf("Starting\n");

#ifdef PROFILE_ON
   XTmrCtr_Start(&timer, 0);
   XTmrCtr_Start(&timer, 1);
#endif

   stream_grabber_start();
   while(1) {
#ifndef PROFILE_ON
	   XTmrCtr_Start(&timer, 0);
#endif

      //Read Values from Microblaze buffer, which is continuously populated by AXI4 Streaming Data FIFO.
      read_fsl_values(q, SAMPLES);
      stream_grabber_start();

      sample_f = 100*1000*1000/2048.0/DOWN_SAMPLES;
      //xil_printf("sample frequency: %d \r\n",(int)sample_f);

      //zero w array
      for(l=0;l<SAMPLES/DOWN_SAMPLES;l++)
         w[l]=0; 

//      xil_printf("Before fft\n");
      frequency=fft(q,w,SAMPLES/DOWN_SAMPLES,M,sample_f);
//      xil_printf("After fft\n");

      //ignore noise below set frequency
      //if(frequency > 200.0) {
      //xil_printf("frequency: %d Hz\r\n", (int)(frequency+.5));
      findNote(frequency);

      //get time to run program
      ticks=XTmrCtr_GetValue(&timer, 0);
      //XTmrCtr_Stop(&timer, 0);
      tot_time=ticks/CLOCK;

#ifdef PROFILE_ON
      if ((int)(1000*tot_time)> 5*1000) {
         break;
      }
#else
      xil_printf("frequency: %6d Hz, program time: %dms \n",(int)(frequency+.5), (int)(1000*tot_time));
#endif
   }

   //xil_printf("Done\n");
   XTmrCtr_Stop(&timer, 0);
   XTmrCtr_Stop(&timer, 1);

   printTimerData();

   //+xil_printf("Done Printing\n");
   return 0;
}
