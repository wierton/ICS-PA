#include "common.h"
#include "burst.h"
#include "misc.h"


#define NR_BLOCK 64
#define NR_RANK 128
#define NR_BANK 8

typedef struct {
	bool valid;
	uint32_t mark;
	uint8_t buf[NR_BLOCK];
} CCL;

CCL cachebufs[NR_RANK][NR_BANK];

void init_cache() {
	int i, j;
	for(i = 0; i < NR_RANK; i ++) {
		for(j = 0; j < NR_BANK; j++) {
			cachebufs[i][j].valid = false;
		}
	}
}


uint32_t cache_read(hwaddr_t addr, size_t len) {
	uint16_t mark = ((addr>>15)&0x0001ffff);
	uint8_t rank = ((addr>>8)&0x7f);
	uint8_t inaddr = (addr&0xff);

	int i;
	for(i = 0; i < NR_BANK; i ++)
	{
		if(cachebufs[rank][i].mark == mark)
			return cachebufs[rank][i].buf[inaddr];
	}
	return 0;
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {

}

