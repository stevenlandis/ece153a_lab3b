#include "state_machine.h"
#include "qpn_port.h"
#include "xil_printf.h"
#include "note.h"
#include "fft.h"
#include "peripheral.h"
// --------------------------------
//  Type and Variable Declarations
// --------------------------------

enum Lab3bSignals {
	ENCODER_UP = Q_USER_SIG,
	ENCODER_DOWN,
	BUTTON_PRESSED,         // fires when encoder changes and button is down
					        // and fails when button is pressed and twisted at the same time
	BUTTON_RELEASED,
	BUTTON_CLICK,           // fires when button changes from up to down
	SWITCH_TOGGLE
};

static QEvent l_lab2aQueue[1000];

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
QState SM_state_setOctave(Lab3b_SM* me);
QState SM_state_setA4(Lab3b_SM* me);
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

static int pastSwitchState = 0;
void encoderHandler(u32 encoderData) {
	u32 button = encoderData & 0b100;

	if (button) {
		QActive_post((QActive*)(&SM_SM), BUTTON_PRESSED);
		QActive_post((QActive*)(&SM_SM), BUTTON_RELEASED);
	} else {
//		QActive_post((QActive*)(&SM_SM), BUTTON_RELEASED);
	}

	u32 switchState = encoderData & 0b1000;

	if (switchState != pastSwitchState) {
		pastSwitchState = switchState;
		QActive_post((QActive*)(&SM_SM), SWITCH_TOGGLE);
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
//	me->octave = 0;
//	me->f = 0;
//	me->cents = 0;
//	me->prevCents = 0;
	me->menuChoice = 0;
	me->errorMode = 0;
	me->octaveRange = 0;
	me->A4 = 440;
	// button state starts as up
	me->buttonState = 0;
	me->buttonWaited = 1; // whether or not enough time has passed between button presses

	// reset all drawing flags
	me->gBackground = NOTHING;
	me->gNote = NOTHING;
	me->gOctave = NOTHING;
	me->gFreq = NOTHING;
	me->gCents = NOTHING;
	me->gGoalBar = NOTHING;
	me->gFreqBar = NOTHING;
	me->gHistory = NOTHING;

	me->gMenuItems = NOTHING;
	me->gMenuMarker = NOTHING;

	me->gDrawOctaveRange = NOTHING;
	me->gOctaveText = NOTHING;
	me->gA4Text = NOTHING;
	me->gDrawA4 = NOTHING;

	QActive_ctor(&me->super, (QStateHandler)&SM_state_init);
}

// first starting, screen in unknown state
QState SM_state_init(Lab3b_SM *me) {
	xil_printf("init\n");
	me->gBackground = DRAW;
	setA4(440);
	setOctaveRange(9);
    return Q_TRAN(&SM_state_main);
}

QState SM_state_on(Lab3b_SM* me) {
	switch(Q_SIG(me)) {
	case Q_INIT_SIG:
		return Q_TRAN(&SM_state_main);
	case BUTTON_PRESSED:
		if (me->buttonState == 0 && me->buttonWaited) {
			me->buttonWaited = 0;
			XTmrCtr_Start(&per_timer, 0);
			XGpio_DiscreteWrite(&per_leds, 1, 0xFFFF);
			QActive_post((QActive*)(&SM_SM), BUTTON_CLICK);
			me->buttonState = 1;
		}
		return Q_HANDLED();
	case BUTTON_RELEASED:
		me->buttonState = 0;
		return Q_HANDLED();
	}

	return Q_SUPER(&QHsm_top);
}

QState SM_state_main(Lab3b_SM *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
//			xil_printf("on-entry\n");
			me->gNote = DRAW;
			me->gOctave = DRAW;
			me->gFreq = DRAW;
			me->gCents = DRAW;

			if (me->errorMode == 0) {
				me->gGoalBar = DRAW;
				me->gFreqBar = DRAW;
			} else {
				me->gHistory = DRAW;
			}
			return Q_HANDLED();
		}

		case Q_INIT_SIG: {
//			xil_printf("on-init\n");
			return Q_HANDLED();
		}
		case Q_EXIT_SIG: {
			me->gNote = ERASE;
			me->gOctave = ERASE;
			me->gFreq = ERASE;
			me->gCents = ERASE;
			me->gGoalBar = ERASE;
			me->gFreqBar = ERASE;

			if (me->errorMode == 0) {
				me->gGoalBar = ERASE;
				me->gFreqBar = ERASE;
			} else {
				me->gHistory = ERASE;
			}

			return Q_HANDLED();
		}

		case BUTTON_CLICK:
			return Q_TRAN(&SM_state_menu);

		case SWITCH_TOGGLE:
			if (me->errorMode == 0) {
				me->errorMode = 1;
				me->gGoalBar = ERASE;
				me->gFreqBar = ERASE;
				me->gHistory = DRAW;
			} else {
				me->errorMode = 0;
				me->gGoalBar = DRAW;
				me->gFreqBar = DRAW;
				me->gHistory = ERASE;
			}
			return Q_HANDLED();
	}

	return Q_SUPER(&SM_state_on);
}

QState SM_state_menu(Lab3b_SM* me) {
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG:
		me->gMenuItems = DRAW;
		me->menuChoice = 0;
		me->gMenuMarker = DRAW;
		return Q_HANDLED();

	case Q_INIT_SIG:
		return Q_HANDLED();

	case Q_EXIT_SIG:
		me->gMenuItems = ERASE;
		me->gMenuMarker = ERASE;
		return Q_HANDLED();

	case ENCODER_UP:
		if (me->menuChoice < 2) {
			me->menuChoice++;
			me->gMenuMarker = DRAW;
		}
		return Q_HANDLED();

	case ENCODER_DOWN:
		if (me->menuChoice > 0) {
			me->menuChoice--;
			me->gMenuMarker = DRAW;
		}
		return Q_HANDLED();

	case BUTTON_CLICK:
		switch (me->menuChoice) {
		case 0:
			return Q_TRAN(&SM_state_setOctave);
		case 1:
			return Q_TRAN(&SM_state_setA4);
		case 2:
			return Q_TRAN(&SM_state_main);
		}
	}

	return Q_SUPER(&SM_state_on);
}
QState SM_state_setOctave(Lab3b_SM* me) {
	switch (Q_SIG(me)){
	case Q_ENTRY_SIG:{
		me->gOctaveText = DRAW;
		me->gDrawOctaveRange = DRAW;
		return Q_HANDLED();
	}
	case Q_EXIT_SIG:{ //erase flags
		me->gOctaveText = ERASE;
		me->gDrawOctaveRange= ERASE;
		return Q_HANDLED();
	}
	case Q_INIT_SIG:{
		return Q_HANDLED();
	}
	case ENCODER_UP:{
		if(me->octaveRange<9){
			me->octaveRange++;
			me->gDrawOctaveRange = DRAW;
		}
		return Q_HANDLED();
	}
	case ENCODER_DOWN:{
		if(me->octaveRange>0){
			me->octaveRange--;
			me->gDrawOctaveRange = DRAW;
		}
		return Q_HANDLED();

	}
	case BUTTON_CLICK:{ //set octaveChoice
		setOctaveRange(me->octaveRange);
		return Q_TRAN(&SM_state_main);
	}
	}
	return Q_SUPER(&SM_state_on);


}

QState SM_state_setA4(Lab3b_SM* me){
	switch (Q_SIG(me)){
	case Q_ENTRY_SIG:{
		me->gA4Text = DRAW;
		me->gDrawA4 = DRAW;
		return Q_HANDLED();
	}
	case Q_EXIT_SIG:{ //erase flags
		me->gA4Text = ERASE;
		me->gDrawA4= ERASE;
		return Q_HANDLED();
	}
	case Q_INIT_SIG:{
		return Q_HANDLED();
	}
	case ENCODER_UP:{
		if(me->A4<460){
			me->A4++;
			me->gDrawA4= DRAW;
		}
		return Q_HANDLED();
	}
	case ENCODER_DOWN:{
		if(me->A4>420){
			me->A4--;
			me->gDrawA4 = DRAW;
		}
		return Q_HANDLED();

	}
	case BUTTON_CLICK:{ //set octaveChoice
		setA4(me->A4);
		return Q_TRAN(&SM_state_main);
	}
	}
	return Q_SUPER(&SM_state_on);




}

