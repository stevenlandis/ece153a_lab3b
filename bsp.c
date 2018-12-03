#include "qpn_port.h"
#include "qepn.h"
#include "xil_printf.h"

// Define functions for QPNano

void QF_onStartup() {
	//xil_printf("on startup\n");
	QF_INT_UNLOCK();
}

void QF_onIdle() {
	QF_INT_UNLOCK();

	xil_printf("on idle\n");
}

void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}
