#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat(movs_,SUFFIX))
{
	DATA_TYPE src = MEM_R(cpu.esi);
	MEM_W(cpu.edi,src);
	cpu.edi += DATA_BYTE;
	cpu.esi += DATA_BYTE;
#if DATA_BYTE == 1
	printf("pause here!\n");;
#endif

	print_asm("movs"str(SUFFIX)" $es:($esi),$ds:($edi)");
	return 1;
}

#include "cpu/exec/template-end.h"
