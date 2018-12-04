#ifndef SRC_STATE_MACHINE_H_
#define SRC_STATE_MACHINE_H_

#include "xil_types.h"
#include "qpn_port.h"

// declare struct for state machine
typedef struct Lab3b_SM_Tag {
	QActive super;

	// internal data
	int octave;
	float f;
	float cents, prevCents;
	int menuChoice;

	// draw/erase flags
	int drawBackground;
	int drawNote, eraseNote;
	int drawOctave, eraseOctave;
	int drawFreq, eraseFreq;
	int drawFHz, eraseFHz;
	int drawCents, eraseCents;
	int drawGoalBar, eraseGoalBar;
	int drawFreqBar, eraseFreqBar;
	int drawMenuItems, eraseMenuItems;
	int drawMenuMarker, eraseMenuMarker;
	int drawA4Hz, eraseA4Hz;

} Lab3b_SM;

extern Lab3b_SM SM_SM;

void SM_init();
void SM_run();

void encoderHandler(u32 data);

#endif /* SRC_STATE_MACHINE_H_ */
