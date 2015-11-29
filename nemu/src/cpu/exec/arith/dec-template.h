#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE result = op_src->val - 1;
	OPERAND_W(op_src, result, R_DS);

	/* TODO: Update EFLAGS. */
	cpu.OF = !subOK(op_src->val, 1);
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = (op_src->val < 1);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
