#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, op_src->val, R_DS);
	print_asm_template1();
}

make_instr_helper(i)

#if DATA_BYTE == 4
make_helper(push_i_b)
{
	int len = decode_i_b(eip+1);
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, op_src->val, R_DS);
	print_asm_template1();
	return len+1;
}
#endif

#if DATA_BYTE != 1
make_instr_helper(rm)
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
