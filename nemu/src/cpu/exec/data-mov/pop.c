#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "pop-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "pop-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(pop_rm)
make_helper_v(pop_r)

make_helper(popa)
{
	const uint32_t data_byte = 4;
	cpu.edi = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.esi = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.ebp = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	          swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.ebx = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.edx = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.ecx = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;
	cpu.eax = swaddr_read(cpu.esp, data_byte, R_SS);
	cpu.esp += data_byte;

	print_asm("popa");
	return 1;
}
