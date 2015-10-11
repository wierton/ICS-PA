#include "cpu/exec/template-start.h"

#define instr ret

make_helper(ret)
{
	cpu.eip = MEM_R(cpu.esp);
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	return 1;
}


#include "cpu/exec/template-end.h"
