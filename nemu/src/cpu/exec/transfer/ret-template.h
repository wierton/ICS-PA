#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 4

int find_func_addr(swaddr_t func_addr);

make_helper(ret)
{
	cpu.eip = MEM_R(cpu.esp, R_SS);
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	find_func_addr(cpu.eip);
	return 1;
}

make_helper(ret_w)
{
	int len = decode_i_w(eip + 1);
	cpu.eip = MEM_R(cpu.esp, R_SS);
	find_func_addr(cpu.eip);
	cpu.eip -= len;
	cpu.esp += DATA_BYTE;
	cpu.esp += op_src->val;
	print_asm("ret $0x%x",op_src->val);
	return len + 1;
}

make_helper(iret)
{
	const uint32_t data_byte = 4;
	cpu.eip = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.CS.val = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.EFLAGS = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;

	print_asm("iret 0x%x", cpu.eip);
	find_func_addr(cpu.eip);
	cpu.eip -= 1;
	cpu.IF = 1;
	return 1;
}
#endif

#include "cpu/exec/template-end.h"
