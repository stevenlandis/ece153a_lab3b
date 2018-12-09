#ifndef SRC_STATE_MACHINE_H_
#define SRC_STATE_MACHINE_H_

#include "xil_types.h"
#include "qpn_port.h"

enum UI_FLAG_TAG {
	NOTHING,
	DRAW,
	ERASE
};
typedef enum UI_FLAG_TAG UI_FLAG;

// declare struct for state machine
typedef struct Lab3b_SM_Tag {
	QActive super;

	// internal data
	int octaveRange;
	int A4;
	int menuChoice;
	int errorMode;

	// internal button state
	// to make sure button is released before re-pressing
	int buttonState;
	int buttonWaited;

	// draw/erase flags
	UI_FLAG gBackground;
	UI_FLAG gNote;
	UI_FLAG gOctave;
	UI_FLAG gFreq;
	UI_FLAG gCents;
	UI_FLAG gGoalBar;
	UI_FLAG gFreqBar;
	UI_FLAG gHistory;

	UI_FLAG gMenuItems;
	UI_FLAG gMenuMarker;

	UI_FLAG gDrawOctaveRange;
	UI_FLAG gOctaveText;
	UI_FLAG gA4Text;
	UI_FLAG gDrawA4;

} Lab3b_SM;

extern Lab3b_SM SM_SM;

void SM_init();
void SM_run();

void encoderHandler(u32 data);

#endif /* SRC_STATE_MACHINE_H_ */
