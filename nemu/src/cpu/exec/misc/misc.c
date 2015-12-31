#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

void raise_intr(uint8_t);
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg);

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(int_i_b)
{
	cpu.IF = 0;

	GateDesc gd;
	int len = 2;
	const uint32_t data_byte = 4;
	uint32_t *p = (uint32_t *)&gd;
	uint8_t no = instr_fetch(eip + 1, 1);
	
	/* push EFLAGS, CS, eip into stack */
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.EFLAGS, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.CS.val, R_SS);
	cpu.esp -= data_byte;
	swaddr_write(cpu.esp, data_byte, cpu.eip + len, R_SS);

	*p = swaddr_read(cpu.IDTR.base + no * 8, 4, R_DS);
	*(p+1) = swaddr_read(cpu.IDTR.base + no * 8 + 4, 4, R_DS);

	/* fill CS */
	cpu.CS.val = gd.segment;
	
	/* calc the base addr */
	lnaddr_t base = seg_translate(0, 4, R_CS);

	/* get the entry addr */
	uint32_t EntryAddr = base + ((gd.offset_31_16 << 16) | gd.offset_15_0);

	cpu.eip = EntryAddr - len;
	printf("int $0x%x", no);
	print_asm("int $0x%x", no);
	return len;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(cdq)
{
	int32_t s = cpu.eax>>31;
	if(s)
		cpu.edx = 0xffffffff;
	else
		cpu.edx = 0x0;
	print_asm("cltd");
	return 1;
}

make_helper(fldz)
{
	return 2;
}

make_helper(fstpl)
{
	return 6;
}
