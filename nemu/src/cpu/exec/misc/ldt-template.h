#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

#if DATA_BYTE == 4

#define instr lgdt

make_helper(lgdt_l)
{
	ModR_M m;
	int ret;
	swaddr_t addr;
	m.val = instr_fetch(eip + 1, 1);
	if(m.mod == 3)
	{
		addr = reg_l(m.R_M);
		ret = 2;
		print_asm("lgdtl %%%s", regsb[m.R_M]);
	}
	else
	{
		addr = swaddr_read(eip + 2, 4, R_DS);
		ret = 6;
		print_asm("lgdtl 0x%x", addr);
	}
	printf("%x\n", addr);
	cpu.GDTR.limit = swaddr_read(addr, 2, R_DS);
	cpu.GDTR.base = swaddr_read(addr + 2, 4, R_DS);
	return ret;
}

#undef instr

#define instr lidt

make_helper(lidt_l)
{
	int len = decode_rm_l(eip + 1);
	cpu.LDTR.limit = swaddr_read(op_src->val, 2, R_DS);
	cpu.LDTR.base = swaddr_read(op_src->val + 2, 4, R_DS);
	return len + 1;
}

#endif

#include "cpu/exec/template-end.h"
