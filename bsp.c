#include "qpn_port.h"
#include "qepn.h"
#include "xil_printf.h"
#include "lcd.h"
#include "state_machine.h"
#include "fft.h"
#include "stream_grabber.h"
#include "note.h"
// Define functions for QPNano

void QF_onStartup() {
	//xil_printf("on startup\n");
	QF_INT_UNLOCK();
}

void QF_onIdle() {
	QF_INT_UNLOCK();

	get_fft_samples();
	stream_grabber_start();

	float freq = do_fft();
	int oct, note, cents;
	getFreqInfo(freq, &oct, &note, &cents);
//	SM_SM.f = 440;
//	SM_SM.cents = getCents(SM_SM.f);

//	xil_printf("f: %d --> o: %d\n", (int)SM_SM.f, getOctave(SM_SM.f));
//	xil_printf("f: %d --> o: %d, n: %d\n", (int)SM_SM.f, getOctave(SM_SM.f), getNoteN(SM_SM.f));
//	xil_printf("f: %d --> o: %d, n: %d, c: %d\n", (int)freq, oct, note, cents);

	switch(SM_SM.gBackground) {
	case NOTHING:
		break;
	case DRAW:
		setColor(100,100,100);
		fillRect(0,0,240-1,320-1);
		SM_SM.gBackground = NOTHING;
		break;
	case ERASE:
		SM_SM.gBackground = NOTHING;
		break;
	}

	switch(SM_SM.gNote) {
	case NOTHING:
		break;
	case DRAW:
		drawNote(note);
		break;
	case ERASE:
		eraseNote();
		SM_SM.gNote = NOTHING;
		break;
	}

	switch(SM_SM.gOctave) {
	case NOTHING:
		break;
	case DRAW:
		drawOctave(oct);
		break;
	case ERASE:
		eraseOctave();
		SM_SM.gOctave = NOTHING;
		break;
	}

	switch(SM_SM.gFreq) {
	case NOTHING:
		break;
	case DRAW:
		drawFreq(freq);
		break;
	case ERASE:
		eraseFreq();
		SM_SM.gFreq = NOTHING;
		break;
	}

	switch(SM_SM.gCents) {
	case NOTHING:
		break;
	case DRAW:
		drawCents(cents);
		break;
	case ERASE:
		eraseCents();
		SM_SM.gCents = NOTHING;
		break;
	}

	switch(SM_SM.gHistory) {
	case NOTHING:
		break;
	case DRAW:
		eraseHistory();
		stepHistory(cents);
		drawHistory();
		break;
	case ERASE:
		eraseHistory();
		SM_SM.gHistory = NOTHING;
		break;
	}

	if (SM_SM.gGoalBar == DRAW) {
		drawGoalBar();
		SM_SM.gGoalBar = NOTHING;
	}

	if (SM_SM.gFreqBar == DRAW) {
		drawFreqBar(cents);
	} else if (SM_SM.gFreqBar == ERASE) {
		eraseFreqBar();
		SM_SM.gFreqBar = NOTHING;
	}

	if (SM_SM.gGoalBar == ERASE) {
		eraseGoalBar();
		SM_SM.gGoalBar = NOTHING;
	}

	switch(SM_SM.gMenuItems) {
	case NOTHING:
		break;
	case DRAW:
		drawMenuItem(0, "Set Octave");
		drawMenuItem(1, "Set A4");
		drawMenuItem(2, "Back");
		SM_SM.gMenuItems = NOTHING;
		break;
	case ERASE:
		eraseMenuItem(0);
		eraseMenuItem(1);
		eraseMenuItem(2);
		SM_SM.gMenuItems = NOTHING;
		break;
	}

	switch(SM_SM.gMenuMarker) {
	case NOTHING:
		break;
	case DRAW:
		drawMenuMarker(SM_SM.menuChoice);
		SM_SM.gMenuMarker = NOTHING;
		break;
	case ERASE:
		eraseMenuMarker();
		SM_SM.gMenuMarker = NOTHING;
		break;
	}


	switch(SM_SM.gDrawOctaveRange) {
	case NOTHING:
		break;
	case DRAW:
		drawOctaveRange(SM_SM.octaveRange);
		SM_SM.gDrawOctaveRange = NOTHING;
		break;
	case ERASE:
		eraseOctaveRange();
		SM_SM.gDrawOctaveRange = NOTHING;
		break;
	}

	switch(SM_SM.gOctaveText){
	case NOTHING:
		break;
	case DRAW:
		drawOctaveText();
		SM_SM.gOctaveText = NOTHING;
		break;
	case ERASE:
		eraseOctaveText();
		SM_SM.gOctaveText = NOTHING;
		break;
	}

	switch(SM_SM.gA4Text){
	case NOTHING:
		break;
	case DRAW:
		drawA4Text();
		SM_SM.gA4Text = NOTHING;
		break;
	case ERASE:
		eraseA4Text();
		SM_SM.gA4Text = NOTHING;
		break;
	}

	switch(SM_SM.gDrawA4){
	case NOTHING:
		break;
	case DRAW:
		drawA4(SM_SM.A4);
		SM_SM.gDrawA4 = NOTHING;
		break;
	case ERASE:
		eraseA4();
		SM_SM.gDrawA4 = NOTHING;
		break;
	}
//	FLAG(eraseNote,
//		eraseNote(););
//	FLAG(eraseOctave,
//		eraseOctave(););
//	FLAG(eraseFreq,
//		eraseFreq(););
//	if (SM_SM.eraseCents) {
//		eraseCents();
//		SM_SM.eraseCents = 0;
//	}
//	if (SM_SM.eraseMenuItems) {
//		eraseMenuItem(0);
//		eraseMenuItem(1);
//		eraseMenuItem(2);
//		SM_SM.eraseMenuItems = 0;
//	}
//	if (SM_SM.eraseMenuMarker) {
//		eraseMenuMarker();
//		SM_SM.eraseMenuMarker = 0;
//	}
//	FLAG(eraseFreqBar,
//		eraseFreqBar(SM_SM.prevCents););
//	FLAG(eraseGoalBar,
//		eraseGoalBar(););
//
//	FLAG(drawBackground,
//		setColor(100,100,100);
//		fillRect(0,0,240-1,320-1);
//	);
//
//	if (SM_SM.drawNote) {
//		drawNote(SM_SM.f);
////		SM_SM.drawNote = 0;
//	}
//
//	if (SM_SM.drawOctave) {
//		drawOctave(SM_SM.octave);
////		SM_SM.drawOctave = 0;
//	}
//
//	if (SM_SM.drawFreq) {
//		drawFreq(SM_SM.f);
////		SM_SM.eraseFreq = 0;
//	}
//
//	if (SM_SM.drawCents) {
//		drawCents(SM_SM.cents);
////		SM_SM.drawCents = 0;
//	}
//
//	if (SM_SM.drawGoalBar) {
//		drawGoalBar();
//		SM_SM.drawGoalBar = 0;
//	}
//
//	if (SM_SM.drawFreqBar) {
//		int tempCents = SM_SM.cents;
//		drawFreqBar(tempCents, SM_SM.prevCents);
//		SM_SM.prevCents = tempCents;
////		SM_SM.drawFreqBar = 0;
//	}
//
////	if (SM_SM.eraseFreqBar) {
////		eraseFreqBar(SM_SM.prevCents);
////		SM_SM.eraseFreqBar = 0;
////	}
////
////	if (SM_SM.eraseGoalBar) {
////		eraseGoalBar();
////		SM_SM.eraseGoalBar = 0;
////	}
//
//	// menu state
//	if (SM_SM.drawMenuItems) {
//		drawMenuItem(0, "Set Octave");
//		drawMenuItem(1, "Set A4");
//		drawMenuItem(2, "Back");
//		SM_SM.drawMenuItems = 0;
//	}
//
//	if (SM_SM.drawMenuMarker) {
//		drawMenuMarker(SM_SM.menuChoice);
//		SM_SM.drawMenuMarker = 0;
//	}
}

void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}
