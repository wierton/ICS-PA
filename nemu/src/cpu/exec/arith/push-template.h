#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	cpu.esp = cpu.esp - DATA_BYTE;
	MEM_W(cpu.esp, op_src->val);
	print_asm_template1();
}

make_helper(concat(push_m_,SUFFIX))
{
	swaddr_t addr = instr_fetch(eip + 1, 4);
	cpu.esp = cpu.esp - DATA_BYTE;
	MEM_W(cpu.esp, op_src->val);
	print_asm("push $0x%x",addr);
	return 5;
}

make_instr_helper(i)

#if DATA_BYTE != 1
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
