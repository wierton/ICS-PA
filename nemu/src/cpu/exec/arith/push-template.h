#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	cpu.esp = cpu.esp - DATA_BYTE;
	MEM_W(cpu.esp, op_src->val);
}

make_instr_helper(i)
make_instr_helper(r)

#include "cpu/exec/template-end.h"
