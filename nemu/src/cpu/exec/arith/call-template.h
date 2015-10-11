#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, cpu.eip);
	cpu.eip += op_src->val;
	print_asm("call 0x%08x",cpu.eip);
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
