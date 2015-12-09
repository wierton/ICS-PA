#include "cpu/helper.h"
#include "cpu/decode/modrm.h"
#include "all-instr.h"

void ExecJudge(bool cond)
{
	if(!cond)
	{
		fprintf(stderr, "\33[1;31m");
		fprintf(stderr, "judge failed:eip\t0x%x\n", cpu.eip);
		fprintf(stderr, "\33[0m\n");
	}
}
