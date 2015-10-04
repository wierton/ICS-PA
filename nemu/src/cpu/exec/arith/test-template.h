#include "cpu/exec/template-start.h"
#include "arith-bits.h"

#define instr test

static void do_execute() {
	DATA_TYPE dst = op_src->val & op_dest->val;
	cpu.OF = 0;
	cpu.SF = dst>>31;
	cpu.ZF = (dst == 0);
	cpu.PF = anyEvenBit(op_dest->val);
	cpu.CF = 0;
	printf("%x\n",cpu.ZF);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)


#include "cpu/exec/template-end.h"
