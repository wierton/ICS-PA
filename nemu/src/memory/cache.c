#include "common.h"

#define NR_CCL 64
#define NR_RANK 1024

typedef struct {
	bool valid;
	uint8_t buf[NR_CCL];
} CCL;

CCL cachebufs[NR_RANK];

void init_cache() {
	int i;
	for(i = 0; i < NR_RANK; i ++)
	{
		cachebufs[i].valid = false;
	}
}

/*
static void cache_read(hwaddr_t addr, void *data) {
}

static void cache_write(hwaddr_t addr, void *data) {
}
*/
