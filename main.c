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
#include "peripheral.h"
#include "lcd.h"

//#define PROFILE_ON


// Buffers for storing samples and doing fourier transform
static float q[SAMPLES/DOWN_SAMPLES];
static float w[SAMPLES/DOWN_SAMPLES];


int main() {
   float sample_f;
   int l;
   int ticks; //used for timer
   float frequency; 
   float tot_time; //time to run program
   init_platform();

   // set up peripherals
   per_init();
//   clrScr();
   for (int i = 0; i > 10; i ++) {
	   fillRect(0,0,240-1,320-1);
	   fillBackground(0,0,240-1,320-1);
   }

   xil_printf("Starting\n");

   stream_grabber_start();
   while(1) {
	   XTmrCtr_Start(&per_timer, 0);

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
//      findNote(frequency);

      //get time to run program
      ticks=XTmrCtr_GetValue(&per_timer, 0);
      //XTmrCtr_Stop(&timer, 0);
      tot_time=ticks/CLOCK;

      xil_printf("frequency: %6d Hz, program time: %dms \n",(int)(frequency+.5), (int)(1000*tot_time));
   }

   //xil_printf("Done\n");
   XTmrCtr_Stop(&per_timer, 0);
   XTmrCtr_Stop(&per_timer, 1);

   printTimerData();

   //+xil_printf("Done Printing\n");
   return 0;
}
