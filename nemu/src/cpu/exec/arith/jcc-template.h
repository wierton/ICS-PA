#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute() {
	//printf("arith/jcc-template.h:%x,%x\n", op_dest->val, op_src->val);
	if(cpu.ZF == 1)
		cpu.eip = cpu.eip + op_dest->val;
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
