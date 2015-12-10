#include "common.h"
#include "x86.h"
#include "memory.h"
#include <string.h>

static PDE updir[NR_PDE] align_to_page;
static CR3 ucr3;

inline PDE* get_updir() { return updir; }
inline uint32_t get_ucr3() { return ucr3.val; }

inline PDE* get_kpdir();

uint32_t brk = 0;

/* The brk() system call handler. */
void mm_brk(uint32_t new_brk) {
	if(new_brk > brk) {
		mm_malloc(brk, new_brk - brk);
	}
	brk = new_brk;
}

/* only can be used after init page */
char volatile str[201];
char *pstr;
int prints(char ptr[])
{
	int i;
	char *pptr;
	CR0 cr0;
	cr0.val = read_cr0();
	if(cr0.paging)
	{
		pstr = (char *)str;
		pptr = (char *)ptr;
	}
	else
	{
		pstr = (char *)va_to_pa(str);
		pptr = (char *)va_to_pa(ptr);
	}
	for(i = 0;i < 200;i ++)
	{
		if(pptr[i] == 0)
			break;
		pstr[i] = pptr[i];
	}
	pstr[i] = 0;
	asm volatile("mov $pstr,%eax;");
	asm volatile("bsf %eax,%eax;");
	return 0;
}

int printx(uint32_t addr)
{
	int i;
	str[0] = '0';
	str[1] = 'x';
	for(i = 7; i >= 0; i--)
	{
		uint8_t tmp = (addr >> (4 * i)) & 0xf;
		if(tmp < 0xa)
			str[9 - i] = '0' + tmp;
		else
			str[9 - i] = 'a' + tmp - 0xa;
	}
	str[10] = 0;
	asm volatile("mov $str,%eax;");
	asm volatile("bsf %eax,%eax;");
	return 0;
}

void init_mm() {
	PDE *kpdir = get_kpdir();

	/* make all PDE invalid */
	memset(updir, 0, NR_PDE * sizeof(PDE));

	/* create the same mapping above 0xc0000000 as the kernel mapping does */
	memcpy(&updir[KOFFSET / PT_SIZE], &kpdir[KOFFSET / PT_SIZE], 
			(PHY_MEM / PT_SIZE) * sizeof(PDE));

	ucr3.val = (uint32_t)va_to_pa((uint32_t)updir) & ~0xfff;
}

