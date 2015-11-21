#include "common.h"
#include "misc.h"

#include "memory/memory.h"

#define DEBUG_CACHE

#define INADDR_WIDTH 6
#define SETNUM_WIDTH 7
#define MEMMARK_WIDTH (32 - SETNUM_WIDTH - INADDR_WIDTH)

typedef union {
	struct {
		uint32_t inaddr		: INADDR_WIDTH;
		uint32_t setnum		: SETNUM_WIDTH;
		uint32_t memmark	: MEMMARK_WIDTH;
	};
	uint32_t addr;
} cachedebug_addr;

#define NR_BLOCKSIZE (1 << INADDR_WIDTH)
#define NR_SETNUM (1 << SETNUM_WIDTH)
#define NRdebug_iNSETNUM 8

#define CACHE_MASK (NR_BLOCKSIZE - 1)

typedef struct {
	bool valid;
	uint32_t memmark;
	uint8_t buf[NR_BLOCKSIZE];
} CCL;

CCL cachebufs[NR_SETNUM][NRdebug_iNSETNUM];

/* declaration of dram_read and dram_write */
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

void init_cache() {
	int i, j;
	for(i = 0; i < NR_SETNUM; i ++) {
		for(j = 0; j < NRdebug_iNSETNUM; j++) {
			cachebufs[i][j].valid = false;
		}
	}
}

static void cpu_cache_read(hwaddr_t addr, void *data) {
	cachedebug_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	/* uint32_t inaddr = temp.inaddr;*//* not used */

	int i, validdebug_inset = -1, readingdebug_i = -1;
	for(i = 0; i < NRdebug_iNSETNUM; i ++)
	{
		if(cachebufs[setnum][i].valid)
		{
			if(cachebufs[setnum][i].memmark == memmark)
			{
				readingdebug_i = i; /* the position of data found in cache */
			}
		}
		else
		{
			validdebug_inset = i; /* the unused cache block */
		}
	}

	/* target data not found in cache.*/
	if(readingdebug_i == -1)
	{
		/* no empty block found in target set */
		if(validdebug_inset == -1)
			validdebug_inset = 0;
		cachebufs[setnum][validdebug_inset].memmark = memmark;
		readingdebug_i = validdebug_inset;
		for(i = 0; i < NR_BLOCKSIZE; i ++)
		{
			cachebufs[setnum][validdebug_inset].buf[i] = dram_read((addr&~CACHE_MASK) + i, 1);
		}
	}
	
	memcpy(data, cachebufs[setnum][readingdebug_i].buf, NR_BLOCKSIZE);
}

static void cpu_cache_write(hwaddr_t addr, uint8_t *data, uint8_t *mask)
{
	cachedebug_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	/* uint32_t inaddr = temp.inaddr;*//* not used */

	int i, j;
	for(i = 0; i < NRdebug_iNSETNUM; i ++)
	{
		if(cachebufs[setnum][i].valid && cachebufs[setnum][i].memmark == memmark)
		{
			for(j = 0; j < NR_BLOCKSIZE; j ++)
			{
				if(mask[j])
				{
					cachebufs[setnum][i].buf[j] = data[j];
					dram_write(addr, data[j], 1);
				}
			}
		}
	}	
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
	uint32_t offset = addr & CACHE_MASK;
	uint8_t temp[2 * NR_BLOCKSIZE];

	cpu_cache_read(addr, temp);

	if(offset + len > NR_BLOCKSIZE)
	{
		/* data cross the burst boundary */
		cpu_cache_read(addr + NR_BLOCKSIZE, temp + NR_BLOCKSIZE);
	}

#ifdef DEBUG_CACHE
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
	printf("\n");
#endif

	return unalign_rw(temp + offset, 4);
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
