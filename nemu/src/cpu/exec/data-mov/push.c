#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4 
#include "push-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(push_rm)
make_helper_v(push_r)
make_helper_v(push_i)

make_helper(pusha)
{
	uint32_t temp = cpu.esp;
	const uint32_t data_byte = 4;

	printf("edx:%x\necx:%x\n",cpu.edx,cpu.ecx);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.eax, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.ecx, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.edx, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.ebx, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, temp, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.ebp, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.esi, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.edi, R_SS);

	print_asm("pusha");

	return 1;
}




