#include "cpu/exec/helper.h"
#include "monitor/monitor.h"
#include "SDL/SDL.h"

inline void stop_nemu();
uint32_t swaddr_read(swaddr_t, size_t, uint8_t);

make_helper(cli)
{
	cpu.IF = 0;
	return 1;
}

make_helper(sti)
{
	cpu.IF = 1;
	return 1;
}

make_helper(print)
{
	int i;
	char str[201];
	for(i=0;i<200;i++)
	{
		str[i] = swaddr_read(cpu.eax + i, 1, R_DS);
		if(str[i] == 0)
			break;
	}
	str[i] = 0;
	printf("%s", str);
	return 2;
}

make_helper(inv) {
	/* invalid opcode */

	uint32_t temp[8];
	temp[0] = instr_fetch(eip, 4);
	temp[1] = instr_fetch(eip + 4, 4);

	uint8_t *p = (void *)temp;
	printf("invalid opcode(eip = 0x%08x): %02x %02x %02x %02x %02x %02x %02x %02x ...\n\n", 
			eip, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

	extern char logo [];
	printf("There are two cases which will trigger this unexpected exception:\n\
1. The instruction at eip = 0x%08x is not implemented.\n\
2. Something is implemented incorrectly.\n", eip);
	printf("Find this eip value(0x%08x) in the disassembling result to distinguish which case it is.\n\n", eip);
	printf("\33[1;31mIf it is the first case, see\n%s\nfor more details.\n\nIf it is the second case, remember:\n\
* The machine is always right!\n\
* Every line of untested code is always wrong!\33[0m\n\n", logo);

	assert(0);
}

make_helper(nemu_trap) {
	print_asm("nemu trap (eax = %d)", cpu.eax);
	int i;

	switch(cpu.eax) {
		case 2:
			for(i = 0; i < cpu.edx; i++)
			{
				uint8_t tmp = swaddr_read(cpu.ecx + i, 1, R_DS);
				printf("%c", (char)tmp);
			}
		   	break;

		default:
			printf("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
					(cpu.eax == 0 ? "GOOD" : "BAD"), cpu.eip);
			nemu_state = STOP;
			break;
	}

	return 1;
}

typedef struct {
	uint32_t sw,dw,sp,dp,sx,sy,dx,dy,cw,ch;
} ACC;

make_helper(nemu_acc)
{
	ACC ac;
	int i;
	uint32_t *p = (uint32_t *)(&ac);
	for(i = 0; i < 10; i++)
	{
		*(p + i) = swaddr_read(cpu.eax, 4, R_DS);
	}

	return 1;
}
