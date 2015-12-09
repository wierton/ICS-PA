#include "cpu/helper.h"
#include "cpu/decode/modrm.h"
#include "all-instr.h"

void ExecAssert(bool cond)
{
	if(!cond)
	{
		fprintf(stderr, "\33[1;31m");
		fprintf(stderr, "cond not satisfied!\n");
		fprintf(stderr, "data recorded:eip\t0x%x\n", cpu.eip);
		fprintf(stderr, "\33[0m\n");
	}
}
