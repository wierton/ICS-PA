#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(hlt)
{
	while(!cpu.INTR);
	return 1;
}
