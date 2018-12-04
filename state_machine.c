#include "state_machine.h"
#include "qpn_port.h"
#include "xil_printf.h"
#include "note.h"

// --------------------------------
//  Type and Variable Declarations
// --------------------------------

enum Lab3bSignals {
	ENCODER_UP = Q_USER_SIG,
	ENCODER_DOWN,
	ENCODER_CLICK
};

static QEvent l_lab2aQueue[30];

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
QState SM_state_main(Lab3b_SM* me);
QState SM_state_menu(Lab3b_SM* me);
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
	me->octave = 0;
	me->f = 0;
	me->cents = 0;
	me->prevCents = 0;
	me->menuChoice = 0;

	// draw/erase flags
	me->drawBackground = 0;
	me->drawNote = me->eraseNote = 0;
	me->drawOctave = me->eraseOctave = 0;
	me->drawFreq = me->eraseFreq = 0;
	me->drawCents = me->eraseCents = 0;
	me->drawGoalBar = me->eraseGoalBar = 0;
	me->drawFreqBar = me->eraseFreqBar = 0;
	me->drawMenuItems = me->eraseMenuItems = 0;
	me->drawMenuMarker = me->eraseMenuMarker = 0;
	me->drawA4Hz = me->eraseA4Hz = 0;

	QActive_ctor(&me->super, (QStateHandler)&SM_state_init);
}

// first starting, screen in unknown state
QState SM_state_init(Lab3b_SM *me) {
	xil_printf("init\n");
	me->drawBackground = 1;
	setA4(440);
    return Q_TRAN(&SM_state_main);
}

QState SM_state_main(Lab3b_SM *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
//			xil_printf("on-entry\n");
			me->drawNote = 1;
			me->drawOctave = 1;
			me->drawFreq = 1;
			me->drawCents = 1;
			me->drawGoalBar = 1;
			me->drawFreqBar = 1;
			return Q_HANDLED();
			}

		case Q_INIT_SIG: {
//			xil_printf("on-init\n");
			return Q_HANDLED();
		}

		case Q_EXIT_SIG: {
			me->drawNote = 0;
			me->drawOctave = 0;
			me->drawFreq = 0;
			me->drawCents = 0;
			me->drawGoalBar = 0;
			me->drawFreqBar = 0;

			me->eraseNote = 1;
			me->eraseOctave = 1;
			me->eraseFreq = 1;
			me->eraseCents = 1;
			me->eraseGoalBar = 1;
			me->eraseFreqBar = 1;

			return Q_HANDLED();
		}

		case ENCODER_CLICK:
			return Q_TRAN(&SM_state_menu);
	}

	return Q_SUPER(&QHsm_top);
}

QState SM_state_menu(Lab3b_SM* me) {
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		me->drawMenuItems = 1;
		me->menuChoice = 0;
		me->drawMenuMarker = 1;
		return Q_HANDLED();

	case Q_INIT_SIG:
		return Q_HANDLED();

	case Q_EXIT_SIG:
		me->eraseMenuItems = 1;
		me->eraseMenuMarker = 1;
		return Q_HANDLED();

	case ENCODER_UP:
		me->menuChoice = (me->menuChoice+1)%3;
		me->drawMenuMarker = 1;
		return Q_HANDLED();

	case ENCODER_DOWN:
		me->menuChoice = (me->menuChoice+2)%3;
		me->drawMenuMarker = 1;
		return Q_HANDLED();

	case ENCODER_CLICK:
		switch (me->menuChoice) {
		case 0:
			return Q_TRAN(&SM_state_main);
		case 1:
			return Q_TRAN(&SM_state_main);
		case 2:
			return Q_TRAN(&SM_state_main);
		}
	}

	return Q_SUPER(&QHsm_top);
}
