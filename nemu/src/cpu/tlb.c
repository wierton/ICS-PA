#include "common.h"
#include "misc.h"

#include "memory/memory.h"

#define NUM_WIDTH 8
#define TLB_NUM (1 << NUM_WIDTH)

typedef struct {
	bool valid;
	uint32_t tag:20;
	uint32_t page_frame:20;
} TLB;

typedef union {
	struct {
		uint32_t dont_care	:12;
		uint32_t index		:8;
		uint32_t offset		:12;
	};
	uint32_t val;
} TLB_ADDR;

TLB tlbbufs[TLB_NUM];

void init_tlb() {
	int i;
	for(i = 0; i < TLB_NUM; i ++) {
		tlbbufs[i].valid = false;
	}
}

bool tlb_read(uint32_t addr, uint32_t *page_frame)
{
	TLB_ADDR tlbaddr;
	tlbaddr.val = addr;
	if(tlbbufs[tlbaddr.index].valid)
	{
		*page_frame = tlbbufs[tlbaddr.index].page_frame;
		return true;
	}
	return false;
}

inline void write_tlb(uint32_t index, uint32_t page_frame)
{
	tlbbufs[index].valid = true;
	tlbbufs[index].page_frame = page_frame;
}
