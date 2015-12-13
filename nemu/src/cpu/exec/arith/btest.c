#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "btest-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "btest-template.h"
#undef DATA_BYTE

make_helper_v(bt_i2rm)
make_helper_v(bt_r2rm)


