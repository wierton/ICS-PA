#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "device/port-io.h"

make_helper(out_a2i_b)
{
	uint8_t no = instr_fetch(eip + 1, 1);
	pio_write(no, 1, cpu.al);
	print_asm("out %%al,0x%x", no);
	return 2;
}

make_helper(out_a2i_l)
{
	uint8_t no = instr_fetch(eip + 1, 1);
	pio_write(no, 4, cpu.eax);
	print_asm("out %%eax,0x%x", no);
	return 2;
}

make_helper(out_a2d_b)
{
	pio_write(cpu.dx, 1, cpu.al);
	print_asm("out %%al,(%%dx)");
	return 1;
}

make_helper(out_a2d_l)
{
	pio_write(cpu.dx, 4, cpu.eax);
	print_asm("out %%eax,(%%dx)");
	return 1;
}

