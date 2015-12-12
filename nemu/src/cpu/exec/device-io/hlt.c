#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(hlt)
{
	if(!cpu.INTR)
		return 1;
	else
		return 1;
}
