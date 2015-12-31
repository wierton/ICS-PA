#include "cpu/exec/helper.h"

#define DATA_BYTE 4
#include "ret-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */
void stop_nemu();
make_helper(iret)
{
	stop_nemu();
	const uint32_t data_byte = 4;
	cpu.eip = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.CS.val = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.EFLAGS = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;

	print_asm("iret 0x%x", cpu.eip);
	printf("iret 0x%x", cpu.eip);
	stop_nemu();
	cpu.eip -= 1;
	cpu.IF = 1;
	return 1;
}
