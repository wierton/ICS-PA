#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat(stos_,SUFFIX))
{
	int IncDec = cpu.DF?-DATA_BYTE:DATA_BYTE;
	DATA_TYPE result = cpu.eax;
	MEM_W(cpu.edi, result, R_ES);
#if DATA_BYTE == 1
	print_asm("stos $al,$es:($edi)");
#endif
#if DATA_BYTE == 2
	print_asm("stos $ax,$es:($edi)");
#endif
#if DATA_BYTE == 4
	print_asm("stos $eax,$es:($edi)");
#endif
	cpu.edi += IncDec;
	return 1;
}


#include "cpu/exec/template-end.h"
