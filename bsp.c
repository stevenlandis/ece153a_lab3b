#include "qpn_port.h"
#include "qepn.h"
#include "xil_printf.h"
#include "lcd.h"
#include "state_machine.h"
#include "fft.h"
#include "stream_grabber.h"
#include "note.h"

#define FLAG(flag_,fcn_) if(SM_SM.flag_){fcn_ SM_SM.flag_=0;}

// Define functions for QPNano

void QF_onStartup() {
	//xil_printf("on startup\n");
	QF_INT_UNLOCK();
}

void QF_onIdle() {
	QF_INT_UNLOCK();

	get_fft_samples();
	stream_grabber_start();

	SM_SM.f = do_fft();
//	SM_SM.f = 440;
	SM_SM.cents = getCents(SM_SM.f);

//	xil_printf("f: %d --> o: %d\n", (int)SM_SM.f, getOctave(SM_SM.f));
	xil_printf("f: %d --> o: %d, n: %d\n", (int)SM_SM.f, getOctave(SM_SM.f), getNoteN(SM_SM.f));

	FLAG(eraseNote,
		eraseNote(););
	FLAG(eraseOctave,
		eraseOctave(););
	FLAG(eraseFreq,
		eraseFreq(););
	if (SM_SM.eraseCents) {
		eraseCents();
		SM_SM.eraseCents = 0;
	}
	if (SM_SM.eraseMenuItems) {
		eraseMenuItem(0);
		eraseMenuItem(1);
		eraseMenuItem(2);
		SM_SM.eraseMenuItems = 0;
	}
	if (SM_SM.eraseMenuMarker) {
		eraseMenuMarker();
		SM_SM.eraseMenuMarker = 0;
	}
	FLAG(eraseFreqBar,
		eraseFreqBar(SM_SM.prevCents););
	FLAG(eraseGoalBar,
		eraseGoalBar(););

	FLAG(drawBackground,
		setColor(100,100,100);
		fillRect(0,0,240-1,320-1);
	);

	if (SM_SM.drawNote) {
		drawNote(SM_SM.f);
//		SM_SM.drawNote = 0;
	}

	if (SM_SM.drawOctave) {
		drawOctave(SM_SM.octave);
//		SM_SM.drawOctave = 0;
	}

	if (SM_SM.drawFreq) {
		drawFreq(SM_SM.f);
//		SM_SM.eraseFreq = 0;
	}

	if (SM_SM.drawCents) {
		drawCents(SM_SM.cents);
//		SM_SM.drawCents = 0;
	}

	if (SM_SM.drawGoalBar) {
		drawGoalBar();
		SM_SM.drawGoalBar = 0;
	}

	if (SM_SM.drawFreqBar) {
		int tempCents = SM_SM.cents;
		drawFreqBar(tempCents, SM_SM.prevCents);
		SM_SM.prevCents = tempCents;
//		SM_SM.drawFreqBar = 0;
	}

//	if (SM_SM.eraseFreqBar) {
//		eraseFreqBar(SM_SM.prevCents);
//		SM_SM.eraseFreqBar = 0;
//	}
//
//	if (SM_SM.eraseGoalBar) {
//		eraseGoalBar();
//		SM_SM.eraseGoalBar = 0;
//	}

	// menu state
	if (SM_SM.drawMenuItems) {
		drawMenuItem(0, "Set Octave");
		drawMenuItem(1, "Set A4");
		drawMenuItem(2, "Back");
		SM_SM.drawMenuItems = 0;
	}

	if (SM_SM.drawMenuMarker) {
		drawMenuMarker(SM_SM.menuChoice);
		SM_SM.drawMenuMarker = 0;
	}
}

void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}
