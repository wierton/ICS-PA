#ifndef __COMMON_H__
#define __COMMON_H__

//#define USE_RAMDISK

/* You will define this macro in PA4 */
#define HAS_DEVICE

#define PERF

//#define DEBUG
#define LOG_FILE

#include "debug.h"
#include "macro.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint8_t bool;
typedef uint32_t hwaddr_t;
typedef uint32_t lnaddr_t;
typedef uint32_t swaddr_t;

typedef uint16_t ioaddr_t;

#pragma pack (1)
typedef union {
	uint32_t _4;
	uint32_t _3	: 24;
	uint16_t _2;
	uint8_t _1;
} unalign;

typedef union {
	struct {
		uint32_t data_0_7 :8;
		uint32_t data_8_31:24;
	};
	struct {
		uint32_t data_0_15 :16;
		uint32_t data_16_31:16;
	};
	struct {
		uint32_t data_0_23 :24;
		uint32_t data_24_31:8;
	};
	struct {
		uint32_t val_0_7   :8;
		uint32_t val_8_15  :8;
		uint32_t val_16_23 :8;
		uint32_t val_24_31 :8;
	};
	uint32_t val;
} CrossData;
#pragma pack ()

#define false 0
#define true 1

#endif
