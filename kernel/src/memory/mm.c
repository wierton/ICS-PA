#include "common.h"
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

char str[100];
/* only can be used after init page */
int printc(char c, int i)
{
	nemu_assert(i < 100);
	str[i] = c;
	asm volatile("bsf %eax,%eax;");
	return 0;
}

void init_mm() {
	PDE *kpdir = get_kpdir();

	/* make all PDE invalid */
	memset(updir, 0, NR_PDE * sizeof(PDE));

	/* create the same mapping above 0xc0000000 as the kernel mapping does */
	printc('k', 0);
	memcpy(&updir[KOFFSET / PT_SIZE], &kpdir[KOFFSET / PT_SIZE], 
			(PHY_MEM / PT_SIZE) * sizeof(PDE));

	ucr3.val = (uint32_t)va_to_pa((uint32_t)updir) & ~0xfff;
}

