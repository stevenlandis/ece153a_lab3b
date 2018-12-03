#ifndef SRC_STATE_MACHINE_H_
#define SRC_STATE_MACHINE_H_

#include "xil_types.h"

void SM_init();
void SM_run();

void encoderHandler(u32 data);

#endif /* SRC_STATE_MACHINE_H_ */
