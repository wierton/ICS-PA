#include "cpu/exec/template-start.h"
#include "arith-bits.h"

#define instr sub

#ifndef __SUB_TEMPLATE_H__
#define __SUB_TEMPLATE_H__

int subOK(int x, int y) {
	int a=x;
	int b=~y+1;
	int sum=a+b;
	int p=(~x&y&sum)|(x&~y&~sum);
	return ((p>>31)&0x1);
}

#endif

static void do_execute() {
	OPERAND_W(op_dest, op_src->val-op_dest->val);
	cpu.OF = subOK(op_src->val, op_dest->val);
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

//#if SUFFIX != b
make_helper(concat(sub_si2rm_, SUFFIX)) {
	return 5;
}
//#endif
#include "cpu/exec/template-end.h"

