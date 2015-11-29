#include "cpu/exec/template-start.h"

#if DATA_BYTE == 4

#define instr lgdt

make_helper(lgdt_l)
{
	int len = decode_rm_l(eip + 1);
	cpu.GDTR.limit = swaddr_read(op_src->val, 2, R_DS);
	cpu.GDTR.base = swaddr_read(op_src->val + 2, 4, R_DS);
	print_asm_template1();
	return len + 1;
}

#undef instr

#define instr lidt

make_helper(lidt_l)
{
	int len = decode_rm_l(eip + 1);
	cpu.LDTR.limit = swaddr_read(op_src->val, 2, R_DS);
	cpu.LDTR.base = swaddr_read(op_src->val + 2, 4, R_DS);
	return len + 1;
}

#endif

#include "cpu/exec/template-end.h"
