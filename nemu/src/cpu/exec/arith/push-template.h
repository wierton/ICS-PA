#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	cpu.esp = cpu.esp - DATA_BYTE * 8;
	MEM_W(cpu.eip, op_src->val);
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
