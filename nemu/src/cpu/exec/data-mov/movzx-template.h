#include "cpu/exec/template-start.h"

#define instr movzx

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(movzb_,SUFFIX))
{
	int len = concat(decode_rm2r_,SUFFIX)(eip+1);
	uint8_t result = op_src->val;
	OPERAND_W(op_dest,result,R_DS);
	print_asm_template2();
	return len + 1;
}

make_helper(concat(movzw_,SUFFIX))
{
	int len = concat(decode_rm2r_,SUFFIX)(eip+1);
	uint16_t result = op_src->val;
	OPERAND_W(op_dest,result,R_DS);
	print_asm_template2();
	return len + 1;
}
#endif

#include "cpu/exec/template-end.h"
