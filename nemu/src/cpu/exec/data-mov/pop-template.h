#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	DATA_TYPE src_val = MEM_R(cpu.esp, R_SS);
	OPERAND_W(op_src, src_val, R_SS);
	cpu.esp += DATA_BYTE;
	print_asm_template1();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm)
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
