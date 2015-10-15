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

make_helper(concat(ret_,SUFFIX))
{
	cpu.eip = MEM_R(cpu.esp);
	int len = concat(decode_i_,SUFFIX)(eip + 1);
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	return len + 1;
}

#include "cpu/exec/template-end.h"
