#include "cpu/exec/template-start.h"

#define instr bt

static void do_execute() {
	DATA_TYPE dst = op_dest->val;
	DATA_TYPE src = op_src->val;
	assert(src < DATA_BYTE);
	cpu.CF = ((dst >> src) & 0x1);
	print_asm_template2();
}

#if DATA_BYTE != 1
make_instr_helper(i2rm)
make_instr_helper(r2rm)
#endif

#include "cpu/exec/template-end.h"
