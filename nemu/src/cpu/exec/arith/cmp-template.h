#include "cpu/exec/template-start.h"

#define instr cmp


static void do_execute() {

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(cmp_si2rm_, SUFFIX)) {

	return 5;
}

#include "cpu/exec/template-end.h"
