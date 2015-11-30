#include "common.h"
#include "cpu/decode/decode.h"

/* shared by all helper function */
Operands ops_decoded;

#define DATA_BYTE 1
#include "decode-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "decode-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "decode-template.h"
#undef DATA_BYTE

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg)
{
	assert(sreg >= 0 && sreg <6);
	swaddr_t base = cpu.gsreg[sreg].TI?cpu.LDTR.base:cpu.GDTR.base;
	SegDesc TargetSegDesc;
	lnaddr_t ret_addr;
	uint32_t sel = cpu.gsreg[sreg].INDEX;
	uint32_t *p = (uint32_t *)&TargetSegDesc;

	assert(sel <= cpu.gsreg[sreg].TI?cpu.LDTR.limit:cpu.GDTR.limit);
	*p = lnaddr_read(base + 8*sel, 4);
	*(p+1) = lnaddr_read(base + 4 + 8*sel, 4);

	uint32_t base_15_0 = TargetSegDesc.base_15_0;
	uint32_t base_23_16 = TargetSegDesc.base_23_16;
	uint32_t base_31_24 = TargetSegDesc.base_31_24;

	/* judge if exceed the limit */
	printf("base:%x\n", base);

	/* Is the operation legal? */
	assert(cpu.gsreg[sreg].RPL <= TargetSegDesc.privilege_level);

	uint32_t sreg_base = ((base_15_0 | (base_23_16 << 16)) | (base_31_24 << 24));
	/* judge if protect enable */
	if(cpu.CR0.protect_enable)
		ret_addr = addr + sreg_base;
	else
		ret_addr = addr;
	return ret_addr;
}
