#include "cpu/exec/template-start.h"


#define instr cmp

static void do_execute() {
	DATA_TYPE_S dst = op_dest->val;
	DATA_TYPE_S src = op_src->val;
	DATA_TYPE result = dst - src;
	cpu.OF = !subOK(dst, src);
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = (dst < src);
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
