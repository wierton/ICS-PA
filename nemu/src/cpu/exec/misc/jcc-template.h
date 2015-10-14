#include "cpu/exec/template-start.h"

#define instr jcc

static void do_eflags_execute(bool flag,int diplacement,char jcc_flagstr[])
{
	DATA_TYPE_S eip_dis = op_src->val;
	int eip_pos = cpu.eip;
	if(flag)
	{
		cpu.eip = cpu.eip + eip_dis;
	}
	print_asm("%s $0x%x",jcc_flagstr,eip_pos+eip_dis+diplacement);
}


#define flagstr jcc
make_eflags_helper(i,!cpu.CF,!cpu.ZF)
#undef flagstr


#define flagstr ja
make_eflags_helper(i,!cpu.CF,!cpu.ZF)
#undef flagstr
	
#define flagstr jae
make_eflags_helper(i,!cpu.CF,1)
#undef flagstr
	
#define flagstr jb
make_eflags_helper(i,cpu.CF,1)
#undef flagstr
	
#define flagstr jbe
make_eflags_helper(i,cpu.CF || cpu.ZF,1)
#undef flagstr
	
#define flagstr jc
make_eflags_helper(i,cpu.CF,1)
#undef flagstr
	
#define flagstr je
make_eflags_helper(i,cpu.ZF,1)
#undef flagstr
	
#define flagstr jz
make_eflags_helper(i,cpu.ZF,1)
#undef flagstr
	
#define flagstr jg
make_eflags_helper(i,!cpu.ZF,cpu.SF==cpu.OF)
#undef flagstr
	
#define flagstr jge
make_eflags_helper(i,cpu.SF==cpu.OF,1)
#undef flagstr
	
#define flagstr jl
make_eflags_helper(i,cpu.SF!=cpu.OF,1)
#undef flagstr
	
#define flagstr jle
make_eflags_helper(i,(cpu.ZF || (cpu.SF!=cpu.OF)),1)
#undef flagstr
	
#define flagstr jna
make_eflags_helper(i,cpu.CF,cpu.SF)
#undef flagstr
	
#define flagstr jnae
make_eflags_helper(i,cpu.CF,1)
#undef flagstr
	
#define flagstr jnb
make_eflags_helper(i,!cpu.CF,1)
#undef flagstr
	
#define flagstr jnbe
make_eflags_helper(i,!cpu.CF,!cpu.ZF)
#undef flagstr
	
#define flagstr jnc
make_eflags_helper(i,!cpu.CF,1)
#undef flagstr
	
#define flagstr jne
make_eflags_helper(i,!cpu.ZF,1)
#undef flagstr
	
#define flagstr jng
make_eflags_helper(i,cpu.ZF || (cpu.SF!=cpu.OF),1)
#undef flagstr
	
#define flagstr jnge
make_eflags_helper(i,cpu.SF!=cpu.OF,1)
#undef flagstr
	
#define flagstr jnl
make_eflags_helper(i,cpu.SF==cpu.OF,1)
#undef flagstr
	
#define flagstr jnle
make_eflags_helper(i,!cpu.ZF,cpu.SF==cpu.OF)
#undef flagstr
	
#define flagstr jno
make_eflags_helper(i,!cpu.OF,1)
#undef flagstr
	
#define flagstr jnp
make_eflags_helper(i,!cpu.PF,1)
#undef flagstr
	
#define flagstr jns
make_eflags_helper(i,!cpu.SF,1)
#undef flagstr
	
#define flagstr jnz
make_eflags_helper(i,!cpu.ZF,1)
#undef flagstr
	
#define flagstr jo
make_eflags_helper(i,cpu.OF,1)
#undef flagstr
	
#define flagstr jp
make_eflags_helper(i,cpu.PF,1)
#undef flagstr
	
#define flagstr jpe
make_eflags_helper(i,cpu.PF,1)
#undef flagstr
	
#define flagstr jpo
make_eflags_helper(i,!cpu.PF,1)
#undef flagstr
	
#define flagstr js
make_eflags_helper(i,cpu.SF,1)
#undef flagstr


#include "cpu/exec/template-end.h"
