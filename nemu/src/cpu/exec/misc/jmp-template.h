#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	cpu.eip += op_src->val;
	print_asm_template1();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(jmp_rm_, SUFFIX))
{
	int len = concat(decode_rm_,SUFFIX)(eip+1);
	cpu.eip = op_src->val - (len+1);
	print_asm_template1();
	return len+1;

}
#endif
make_instr_helper(i)

#include "cpu/exec/template-end.h"
