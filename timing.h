/*
 * timing.h
 *
 *  Created on: Nov 18, 2018
 *      Author: stevenlandis
 */

#ifndef SRC_TIMING_H_
#define SRC_TIMING_H_

#define TIMER_DELAY 1*10000

struct T_InfoTag {
	int addr;
	char* name;
	int count;
};
typedef struct T_InfoTag T_Info;

XTmrCtr timer; // Timer
T_Info T_Data[];
#define N_DATA 167

int totalCount = 0;

int find_data(u32 address);
void TimerHandler(void * CallbackRef);

void TimerHandler(void * CallbackRef) {
	u32 a;

	// load return address into a
	// return address is where the interrupt was triggered from
	asm("add %0, r0, r14" : "=r"(a));

	// search data for
	int i = find_data(a);
//	xil_printf("%08x: %d, %s\n", a, i, T_Data[i].name);
	T_Data[i].count++;
	totalCount++;

//	xil_printf("%08x\n", a);

	// acknowledge that interrupt handled
	u32 controlReg = XTimerCtr_ReadReg(timer.BaseAddress, 1, XTC_TCSR_OFFSET);
	XTmrCtr_WriteReg(
		timer.BaseAddress,
		1,
		XTC_TCSR_OFFSET,
		controlReg | XTC_CSR_INT_OCCURED_MASK
	);
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
	xil_printf("Printed %d samples\n", sum);
}

int find_data(u32 address) {
	int i;
	for (i = 0; i < N_DATA; i++) {
		if (T_Data[i].addr >= address) {
			break;
		}
	}

	return i-1;
}

//int find_data_bin_search(u32 address) {
//	int i;
//	int lo = 0;
//	int hi = N_DATA-1;
//	int mid = (lo + hi)/2;
//
//	while (lo > hi) {
//
//	}
//}

T_Info T_Data[] = {
	    {0x00000000, "_start", 0},
	    {0x00000008, "_vector_sw_exception", 0},
	    {0x00000010, "_vector_interrupt", 0},
	    {0x00000020, "_vector_hw_exception", 0},
	    {0x00000050, "_start1", 0},
	    {0x00000080, "_exit", 0},
	    {0x00000084, "deregister_tm_clones", 0},
	    {0x000000d4, "register_tm_clones", 0},
	    {0x0000012c, "__do_global_dtors_aux", 0},
	    {0x00000200, "frame_dummy", 0},
	    {0x00000268, "_crtinit", 0},
	    {0x00000318, "_hw_exception_handler", 0},
	    {0x0000033c, "ex_handler_no_ds", 0},
	    {0x00000358, "handle_other_ex", 0},
	    {0x00000374, "handle_other_ex_tail", 0},
	    {0x000003e0, "ex_handler_done", 0},
	    {0x00000400, "_HEAP_SIZE", 0},
	    {0x00000400, "_STACK_SIZE", 0},
	    {0x00000404, "ex_handler_unhandled", 0},
	    {0x00000408, "microblaze_enable_dcache", 0},
	    {0x00000410, "microblaze_enable_icache", 0},
	    {0x00000418, "microblaze_enable_interrupts", 0},
	    {0x00000420, "__extendsfdf2", 0},
	    {0x00000468, "__muldf3", 0},
	    {0x00000808, "__divdf3", 0},
	    {0x00000a44, "__floatsidf", 0},
	    {0x00000b2c, "__make_dp", 0},
	    {0x00000b60, "__truncdfsf2", 0},
	    {0x00000bcc, "__floatunsidf", 0},
	    {0x00000c74, "__muldi3", 0},
	    {0x00000ce4, "__clzsi2", 0},
	    {0x00000d80, "__unpack_f", 0},
	    {0x00000e50, "__make_fp", 0},
	    {0x00000e80, "__pack_d", 0},
	    {0x00001118, "__unpack_d", 0},
	    {0x00001230, "__pack_f", 0},
	    {0x00001434, "exit", 0},
	    {0x00001474, "strlen", 0},
	    {0x00001478, "check_alignment", 0},
	    {0x00001480, "len_loop", 0},
	    {0x00001494, "end_len", 0},
	    {0x000014a4, "done_len", 0},
	    {0x000014ac, "align_arg", 0},
	    {0x000014b0, "align_loop", 0},
	    {0x000014d0, "__call_exitprocs", 0},
	    {0x00001698, "_exception_handler", 0},
	    {0x0000169c, "_program_clean", 0},
	    {0x000016a4, "_program_init", 0},
	    {0x000016ac, "__do_global_ctors_aux", 0},
	    {0x000016fc, "mult_real", 0},
	    {0x00001748, "mult_im", 0},
	    {0x00001794, "fft", 0},
	    {0x000020a4, "Xil_In32", 0},
	    {0x000020d0, "Xil_Out32", 0},
	    {0x00002110, "TimerHandler", 0},
	    {0x000021f4, "printTimerData", 0},
	    {0x00002338, "find_data", 0},
	    {0x000023b4, "read_fsl_values", 0},
	    {0x000024c0, "main", 0},
	    {0x000027d0, "findNote", 0},
	    {0x000029c4, "enable_caches", 0},
	    {0x00002a08, "init_uart", 0},
	    {0x00002a2c, "init_platform", 0},
	    {0x00002a70, "stream_grabber_start", 0},
	    {0x00002aa0, "stream_grabber_wait_enough_samples", 0},
	    {0x00002ae4, "stream_grabber_read_sample", 0},
	    {0x00002b24, "fastCos", 0},
	    {0x00002b6c, "fastSin", 0},
	    {0x00002bb4, "__interrupt_handler", 0},
	    {0x00002bb4, "_interrupt_handler", 0},
	    {0x00002c58, "microblaze_register_handler", 0},
	    {0x00002c6c, "Xil_Assert", 0},
	    {0x00002cb4, "XNullHandler", 0},
	    {0x00002cbc, "getnum", 0},
	    {0x00002d2c, "padding.part.0", 0},
	    {0x00002d84, "outnum", 0},
	    {0x00002f30, "xil_printf", 0},
	    {0x000032c4, "XIntc_DeviceInterruptHandler", 0},
	    {0x00003418, "StubHandler", 0},
	    {0x00003468, "XIntc_Initialize", 0},
	    {0x0000364c, "XIntc_Start", 0},
	    {0x00003754, "XIntc_Connect", 0},
	    {0x00003864, "XIntc_Enable", 0},
	    {0x00003940, "XTmrCtr_StubCallback", 0},
	    {0x000039bc, "XTmrCtr_CfgInitialize", 0},
	    {0x00003a98, "XTmrCtr_InitHw", 0},
	    {0x00003bcc, "XTmrCtr_Initialize", 0},
	    {0x00003c98, "XTmrCtr_Start", 0},
	    {0x00003da4, "XTmrCtr_Stop", 0},
	    {0x00003e9c, "XTmrCtr_GetValue", 0},
	    {0x00003f90, "XTmrCtr_SetResetValue", 0},
	    {0x00004068, "XTmrCtr_SetOptions", 0},
	    {0x0000419c, "XTmrCtr_GetOptions", 0},
	    {0x000042cc, "XTmrCtr_LookupConfig", 0},
	    {0x000042f4, "outbyte", 0},
	    {0x00004320, "XUartLite_SendByte", 0},
	    {0x0000433c, "__init", 0},
	    {0x00004378, "__fini", 0},
	    {0x00004398, "__CTOR_LIST__", 0},
	    {0x00004398, "__CTOR_LIST__", 0},
	    {0x00004398, "___CTORS_LIST___", 0},
	    {0x0000439c, "__CTOR_END__", 0},
	    {0x000043a0, "__CTOR_END__", 0},
	    {0x000043a0, "___CTORS_END___", 0},
	    {0x000043a0, "__DTOR_LIST__", 0},
	    {0x000043a0, "__DTOR_LIST__", 0},
	    {0x000043a0, "___DTORS_LIST___", 0},
	    {0x000043a4, "__DTOR_END__", 0},
	    {0x000043a8, "__rodata_start", 0},
	    {0x00004dd8, "__thenan_df", 0},
	    {0x00004dec, "__clz_tab", 0},
	    {0x00004eec, "_ctype_", 0},
	    {0x00004ff0, "_ctype_b", 0},
	    {0x00005170, "_global_impure_ptr", 0},
	    {0x00005178, "cosTable", 0},
	    {0x00007578, "sinTable", 0},
	    {0x00009ac8, "OptionsTable", 0},
	    {0x00009b00, "__data_start", 0},
	    {0x00009b00, "MB_ExceptionVectorTable", 0},
	    {0x00009b00, "__rodata_end", 0},
	    {0x00009b00, "__sbss2_end", 0},
	    {0x00009b00, "__sbss2_start", 0},
	    {0x00009b00, "_SDA2_BASE_", 0},
	    {0x00009b00, "__sdata2_end", 0},
	    {0x00009b00, "__sdata2_start", 0},
	    {0x00009b40, "__ctype_ptr__", 0},
	    {0x00009b44, "_impure_ptr", 0},
	    {0x00009b48, "impure_data", 0},
	    {0x00009c38, "T_Data", 0},
	    {0x0000a400, "MB_InterruptVectorTable", 0},
	    {0x0000a408, "Xil_AssertWait", 0},
	    {0x0000a40c, "XTmrCtr_Offsets", 0},
	    {0x0000a410, "XIntc_ConfigTable", 0},
	    {0x0000a450, "XTmrCtr_ConfigTable", 0},
	    {0x0000a45c, "__data_end", 0},
	    {0x0000a45c, "__TMC_END__", 0},
	    {0x0000a460, "__bss_start", 0},
	    {0x0000a460, "completed.5083", 0},
	    {0x0000a460, "__sbss_end", 0},
	    {0x0000a460, "__sbss_start", 0},
	    {0x0000a460, "_SDA_BASE_", 0},
	    {0x0000a460, "__sdata_end", 0},
	    {0x0000a460, "__sdata_start", 0},
	    {0x0000a460, "__tbss_end", 0},
	    {0x0000a460, "__tbss_start", 0},
	    {0x0000a460, "__tdata_end", 0},
	    {0x0000a460, "__tdata_start", 0},
	    {0x0000a464, "dtor_idx.5085", 0},
	    {0x0000a468, "object.5095", 0},
	    {0x0000a480, "new_", 0},
	    {0x0000ac80, "new_im", 0},
	    {0x0000b480, "totalCount", 0},
	    {0x0000b484, "q", 0},
	    {0x0000b884, "w", 0},
	    {0x0000bc84, "Xil_AssertCallbackRoutine", 0},
	    {0x0000bc88, "timer", 0},
	    {0x0000bcb0, "per_intc", 0},
	    {0x0000bcc4, "Xil_AssertStatus", 0},
	    {0x0000bcc8, "XIntc_BitPosMask", 0},
	    {0x0000bd48, "__bss_end", 0},
	    {0x0000bd48, "_heap", 0},
	    {0x0000bd48, "_heap_start", 0},
	    {0x0000c148, "_heap_end", 0},
	    {0x0000c148, "_stack_end", 0},
	    {0x0000c548, "_end", 0},
	    {0x0000c548, "_stack", 0},
	    {0x0000c548, "__stack", 0}
	};






#endif /* SRC_TIMING_H_ */
