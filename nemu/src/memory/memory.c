#include "common.h"
#include "device/mmio.h"

/* Define if necessary */
/* #define DEBUG_CACHE_READ */
/* #define DEBUG_CACHE_WRITE */
/* #define DEBUG_CACHE2_READ */
/* #define DEBUG_CACHE2_WRITE */
/* #define DEBUG_CACHE_TIME_CALC */

#ifdef DEBUG_CACHE_TIME_CALC
extern long long memory_access_time;
#endif

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

uint32_t cache_read(hwaddr_t, size_t);
void cache_write(hwaddr_t, size_t, uint32_t);

uint32_t cache2_read(hwaddr_t, size_t);
void cache2_write(hwaddr_t, size_t, uint32_t);

/* segment translate function declration*/
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg);

/* page translate function declration*/
hwaddr_t page_translate(lnaddr_t addr);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	int no = is_mmio(addr);
	if(no != -1)
	{
		return mmio_read(addr, len, no) & (~0u >> ((4 - len) << 3));
	}
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	int no = is_mmio(addr);
	if(no != -1)
	{
		mmio_write(addr, len, data, no);
	}
	else
	{
		dram_write(addr, len, data);
	}
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	uint32_t addr_0_11 = addr & 0xfff;
	hwaddr_t hwaddr = page_translate(addr);
	if(addr_0_11 + len > 0x1000)
	{
		/* data cross the page boundary */
		uint32_t ret;
		CrossData first, second;
		first.val = hwaddr_read(hwaddr, 4);
		second.val = hwaddr_read(((hwaddr + 0xfff)& ~0xfff), 4);
		switch(0x1000 - addr_0_11)
		{
			case 1:ret = first.data_0_7 | (second.data_0_23 << 8);break;
			case 2:ret = first.data_0_15 | (second.data_0_15 << 16);break;
			case 3:ret = first.data_0_23 | (second.data_0_7 << 24);break;
			default:
				   printf("crossaddr:0x%x,%d\n", addr_0_11, len);
				   assert(0);
				   break;
		}
		return ret;
	}

	return hwaddr_read(hwaddr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
	uint32_t addr_0_11 = addr & 0xfff;
	hwaddr_t hwaddr = page_translate(addr);
	if(addr_0_11 + len > 0x1000)
	{
		/* data cross the page boundary */
		CrossData cross;
		cross.val = data;
		hwaddr_t upaddr = ((hwaddr + 0xfff)& ~0xfff);
		switch(0x1000 - addr_0_11)
		{
			case 1:
				hwaddr_write(hwaddr, 1, cross.data_0_7);
				hwaddr_write(upaddr, 1, cross.val_8_15);
				hwaddr_write(upaddr, 2, cross.data_16_31);
				break;
			case 2:
				hwaddr_write(hwaddr, 2, cross.data_0_15);
				hwaddr_write(upaddr, 2, cross.data_16_31);
				break;
			case 3:
				hwaddr_write(hwaddr, 2, cross.data_0_15);
				hwaddr_write(hwaddr, 1, cross.val_16_23);
				hwaddr_write(upaddr, 1, cross.data_24_31);
				break;
			default:
				   printf("crossaddr:0x%x,%d\n", addr_0_11, len);
				   assert(0);
				   break;
		}
	}
	
	hwaddr_write(hwaddr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

