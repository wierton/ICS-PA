#include "cpu/exec/template-start.h"
#include "arith-bits.h"

#define instr sub

static void do_execute() {
	OPERAND_W(op_dest, op_dest->val-op_src->val);
	cpu.OF = !subOK(op_dest->val, op_src->val);
	cpu.SF = (op_dest->val)>>31;
	cpu.ZF = (op_dest->val == 0);
	cpu.PF = anyEvenBit(op_dest->val);
	cpu.CF = (op_src->val < op_dest->val);
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

