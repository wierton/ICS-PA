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

void ExecLog();
inline void stop_eip();

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg)
{
	assert(sreg >= 0 && sreg <6);
	swaddr_t base = cpu.gsreg[sreg].TI?cpu.LDTR.base:cpu.GDTR.base;
	SegDesc TargetSegDesc;
	uint32_t sel = cpu.gsreg[sreg].INDEX;
	uint32_t *p = (uint32_t *)&TargetSegDesc;

	if(!cpu.CR0.protect_enable || cpu.gsreg[sreg].val == 0x0)
		return addr;

	assert(sel <= (cpu.gsreg[sreg].TI?cpu.LDTR.limit:cpu.GDTR.limit));
	*p = hwaddr_read(base + 8*sel, 4);
	*(p+1) = hwaddr_read(base + 4 + 8*sel, 4);

	uint32_t base_15_0 = TargetSegDesc.base_15_0;
	uint32_t base_23_16 = TargetSegDesc.base_23_16;
	uint32_t base_31_24 = TargetSegDesc.base_31_24;

	/* judge if has been load in sreg*/
	assert(TargetSegDesc.present);

	/* judge if exceed the limit */	

	/* Is the operation legal? */
	assert(cpu.gsreg[sreg].RPL <= TargetSegDesc.privilege_level);

	uint32_t sreg_base = ((base_15_0 | (base_23_16 << 16)) | (base_31_24 << 24));

	return addr + sreg_base;
}

hwaddr_t page_translate(lnaddr_t addr)
{
	if(!cpu.CR0.protect_enable || !cpu.CR0.paging)
		return addr;
	PageAddr pageaddr;
	pageaddr.val = addr;

	/* read page dir */
	PDE pdir;
	pdir.val = hwaddr_read((cpu.CR3.page_directory_base << 12) + pageaddr.pagedir * 4, 4);

	if(!pdir.present)
	{
		ExecLog();
		printf("pdir:0x%x\n", pdir.val);
		return 0x2000000;
	}
	assert(pdir.present);

	PTE ptable;
	ptable.val = hwaddr_read((pdir.page_frame << 12) + pageaddr.pagetab * 4, 4);
	assert(ptable.present);

	/* calc physic address */
	return (ptable.page_frame << 12) + pageaddr.off;
}


