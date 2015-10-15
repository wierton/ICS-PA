#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat(stos_,SUFFIX))
{
	DATA_TYPE result = cpu.eax;
	MEM_W(cpu.edi,result);
	print_asm("stos $al,$es:($edi)");
	cpu.edi++;
	return 1;
}


#include "cpu/exec/template-end.h"
