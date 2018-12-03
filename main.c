#include <stdio.h>
#include "platform.h"
#include "fft.h"
#include "note.h"
#include "stream_grabber.h"

#include "timing.h"
#include "peripheral.h"
#include "lcd.h"
#include "state_machine.h"

#define CLOCK 100000000.0 //clock speed

//#define PROFILE_ON

int main() {
   int ticks; //used for timer
   float frequency; 
   float tot_time; //time to run program
   init_platform();

   // set up peripherals
   per_init2();

   // initialize state machine
   SM_init();
//   clrScr();
//   for (int i = 0; i < 10; i ++) {
//	   fillRect(0,0,240-1,320-1);
//	   fillBackground(0,0,240-1,320-1);
//   }

   xil_printf("Starting\n");
//   XTmrCtr_Start(&per_timer, 1);
   SM_run();

   stream_grabber_start();
   while(1) {
	   XTmrCtr_Start(&per_timer, 0);

      //Read Values from Microblaze buffer, which is continuously populated by AXI4 Streaming Data FIFO.
	  get_fft_samples();
      stream_grabber_start();

//      frequency = do_fft();
      frequency = do_fft();
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
