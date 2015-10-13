#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, op_src->val);
	print_asm_template1();
}

make_instr_helper(i)

#if DATA_BYTE == 4
make_helper(push_i_b)
{
	int len = decode_i_b(eip+1);
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, op_src->val);
	print_asm("push"str(SUFFIX)" $0x%x",op_src->val);
	return len+1;
}
#endif

#if DATA_BYTE != 1
make_instr_helper(rm_imm)
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
