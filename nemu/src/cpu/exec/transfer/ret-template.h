#include "cpu/exec/template-start.h"

#define instr ret

#if DATA_BYTE == 4
make_helper(ret)
{
	cpu.eip = MEM_R(cpu.esp, R_SS);
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	return 1;
}

make_helper(ret_w)
{
	int len = decode_i_w(eip + 1);
	cpu.eip = MEM_R(cpu.esp, R_SS);
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

	cpu.eip -= 1;
	cpu.IF = 1;
	return 1;
}
#endif

#include "cpu/exec/template-end.h"
