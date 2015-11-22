#include "common.h"

/* Define if necessary */
/* #define DEBUG_CACHE_READ */
/* #define DEBUG_CACHE_WRITE */
/* #define DEBUG_CACHE2_READ */
/* #define DEBUG_CACHE2_WRITE */
/* #define DEBUG_CACHE_TIME_CALC */

#ifdef DEBUG_CACHE_TIME_CALC
extern long long memory_access_time;
#endif

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

uint32_t cache_read(hwaddr_t, size_t);
void cache_write(hwaddr_t, size_t, uint32_t);

uint32_t cache2_read(hwaddr_t, size_t);
void cache2_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
#if defined(DEBUG_CACHE_READ)
	uint32_t cache_data = cache_read(addr, len) & (~0u >> ((4 - len) << 3));
	uint32_t dram_data = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	if(cache_data != dram_data)
		printf("data read error at 0x%x: (cache)0x%x\t(dram)0x%x\n", addr, cache_data, dram_data);
#elif defined(DEBUG_CACHE2_READ)
	uint32_t cache2_data = cache2_read(addr, len) & (~0u >> ((4 - len) << 3));
	uint32_t dram_data = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	if(cache2_data != dram_data)
		printf("data read error at 0x%x: (cache)0x%x\t(dram)0x%x\n", addr, cache2_data, dram_data);
#endif
	return cache_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
#if defined(DEBUG_CACHE_WRITE)
	dram_write(addr, len, data);
	uint32_t dram_data = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	cache_write(addr, len, data);
	uint32_t cache_data = dram_read(addr, len) & (~0u >> ((4 - len)<< 3));
	if(cache_data != dram_data)
		printf("data write error at 0x%x: (cache)0x%x\t(dram)0x%x\n", addr, cache_data, dram_data);
#elif defined(DEBUG_CACHE2_WRITE)
	dram_write(addr, len, data);
	uint32_t dram_data = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	cache2_write(addr, len, data);
	uint32_t cache2_data = dram_read(addr, len) & (~0u >> ((4 - len)<< 3));
	if(cache2_data != dram_data)
		printf("data write error at 0x%x: (cache)0x%x\t(dram)0x%x\n", addr, cache2_data, dram_data);
#endif
	cache_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

