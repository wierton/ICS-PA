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

#if DATA_BYTE == 4

make_helper(call_rm_l)
{
	int len = concat(decode_rm_, SUFFIX)(eip+1);
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, cpu.eip+len);
	cpu.eip = op_src->val - (len+1);
	print_asm("call $0x%x",cpu.eip+len+1);
	return len+1;
}

#endif

#include "cpu/exec/template-end.h"
