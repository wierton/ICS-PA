#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute() {
	printf("arith/jcc-template.h:%x\n", cpu.eip);
	cpu.eip = op_src->val;
	printf("arith/jcc-template.h:%x\n", cpu.eip);
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
