#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	DATA_TYPE dst_2 = op_dest->val + op_src->val;
	DATA_TYPE dst_3 = op_dest->val + op_src->val + cpu.CF;
	OPERAND_W(op_dest, op_dest->val + op_src->val +cpu.CF);
	int o2f = !addOK(op_dest->val, op_src->val);
	if (o2f || (dst_3 < dst_2))
		cpu.OF = 1;
	else
		cpu.OF = 0;
	cpu.SF = MSB(dst_3);
	cpu.ZF = (dst_3 == 0);
	cpu.PF = anyEvenBit(dst_3);
	cpu.CF = cpu.OF;
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
