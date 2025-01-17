#include "cpu/exec/template-start.h"

#define instr xor

static void do_execute () {
	DATA_TYPE result = op_dest->val ^ op_src->val;
	OPERAND_W(op_dest, result, R_DS);

	/* TODO: Update EFLAGS. */
	int dst = op_dest->val ^ op_src->val;
	cpu.OF = 0;
	cpu.SF = (dst)>>31;
	cpu.ZF = (dst == 0);
	cpu.PF = anyEvenBit(dst);
	cpu.CF = 0;

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
