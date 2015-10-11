#include "cpu/exec/template-start.h"

#define instr leave

make_helper(leave)
{
	cpu.esp = cpu.ebp;
	cpu.ebp = MEM_R(cpu.esp);
	cpu.esp += DATA_BYTE;
	return 1;
}

#include "cpu/exec/template-end.h"
