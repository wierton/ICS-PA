nclude "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	OPERAND_W(op_dest, op_src->val-op_dest->val);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

