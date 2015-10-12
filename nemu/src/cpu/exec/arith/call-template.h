#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX))
{
	int len = concat(decode_i_, SUFFIX)(cpu.eip+1);
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, cpu.eip+len);
	cpu.eip += op_src->val;
	print_asm("call $0x%x",cpu.eip+len+1);
	return len+1;
}


#include "cpu/exec/template-end.h"
