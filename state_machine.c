#include "state_machine.h"
#include "qpn_port.h"
#include "xil_printf.h"

// --------------------------------
//  Type and Variable Declarations
// --------------------------------

enum Lab3bSignals {
	ENCODER_UP = Q_USER_SIG,
	ENCODER_DOWN,
	ENCODER_CLICK
};

static QEvent l_lab2aQueue[30];

// declare struct for state machine
typedef struct Lab3b_SM_Tag {
	QActive super;

	// internal data
	int note;
	int octave;
	int sharp;
	float f;
	float cents;
	int menuChoice;

	// draw/erase flags
	int drawBackground;
	int drawNote, eraseNote;
	int drawOctave, eraseOctave;
	int drawSharp, eraseSharp;
	int drawF, eraseF;
	int drawFHz, eraseFHz;
	int drawCents, eraseCents;
	int drawErrorBlackLine, eraseErrorBlackLine;
	int drawErrorRedLine, eraseErrorRedLine;
	int drawMenuOctave, eraseMenuOctave;
	int drawMenuA4, eraseMenuA4;
	int drawChoiceOctave, eraseChoiceOctave;
	int drawChoiceA4, eraseChoiceA4;
	int drawA4Hz, eraseA4Hz;

} Lab3b_SM;

// declare main state machine
Lab3b_SM SM_SM;

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0,            (QEvent *)0,          0                    },
	{ (QActive *)&SM_SM,    l_lab2aQueue,         Q_DIM(l_lab2aQueue)  }
};

volatile unsigned int encoderState = 0;

// --------------------------------
//  Internal Function Declarations
// --------------------------------
void SM_ctor();
QState SM_state_init(Lab3b_SM* me);
QState SM_state_on(Lab3b_SM* me);
void updateEncoderState(u32 data);
void twistDown();
void twistUp();

// -------------------------------
//  External Function Definitions
// -------------------------------
void SM_init() {
	SM_ctor();
}

void SM_run() {
	QF_run();
}

void twistDown() {
	QActive_post((QActive*)(&SM_SM), ENCODER_DOWN);
//	xil_printf("d\n");
}

void twistUp() {
	QActive_post((QActive*)(&SM_SM), ENCODER_UP);
//	xil_printf("u\n");
}

void encoderHandler(u32 encoderData) {
	u32 button = encoderData & 0b100;

	if (button) {
		QActive_post((QActive*)(&SM_SM), ENCODER_CLICK);
	}

	u32 ab = encoderData & 0b11;

	switch(encoderState) {
	case 0:
		if        (ab == 0b01) {
			encoderState = 1;
		} else if (ab == 0b10) {
			encoderState = 4;
		}
		break;
	case 1:
		if        (ab == 0b11) {
			encoderState = 0;
		} else if (ab == 0b00) {
			encoderState = 2;
		}
		break;
	case 2:
		if        (ab == 0b01) {
			encoderState = 1;
		} else if (ab == 0b10) {
			encoderState = 3;
		}
		break;
	case 3:
		if        (ab == 0b11) {
			encoderState = 0;

			twistUp();
		} else if (ab == 0b00) {
			encoderState = 2;
		}
		break;
	case 4:
		if        (ab == 0b00) {
			encoderState = 5;
		} else if (ab == 0b11) {
			encoderState = 0;
		}
		break;
	case 5:
		if        (ab == 0b01) {
			encoderState = 6;
		} else if (ab == 0b10) {
			encoderState = 4;
		}
		break;
	case 6:
		if        (ab == 0b11) {
			encoderState = 0;

			twistDown();
		} else if (ab == 0b00) {
			encoderState = 5;
		}
		break;
	}
}

// -------------------------------
//  Internal Function Definitions
// -------------------------------
void SM_ctor() {
	Lab3b_SM* me = &SM_SM;

	xil_printf("Constructing SM\n");

	//initialize internal variables
	// internal data
	me->note = 0;
	me->octave = 0;
	me->sharp = 0;
	me->f = 0;
	me->cents = 0;
	me->menuChoice = 0;

	// draw/erase flags
	me->drawBackground = 0;
	me->drawNote = me->eraseNote = 0;
	me->drawOctave = me->eraseOctave = 0;
	me->drawSharp = me->eraseSharp = 0;
	me->drawF = me->eraseF = 0;
	me->drawFHz = me->eraseFHz = 0;
	me->drawCents = me->eraseCents = 0;
	me->drawErrorBlackLine = me->eraseErrorBlackLine = 0;
	me->drawErrorRedLine = me->eraseErrorRedLine = 0;
	me->drawMenuOctave = me->eraseMenuOctave = 0;
	me->drawMenuA4 = me->eraseMenuA4 = 0;
	me->drawChoiceOctave = me->eraseChoiceOctave = 0;
	me->drawChoiceA4 = me->eraseChoiceA4 = 0;
	me->drawA4Hz = me->eraseA4Hz = 0;

	QActive_ctor(&me->super, (QStateHandler)&SM_state_init);
}

// first starting, screen in unknown state
QState SM_state_init(Lab3b_SM *me) {
	xil_printf("init\n");
    return Q_TRAN(&SM_state_on);
}

QState SM_state_on(Lab3b_SM *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("on-entry\n");
			return Q_HANDLED();
			}

		case Q_INIT_SIG: {
			xil_printf("on-init\n");
			return Q_HANDLED();
		}
	}

	return Q_SUPER(&QHsm_top);
}
