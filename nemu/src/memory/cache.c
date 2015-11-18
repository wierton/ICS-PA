#include "common.h"

#define NR_BLOCK 64
#define NR_RANK 128
#define NR_BANK 8

typedef struct {
	bool valid;
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

/*
static void cache_read(hwaddr_t addr, void *data) {
}

static void cache_write(hwaddr_t addr, void *data) {
}
*/
