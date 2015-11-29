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
	uint32_t *p = (uint32_t *)&TargetSegDesc;
	*p = lnaddr_read(base, 4);
	*(p+1) = lnaddr_read(base + 32, 4);
	return 0;
}
