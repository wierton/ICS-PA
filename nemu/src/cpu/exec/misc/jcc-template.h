#include "cpu/exec/template-start.h"

static void do_jcc_execute(int diplacement) {
	cpu.eip = cpu.eip + op_src->val;
	print_asm(str(jcc)" $0x%x",cpu.eip+diplacement);
}

#define instr jcc
make_jcc_helper(i,!cpu.CF,!cpu.ZF)
#undef instr


#define instr ja
make_jcc_helper(i,!cpu.CF,!cpu.ZF)
#undef instr
	
#define instr jae
make_jcc_helper(i,!cpu.CF,1)
#undef instr
	
#define instr jb
make_jcc_helper(i,cpu.CF,1)
#undef instr
	
#define instr jbe
make_jcc_helper(i,cpu.CF || cpu.ZF,1)
#undef instr
	
#define instr jc
make_jcc_helper(i,cpu.CF,1)
#undef instr
	
#define instr je
make_jcc_helper(i,cpu.ZF,1)
#undef instr
	
#define instr jz
make_jcc_helper(i,cpu.ZF,1)
#undef instr
	
#define instr jg
make_jcc_helper(i,!cpu.ZF,cpu.SF==cpu.OF)
#undef instr
	
#define instr jge
make_jcc_helper(i,cpu.SF==cpu.OF,1)
#undef instr
	
#define instr jl
make_jcc_helper(i,cpu.SF!=cpu.OF,1)
#undef instr
	
#define instr jle
make_jcc_helper(i,cpu.ZF,cpu.SF!=cpu.OF)
#undef instr
	
#define instr jna
make_jcc_helper(i,cpu.CF,cpu.SF)
#undef instr
	
#define instr jnae
make_jcc_helper(i,cpu.CF,1)
#undef instr
	
#define instr jnb
make_jcc_helper(i,!cpu.CF,1)
#undef instr
	
#define instr jnbe
make_jcc_helper(i,!cpu.ZF,!cpu.ZF)
#undef instr
	
#define instr jnc
make_jcc_helper(i,!cpu.CF,1)
#undef instr
	
#define instr jne
make_jcc_helper(i,!cpu.ZF,1)
#undef instr
	
#define instr jng
make_jcc_helper(i,cpu.ZF || (cpu.SF!=cpu.OF),1)
#undef instr
	
#define instr jnge
make_jcc_helper(i,cpu.SF!=cpu.OF,1)
#undef instr
	
#define instr jnl
make_jcc_helper(i,cpu.SF==cpu.OF,1)
#undef instr
	
#define instr jnle
make_jcc_helper(i,!cpu.ZF,cpu.SF==cpu.OF)
#undef instr
	
#define instr jno
make_jcc_helper(i,!cpu.OF,1)
#undef instr
	
#define instr jnp
make_jcc_helper(i,!cpu.PF,1)
#undef instr
	
#define instr jns
make_jcc_helper(i,!cpu.SF,1)
#undef instr
	
#define instr jnz
make_jcc_helper(i,!cpu.ZF,1)
#undef instr
	
#define instr jo
make_jcc_helper(i,cpu.OF,1)
#undef instr
	
#define instr jp
make_jcc_helper(i,cpu.PF,1)
#undef instr
	
#define instr jpe
make_jcc_helper(i,cpu.PF,1)
#undef instr
	
#define instr jpo
make_jcc_helper(i,!cpu.PF,1)
#undef instr
	
#define instr js
make_jcc_helper(i,cpu.SF,1)
#undef instr


#include "cpu/exec/template-end.h"
