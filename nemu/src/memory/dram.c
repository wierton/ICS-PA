#include "common.h"
#include "burst.h"
#include "misc.h"

/* Simulate the (main) behavor of DRAM.
 * Although this will lower the performace of NEMU, it makes
 * you clear about how DRAM perform read/write operations.
 * Note that cross addressing is not simulated.
 */

#define COL_WIDTH 10
#define ROW_WIDTH 10
#define BANK_WIDTH 3
#define RANK_WIDTH (27 - COL_WIDTH - ROW_WIDTH - BANK_WIDTH)

typedef union {
	struct {
		uint32_t col	: COL_WIDTH;
		uint32_t row	: ROW_WIDTH;
		uint32_t bank	: BANK_WIDTH;
		uint32_t rank	: RANK_WIDTH;
	};
	uint32_t addr;
} dram_addr;


#define NR_COL (1 << COL_WIDTH)
#define NR_ROW (1 << ROW_WIDTH)
#define NR_BANK (1 << BANK_WIDTH)
#define NR_RANK (1 << RANK_WIDTH)

#define HW_MEM_SIZE (1 << (COL_WIDTH + ROW_WIDTH + BANK_WIDTH + RANK_WIDTH))

uint8_t dram[HW_MEM_SIZE];

uint8_t *hw_mem = (void *)dram;

uint32_t dram_read(hwaddr_t addr, size_t len) {
	Assert(addr + 4 < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	uint8_t* temp = dram + addr;

	return unalign_rw(temp, 4);
}

void dram_write(hwaddr_t addr, size_t len, uint32_t data) {
	Assert(addr + 4 < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	void* temp = dram + addr;
	switch(len)
	{
		case 1:((uint8_t*)temp)[0] = data;break;
		case 2:((uint16_t*)temp)[0] = data;break;
		case 4:((uint32_t*)temp)[0] = data;break;
		default:assert(0);
	}
}
