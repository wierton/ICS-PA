#include <stdlib.h>

#include "common.h"
#include "misc.h"

#include "memory/memory.h"

#define NUM_WIDTH 6
#define TLB_NUM (1 << NUM_WIDTH)

typedef struct {
	bool valid;
	uint32_t tag:20;
	uint32_t page_frame:20;
} TLB;

typedef union {
	struct {
		uint32_t offset     :12;
		uint32_t tag		:20;
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
}

void tlb_write(uint32_t addr, uint32_t page_frame)
{
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
}
