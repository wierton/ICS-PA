#include "cpu/exec/template-start.h"

#define instr movsx

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(movsb_,SUFFIX))
{
	int len = concat(decode_rm2r_,SUFFIX)(eip+1);
	int8_t result_t = op_src->val;
	int32_t result = result_t;
	OPERAND_W(op_dest,result,R_DS);
	print_asm_template2();
	return len + 1;
}

make_helper(concat(movsw_,SUFFIX))
{
	int len = concat(decode_rm2r_,SUFFIX)(eip+1);
	int16_t result_t = op_src->val;
	int32_t result = result_t;
	OPERAND_W(op_dest,result,R_DS);
	print_asm_template2();
	return len + 1;
}
#endif

#include "cpu/exec/template-end.h"
