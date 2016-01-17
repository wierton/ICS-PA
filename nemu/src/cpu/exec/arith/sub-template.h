#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE dst = op_dest->val;
	DATA_TYPE src = op_src->val;
	DATA_TYPE result = dst - src;
	cpu.OF = !subOK(op_dest->val, op_src->val);
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = (dst < src);
	OPERAND_W(op_dest, op_dest->val-op_src->val, R_DS);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#if DATA_BYTE != 1
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"

