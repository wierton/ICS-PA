#include "common.h"
#include "misc.h"

#include "memory/memory.h"

/* define if necessary */
/* #define DEBUG_CACHE2_READ */
/* #define DEBUG_CACHE2_WRITE */

#define INADDR2_WIDTH 6
#define SETNUM2_WIDTH 12
#define MEMMARK2_WIDTH (32 - SETNUM2_WIDTH - INADDR2_WIDTH)

typedef union {
	struct {
		uint32_t inaddr		: INADDR2_WIDTH;
		uint32_t setnum		: SETNUM2_WIDTH;
		uint32_t memmark	: MEMMARK2_WIDTH;
	};
	uint32_t addr;
} cache2_addr;

#define NR2_BLOCKSIZE (1 << INADDR2_WIDTH)
#define NR2_SETNUM (1 << SETNUM2_WIDTH)
#define NR2_INSETNUM 16

#define CACHE2_MASK (NR2_BLOCKSIZE - 1)

typedef struct {
	bool valid, dirty;
	uint32_t memmark;
	uint8_t buf[NR2_BLOCKSIZE];
} CCL;

CCL cache2bufs[NR2_SETNUM][NR2_INSETNUM];

/* declaration of dram_read and dram_write */
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

void init_cache2() {
	int i, j;
	for(i = 0; i < NR2_SETNUM; i ++) {
		for(j = 0; j < NR2_INSETNUM; j++) {
			cache2bufs[i][j].valid = false;
			cache2bufs[i][j].dirty = false;
		}
	}
}

static void cpu_cache2_read(hwaddr_t addr, void *data) {
	cache2_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	/* uint32_t inaddr = temp.inaddr;*//* not used */

	int i, valid_inset = -1, reading_i = -1;
	for(i = 0; i < NR2_INSETNUM; i ++)
	{
		if(cache2bufs[setnum][i].valid)
		{
			if(cache2bufs[setnum][i].memmark == memmark)
			{
				reading_i = i; /* the position of data found in cache2 */
			}
		}
		else
		{
			valid_inset = i; /* the unused cache2 block */
		}
	}

	/* target data not found in cache2.*/
	if(reading_i == -1)
	{
		/* no empty block found in target set */
		if(valid_inset == -1)
			valid_inset = 0;/* need to be random */
		/* if dirty , write back*/
		if(cache2bufs[setnum][valid_inset].valid && cache2bufs[setnum][valid_inset].dirty)
		{
			for(i = 0; i < NR2_BLOCKSIZE; i ++)
			{
				dram_write((addr&~CACHE2_MASK) + i, 1, cache2bufs[setnum][valid_inset].buf[i]);
			}
		}
		/* clean the dirty bit */
		cache2bufs[setnum][valid_inset].dirty = false;
		/* update the memmark */
		cache2bufs[setnum][valid_inset].memmark = memmark;
		reading_i = valid_inset;
		/* read from dram */
		for(i = 0; i < NR2_BLOCKSIZE; i ++)
		{
			cache2bufs[setnum][valid_inset].buf[i] = dram_read((addr&~CACHE2_MASK) + i, 1);
		}
	}
	cache2bufs[setnum][reading_i].valid = true;	
	memcpy(data, cache2bufs[setnum][reading_i].buf, NR2_BLOCKSIZE);
}

static void cpu_cache2_write(hwaddr_t addr, uint8_t *data, uint8_t *mask)
{
	cache2_addr temp;
	temp.addr = addr;
	uint32_t memmark = temp.memmark;
	uint32_t setnum = temp.setnum;
	uint8_t read_data[2 * NR2_BLOCKSIZE];
	/* uint32_t inaddr = temp.inaddr;*//* not used */

	int i, j;
#ifdef DEBUG_CACHE2_WRITE
	int loop_times = 0;
#endif
	bool found_in_cache2 = false;
	do {
		for(i = 0; i < NR2_INSETNUM; i ++)
		{
			if(cache2bufs[setnum][i].valid && cache2bufs[setnum][i].memmark == memmark)
			{
				found_in_cache2 = true;
				cache2bufs[setnum][i].dirty = true;
				for(j = 0; j < NR2_BLOCKSIZE; j ++)
					if(mask[j])
					{
						cache2bufs[setnum][i].buf[j] = data[j];
					}
			}
		}
#ifdef DEBUG_CACHE2_WRITE
		loop_times ++;
		if(loop_times > 2)
		{
			printf("cache2 write error, loop times exceed limit : %d\n", loop_times);
			break;
		}
#endif
		/* if not found, read from dram */
		if(!found_in_cache2)
			cpu_cache2_read(addr, read_data);
	} while(!found_in_cache2);
}

uint32_t cache2_read(hwaddr_t addr, size_t len) {
	uint32_t offset = addr & CACHE2_MASK;
	uint8_t temp[2 * NR2_BLOCKSIZE];

	cpu_cache2_read(addr, temp);

	if(offset + len > NR2_BLOCKSIZE)
	{
		/* data cross the burst boundary */
		cpu_cache2_read(addr + NR2_BLOCKSIZE, temp + NR2_BLOCKSIZE);
	}

#ifdef DEBUG_CACHE2_READ
	int debug_i;
	uint32_t debug_addr = addr&~CACHE2_MASK;
	int debug_len = ((offset + len > NR2_BLOCKSIZE) + 1) * NR2_BLOCKSIZE;
	for(debug_i = 0; debug_i < debug_len; debug_i ++)
	{
		uint8_t debug_data = dram_read(debug_addr + debug_i, 1);
		if(temp[debug_i] != debug_data)
		{
			printf("debug cache2 read error at 0x%x:", debug_addr + debug_i);
			printf("(cache2)0x%x\t(dram)0x%x\n", temp[debug_i], debug_data);
		}
	}
#endif

	return unalign_rw(temp + offset, 4);
}

void cache2_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t offset = addr & CACHE2_MASK;
	uint8_t temp[2 * NR2_BLOCKSIZE];
	uint8_t mask[2 * NR2_BLOCKSIZE];
	memset(mask, 0, 2 * NR2_BLOCKSIZE);

	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);

	cpu_cache2_write(addr, temp, mask);
	if(offset + len > NR2_BLOCKSIZE)
	{
		/* data cross the burst boundary */
		cpu_cache2_write(addr + NR2_BLOCKSIZE, temp + NR2_BLOCKSIZE, mask + NR2_BLOCKSIZE);
	}
}
