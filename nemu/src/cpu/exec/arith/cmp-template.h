#include "cpu/exec/template-start.h"
#include "arith-bits.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE dst = op_dest->val - op_src->val;
	printf("%d,%d\n",op_dest->val,op_src->val);
	cpu.OF = subOK(op_src->val, op_dest->val);
	cpu.SF = (dst)>>31;
	cpu.ZF = (dst == 0);
	cpu.PF = anyEvenBit(dst);
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
