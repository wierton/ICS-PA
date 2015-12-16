#include <stdlib.h>
#include "common.h"
#include "misc.h"

#include "memory/memory.h"

/* define if necessary */
/* #define DEBUG_CACHE_READ */
/* #define DEBUG_CACHE_WRITE */
/* #define DEBUG_CACHE_TIME_CALC */

#define INADDR_WIDTH 6
#define SETNUM_WIDTH 7
#define MEMMARK_WIDTH (32 - SETNUM_WIDTH - INADDR_WIDTH)

#ifdef DEBUG_CACHE_TIME_CALC
long long memory_access_time;
#endif

typedef union {
	struct {
		uint32_t inaddr		: INADDR_WIDTH;
		uint32_t setnum		: SETNUM_WIDTH;
		uint32_t memmark	: MEMMARK_WIDTH;
	};
	uint32_t addr;
} cache_addr;

#define NR_BLOCKSIZE (1 << INADDR_WIDTH)
#define NR_SETNUM (1 << SETNUM_WIDTH)
#define NR_INSETNUM 8

#define CACHE_MASK (NR_BLOCKSIZE - 1)

typedef struct {
	bool valid;
	uint32_t memmark;
	uint8_t buf[NR_BLOCKSIZE];
} CCL;

CCL cachebufs[NR_SETNUM][NR_INSETNUM];

/* declaration of dram_read and dram_write */
#if defined(DEBUG_CACHE_READ) || defined(DEBUG_CACHE_WRITE)
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
#endif
uint32_t cache2_read(hwaddr_t, size_t);
void cache2_write(hwaddr_t, size_t, uint32_t);


void init_cache() {
	int i, j;
#ifdef DEBUG_CACHE_TIME_CALC
	memory_access_time = 0;
#endif
	for(i = 0; i < NR_SETNUM; i ++) {
		for(j = 0; j < NR_INSETNUM; j++) {
			cachebufs[i][j].valid = false;
		}
	}
}

void print_cache_info_by_addr(swaddr_t addr)
{
	cache_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	uint32_t inaddr = temp.inaddr;
	
	int i;
	for(i = 0; i < NR_INSETNUM; i ++)
	{
		if(cachebufs[setnum][i].valid && cachebufs[setnum][i].memmark == memmark)
		{
			printf("found in 0x%xth block in 0x%xth set.\n", i, setnum);
			printf("memmark:0x%x\t", memmark);
			printf("setnum:0x%x\t", setnum);
			printf("inaddr:0x%x\n", inaddr);
			return;
		}
	}
	printf("not found in cache!\n");
}

static void cpu_cache_read(hwaddr_t addr, void *data, size_t len) {
	cache_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	uint32_t offset = addr & CACHE_MASK;
	/* uint32_t inaddr = temp.inaddr;*//* not used */

	int i, valid_inset = -1, reading_i = -1;
	for(i = 0; i < NR_INSETNUM; i ++)
	{
		if(cachebufs[setnum][i].valid)
		{
			if(cachebufs[setnum][i].memmark == memmark)
			{
				reading_i = i; /* the position of data found in cache */
			}
		}
		else
		{
			valid_inset = i; /* the unused cache block */
		}
	}

	/* target data not found in cache.*/
	if(reading_i == -1)
	{
#ifdef DEBUG_CACHE_TIME_CALC
		memory_access_time += 100;
#endif
		/* no empty block found in target set */
		if(valid_inset == -1)
			valid_inset = rand()%NR_INSETNUM;
		cachebufs[setnum][valid_inset].memmark = memmark;
		reading_i = valid_inset;
		for(i = 0; i < NR_BLOCKSIZE; i ++)
		{
			cachebufs[setnum][valid_inset].buf[i] = cache2_read((addr&~CACHE_MASK) + i, 1);
		}
	}
#ifdef DEBUG_CACHE_TIME_CALC
	memory_access_time += 2;
#endif
	cachebufs[setnum][reading_i].valid = true;	
	memcpy(data, cachebufs[setnum][reading_i].buf + offset, len);
}

static void cpu_cache_write(hwaddr_t addr, uint8_t *data, uint8_t *mask)
{
	cache_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	/* uint32_t inaddr = temp.inaddr;*//* not used */

	int i, j;
	for(i = 0; i < NR_INSETNUM; i ++)
	{
		if(cachebufs[setnum][i].valid && cachebufs[setnum][i].memmark == memmark)
		{
			for(j = 0; j < NR_BLOCKSIZE; j ++)
				if(mask[j])
				{
#ifdef DEBUG_CACHE_WRITE
					uint32_t debug_addr = (memmark << (INADDR_WIDTH + SETNUM_WIDTH)) | (setnum << INADDR_WIDTH) | (j);
					if(debug_addr != addr
					  && debug_addr != addr + 1
					  && debug_addr != addr + 2
					  && debug_addr != addr + 3)
					{
						printf("write address error at 0x%x:", addr);
						printf("(cache)0x%x\t", debug_addr);
						printf("(real)0x%x\t", addr);
					}
#endif
					cachebufs[setnum][i].buf[j] = data[j];
				}
		}
	}

	/* update th dram */
	for(j = 0; j < NR_BLOCKSIZE; j ++)
		if(mask[j])
		{
			cache2_write((addr&~CACHE_MASK) + j, 1, data[j]);
		}
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
	uint32_t offset = addr & CACHE_MASK;
	len = 4;
	uint8_t temp[4];
	uint32_t temp_addr = (uint32_t)temp;
	int diff = offset + len - NR_BLOCKSIZE;

	if(diff > 0)
	{
		/* data cross the burst boundary */
		cpu_cache_read(addr, temp, len - diff);
		cpu_cache_read((addr + NR_BLOCKSIZE)&~CACHE_MASK, temp + len - diff, diff);
	}
	else
	{
		cpu_cache_read(addr, temp, len);
	}

#ifdef DEBUG_CACHE_READ
	int debug_i;
	uint32_t debug_addr = addr&~CACHE_MASK;
	int debug_len = ((offset + len > NR_BLOCKSIZE) + 1) * NR_BLOCKSIZE;
	for(debug_i = 0; debug_i < debug_len; debug_i ++)
	{
		uint8_t debug_data = dram_read(debug_addr + debug_i, 1);
		if(temp[debug_i] != debug_data)
		{
			printf("debug cache read error at 0x%x:", debug_addr + debug_i);
			printf("(cache)0x%x\t(dram)0x%x\n", temp[debug_i], debug_data);
		}
	}
#endif

	return unalign_rw(temp_addr, 4);
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t offset = addr & CACHE_MASK;
	uint8_t temp[2 * NR_BLOCKSIZE];
	uint8_t mask[2 * NR_BLOCKSIZE];
	memset(mask, 0, 2 * NR_BLOCKSIZE);

	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);

	cpu_cache_write(addr, temp, mask);
	if(offset + len > NR_BLOCKSIZE)
	{
		/* data cross the burst boundary */
		cpu_cache_write(addr + NR_BLOCKSIZE, temp + NR_BLOCKSIZE, mask + NR_BLOCKSIZE);
	}
}
