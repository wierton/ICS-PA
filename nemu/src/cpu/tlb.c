#include <stdlib.h>

#include "common.h"
#include "misc.h"

#include "memory/memory.h"

#define PA3

#ifdef PA3
	#define NUM_WIDTH 6
#else
	#define NUM_WIDTH 8
#endif
#define TLB_NUM (1 << NUM_WIDTH)

#define INDEX_MASK (((1 << NUM_WIDTH) - 1) << 12)
#define INDEX_BIT 12

typedef struct {
	bool valid;
	uint32_t tag:20;
	uint32_t page_frame:20;
} TLB;

typedef union {
	struct {
		uint32_t offset	:12;
		uint32_t tag	:20;
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
#ifdef PA3
	int i;
	TLB_ADDR tlbaddr;
	tlbaddr.val = addr;
	for(i = 0;i < TLB_NUM; i ++)
		if(tlbbufs[i].valid && tlbbufs[i].tag == tlbaddr.tag)
		{
			*page_frame = tlbbufs[i].page_frame;
			return true;
		}
	return false;
#else
	TLB_ADDR tlbaddr;
	uint32_t index = ((addr & INDEX_MASK) >> INDEX_BIT);
	tlbaddr.val = addr;
	if(tlbbufs[index].valid && tlbbufs[index].tag == tlbaddr.tag)
	{
		*page_frame = tlbbufs[index].page_frame;
		return true;
	}
	return false;
#endif
}

void tlb_write(uint32_t addr, uint32_t page_frame)
{
#ifdef PA3
	int i,tmp;
	TLB_ADDR tlbaddr;
	tlbaddr.val = addr;
	for(i = 0;i < TLB_NUM; i ++)
	{
		if(!tlbbufs[i].valid)
		{
			tlbbufs[i].valid = true;
			tlbbufs[i].tag = tlbaddr.tag;
			tlbbufs[i].page_frame = page_frame;
			return;
		}
	}
	tmp = rand()%TLB_NUM;
	tlbbufs[tmp].valid = true;
	tlbbufs[tmp].tag = tlbaddr.tag;
	tlbbufs[tmp].page_frame = page_frame;
#else
	TLB_ADDR tlbaddr;
	uint32_t index = ((addr & INDEX_MASK) >> INDEX_BIT);
	tlbaddr.val = addr;
	tlbbufs[index].valid = true;
	tlbbufs[index].tag = tlbaddr.tag;
	tlbbufs[index].page_frame = page_frame;
#endif
}
