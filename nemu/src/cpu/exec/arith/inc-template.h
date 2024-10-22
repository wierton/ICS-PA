#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;
	OPERAND_W(op_src, result, R_DS);

	/* TODO: Update EFLAGS. */
	int dst = op_src->val + 1;
	cpu.OF = !addOK(op_src->val, 1);
	cpu.SF = (dst)>>31;
	cpu.ZF = (dst == 0);
	cpu.PF = anyEvenBit(dst);
	cpu.CF = !addOK(op_src->val, 1);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
