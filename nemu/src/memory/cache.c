#include "common.h"

#define NR_CCL 1024

typedef struct {
	bool valid;
	uint8_t buf[NR_CCL];
} CCL;
