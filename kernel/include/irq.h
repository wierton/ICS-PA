#ifndef __IRQ_H__
#define __IRQ_H__

#include "common.h"

/* TODO: The decleration order of the members in the ``TrapFrame''
 * structure below is wrong. Please re-orgainize it for the C
 * code to use the trap frame correctly.
 */

typedef struct TrapFrame {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t old_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	int irq;
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
/*	uint32_t esi, ebx, eax, eip, edx, error_code, eflags, ecx, cs, old_esp, edi, ebp;
	int32_t irq;*/
} TrapFrame;

#endif
