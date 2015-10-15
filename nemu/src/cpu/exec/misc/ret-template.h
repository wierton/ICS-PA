#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 4
make_helper(ret)
{
	cpu.eip = MEM_R(cpu.esp);
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	return 1;
}
#endif

#if DATA_BYTE == 2
make_helper(ret_l)
{
	cpu.eip = MEM_R(cpu.esp);
	int len = decode_i_l(eip + 1);
	cpu.esp += op_src->val;
	print_asm("ret $0x%x",op_src->val);
	return len + 1;
}
#endif

#include "cpu/exec/template-end.h"
