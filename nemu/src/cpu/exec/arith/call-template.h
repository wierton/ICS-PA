#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	op_dest->val = cpu.eip + op_src->val;
	cpu.eip = op_dest->val;
	print_asm_template1();
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
