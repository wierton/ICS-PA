#include "cpu/exec/template-start.h"

#define instr and

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
	OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
	cpu.OF = 0;
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = 0;

	print_asm_template2();
}

#if DATA_BYTE == 1
make_helper(and_i2a_b)
{
	int len = decode_i2a_b(eip + 1);
	int src_val = op_src->val;
	src_val = (((src_val)<<24)>>24);
	DATA_TYPE result = op_dest->val & src_val;
	printf("and_i2a_b:%x\n",src_val);
	OPERAND_W(op_dest, result);
	cpu.OF = 0;
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = 0;

	print_asm_template2();
	return len + 1;
}

make_helper(and_i2rm_b)
{
	int len = decode_i2rm_b(eip + 1);
	int src_val = op_src->val;
	src_val = (((src_val)<<24)>>24);
	DATA_TYPE result = op_dest->val & src_val;
	printf("and_i2rm_b:%x\n",src_val);
	OPERAND_W(op_dest, result);
	cpu.OF = 0;
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = 0;

	print_asm_template2();
	return len + 1;
}
#endif

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(si2rm)
#endif

make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
