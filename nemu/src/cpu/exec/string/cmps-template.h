#include "cpu/exec/template-start.h"

#define instr cmps

make_helper(concat(cmps_,SUFFIX))
{
	DATA_TYPE dst = MEM_R(cpu.edi);
	DATA_TYPE src = MEM_R(cpu.esi);
	int32_t result = dst - src;
	cpu.OF = !subOK(dst, src);
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = anyEvenBit(result);
	cpu.CF = (dst < src);
	cpu.edi += DATA_BYTE;
	cpu.esi += DATA_BYTE;

	print_asm("cmpsb $es:($edi),$ds:($esi)");
	return 1;
}

#include "cpu/exec/template-end.h"
