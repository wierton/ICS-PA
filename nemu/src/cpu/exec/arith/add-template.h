#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
	DATA_TYPE dst = op_dest->val + op_src->val;
	cpu.OF = !addOK(op_dest->val, op_src->val);
	cpu.SF = MSB(dst);
	cpu.ZF = (dst == 0);
	cpu.PF = anyEvenBit(dst);
	cpu.CF = !addOK(op_dest->val, op_src->val);
	OPERAND_W(op_dest, op_dest->val+op_src->val);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);

#if DATA_BYTE != 1
make_instr_helper(si2rm);
#endif

#include "cpu/exec/template-end.h"
