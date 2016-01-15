#include "monitor/monitor.h"
#include "cpu/helper.h"
#include "monitor/watchpoint.h"
#include <setjmp.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the ``si'' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 1000000

swaddr_t stop_eip = 0;
int nemu_state = STOP;

int exec(swaddr_t);

char assembly[80];
char asm_buf[128];

inline void func_cost_calc();

/* Used with exception handling. */
jmp_buf jbuf;

/* some function declared */
lnaddr_t seg_translate(swaddr_t, size_t, uint8_t);
uint8_t i8259_query_intr();
void i8259_ack_intr();

void print_bin_instr(swaddr_t eip, int len) {
	int i;
	int l = sprintf(asm_buf, "%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		l += sprintf(asm_buf + l, "%02x ", instr_fetch(eip + i, 1));
	}
	sprintf(asm_buf + l, "%*.s", 50 - (12 + 3 * len), "");
}

/* This function will be called when an `int3' instruction is being executed. */
void do_int3() {
	printf("\nHit breakpoint at eip = 0x%08x\n", cpu.eip);
	nemu_state = STOP;
}

void raise_intr(uint8_t no)
{
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */

	/* close intr */
	cpu.IF = 0;

	GateDesc gd;
	const uint32_t data_byte = 4;
	uint32_t *p = (uint32_t *)&gd;

	/* push EFLAGS, CS, eip into stack */
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.EFLAGS, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.CS.val, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.eip, R_SS);

	*p = lnaddr_read(cpu.IDTR.base + no * 8, 4);
	*(p+1) = lnaddr_read(cpu.IDTR.base + no * 8 + 4, 4);

	/* fill CS */
	cpu.CS.val = gd.segment;

	/* calc the base addr */
	lnaddr_t base = seg_translate(0, 4, R_CS);

	/* get the entry addr */
	uint32_t EntryAddr = base + ((gd.offset_31_16 << 16) | gd.offset_15_0);

	cpu.eip = EntryAddr;

	cpu.IF = 1;

	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}

/* Simulate how the CPU works. */
void cpu_exec(volatile uint32_t n) {
	if(nemu_state == END) {
		printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
		return;
	}
	nemu_state = RUNNING;

#ifdef DEBUG
	volatile uint32_t n_temp = n;
#endif

	setjmp(jbuf);

	for(; n > 0; n --) {
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			fputc('.', stderr);
		}
#endif

		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
		int instr_len = exec(cpu.eip);

		if((n & 0xfffff) == 0) {fputc('.', stderr);}
		cpu.eip += instr_len;
		func_cost_calc();

		if(cpu.eip == stop_eip)
			nemu_state = STOP;

#ifdef DEBUG
		print_bin_instr(eip_temp, instr_len);
		
		strcat(asm_buf, assembly);
		Log_write("%s\n", asm_buf);

		if(n_temp < MAX_INSTR_TO_PRINT) {
			printf("%s\n", asm_buf);
		}
#endif

		/* TODO: check watchpoints here. */
		if(!check_wp()) {nemu_state = STOP;}

		/* TODO: check intr */
		if(cpu.INTR & cpu.IF) {
			uint32_t intr_no = i8259_query_intr();
			i8259_ack_intr();
			raise_intr(intr_no);
		}

		if(nemu_state != RUNNING) { return; }
	}

	if(nemu_state == RUNNING) { nemu_state = STOP; }
}

inline void stop_nemu()
{
	nemu_state = STOP;
}

