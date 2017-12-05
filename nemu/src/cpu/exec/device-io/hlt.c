#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(hlt)
{
	print_asm("hlt");
	if(!cpu.INTR)
		return 0;
	else
		return 1;
}
