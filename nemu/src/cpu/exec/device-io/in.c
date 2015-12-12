#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "device/port-io.h"

make_helper(in_i2a_b)
{
	uint8_t no = instr_fetch(eip + 1, 1);
	uint8_t data = pio_read(no, 1);
	cpu.al = data;
	print_asm("in 0x%x,%%al", no);
	return 2;
}

make_helper(in_i2a_l)
{
	uint8_t no = instr_fetch(eip + 1, 1);
	uint32_t data = pio_read(no, 4);
	cpu.eax = data;
	print_asm("in 0x%x,%%eax", no);
	return 2;
}

make_helper(in_d2a_b)
{
	uint32_t data = pio_read((cpu.edx & 0xffff), 4);
	cpu.al = data;
	print_asm("in (%%dx),%%al");
	return 1;
}

make_helper(in_d2a_l)
{
	uint32_t data = pio_read((cpu.edx & 0xffff), 4);
	cpu.eax = data;
	print_asm("in (%%dx),%%eax");
	return 1;
}

