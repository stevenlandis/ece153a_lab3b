#include "timing.h"
#include "timingTable.h"

int totalCount = 0;
static int en_prof = 0; // whether or not profiling is enabled

// Internal Function Declarations
void resetProfile();

void toggleProfiling() {
	if (en_prof == 0) {
		xil_printf("Starting Profiling\n");
		resetProfile();
		XTmrCtr_Start(&per_timer, 1);
		en_prof = 1;
	} else {
		XTmrCtr_Stop(&per_timer, 1);
		xil_printf("Stopping Profiling\n");
		printTimerData();
		en_prof = 0;
	}
}

void resetProfile() {
	for (int i = 0; i < N_DATA; i++) {
		T_Data[i].count = 0;
	}
}

void pollCode() {
	u32 a;

	// load return address into a
	// return address is where the interrupt was triggered from
	asm("add %0, r0, r14" : "=r"(a));

	// search data for
	int i = find_data_binary(a);
	T_Data[i].count++;
	totalCount++;
}

void printTimerData() {
	int sum = 0;

	xil_printf("Took %d samples\n", totalCount);
	for (int i = 0; i < N_DATA; i++) {
		if (T_Data[i].count > 0) {
			sum += T_Data[i].count;
			int p = (10000 * T_Data[i].count) / totalCount;
			int hi = p/100;
			int lo = p - 100*hi;

			xil_printf("%2d.%02d: %s\n", hi, lo, T_Data[i].name);

			//xil_printf("%f: %s", p, T_Data[i].name);
		}
	}
}

int find_data(u32 address) {
	int i;
	for (i = 0; i < N_DATA; i++) {
		if (T_Data[i].addr > address) {
			break;
		}
	}

	return i-1;
}

int find_data_binary(u32 address) {
	int lo, mid, hi;
	lo = 0;
	hi = N_DATA-1;

	while (lo <= hi) {
		mid = (lo+hi)/2;
//		xil_printf("%d, %d, %d\n",lo,mid,hi);
		if (T_Data[mid].addr < address) {
			lo = mid+1;
		} else if (T_Data[mid].addr == address) {
			return mid;
		} else {
			hi = mid-1;
		}
	}

//	xil_printf("addr: %d, lo: %d, hi: %d\n",address,lo,hi);

	return hi;
}


