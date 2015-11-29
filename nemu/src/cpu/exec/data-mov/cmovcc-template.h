#include "cpu/exec/template-start.h"

#define instr cmovcc

static void do_eflags_execute(bool flag,int diplacement,char cmovcc_flagstr[])
{
	if(flag)
	{
		OPERAND_W(op_dest, op_src->val, R_DS);
	}
	print_asm_template2();
}


#define flagstr cmova
make_eflags_helper(rm2r,!cpu.CF,!cpu.ZF)
#undef flagstr
	
#define flagstr cmovae
make_eflags_helper(rm2r,!cpu.CF,1)
#undef flagstr
	
#define flagstr cmovb
make_eflags_helper(rm2r,cpu.CF,1)
#undef flagstr
	
#define flagstr cmovbe
make_eflags_helper(rm2r,cpu.CF || cpu.ZF,1)
#undef flagstr
	
#define flagstr cmovc
make_eflags_helper(rm2r,cpu.CF,1)
#undef flagstr
	
#define flagstr cmove
make_eflags_helper(rm2r,cpu.ZF,1)
#undef flagstr
	
#define flagstr cmovz
make_eflags_helper(rm2r,cpu.ZF,1)
#undef flagstr
	
#define flagstr cmovg
make_eflags_helper(rm2r,!cpu.ZF,cpu.SF==cpu.OF)
#undef flagstr
	
#define flagstr cmovge
make_eflags_helper(rm2r,cpu.SF==cpu.OF,1)
#undef flagstr
	
#define flagstr cmovl
make_eflags_helper(rm2r,cpu.SF!=cpu.OF,1)
#undef flagstr
	
#define flagstr cmovle
make_eflags_helper(rm2r,(cpu.ZF || (cpu.SF!=cpu.OF)),1)
#undef flagstr
	
#define flagstr cmovna
make_eflags_helper(rm2r,cpu.CF,cpu.SF)
#undef flagstr
	
#define flagstr cmovnae
make_eflags_helper(rm2r,cpu.CF,1)
#undef flagstr
	
#define flagstr cmovnb
make_eflags_helper(rm2r,!cpu.CF,1)
#undef flagstr
	
#define flagstr cmovnbe
make_eflags_helper(rm2r,!cpu.CF,!cpu.ZF)
#undef flagstr
	
#define flagstr cmovnc
make_eflags_helper(rm2r,!cpu.CF,1)
#undef flagstr
	
#define flagstr cmovne
make_eflags_helper(rm2r,!cpu.ZF,1)
#undef flagstr
	
#define flagstr cmovng
make_eflags_helper(rm2r,cpu.ZF || (cpu.SF!=cpu.OF),1)
#undef flagstr
	
#define flagstr cmovnge
make_eflags_helper(rm2r,cpu.SF!=cpu.OF,1)
#undef flagstr
	
#define flagstr cmovnl
make_eflags_helper(rm2r,cpu.SF==cpu.OF,1)
#undef flagstr
	
#define flagstr cmovnle
make_eflags_helper(rm2r,!cpu.ZF,cpu.SF==cpu.OF)
#undef flagstr
	
#define flagstr cmovno
make_eflags_helper(rm2r,!cpu.OF,1)
#undef flagstr
	
#define flagstr cmovnp
make_eflags_helper(rm2r,!cpu.PF,1)
#undef flagstr
	
#define flagstr cmovns
make_eflags_helper(rm2r,!cpu.SF,1)
#undef flagstr
	
#define flagstr cmovnz
make_eflags_helper(rm2r,!cpu.ZF,1)
#undef flagstr
	
#define flagstr cmovo
make_eflags_helper(rm2r,cpu.OF,1)
#undef flagstr
	
#define flagstr cmovp
make_eflags_helper(rm2r,cpu.PF,1)
#undef flagstr
	
#define flagstr cmovpe
make_eflags_helper(rm2r,cpu.PF,1)
#undef flagstr
	
#define flagstr cmovpo
make_eflags_helper(rm2r,!cpu.PF,1)
#undef flagstr
	
#define flagstr cmovs
make_eflags_helper(rm2r,cpu.SF,1)
#undef flagstr


#include "cpu/exec/template-end.h"
