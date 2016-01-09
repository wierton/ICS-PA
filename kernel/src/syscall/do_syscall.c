#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);

static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}

void serial_printc(char ch);

int prints(char []);
int printx(uint32_t);

int fs_open(const char *pathname, int flags);
int fs_read(int fd, void *buf, int len);
int fs_write(int fd, void *buf, int len);
int fs_lseek(int fd, int offset, int whence);
int fs_close(int fd);

void do_syscall(TrapFrame *tf) {
	int i;
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0:
		   prints("syscall happened  ");	
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;

		/* TODO: Add more system calls. */
		case SYS_write:
			if(tf->ebx == 0x1 || tf->ebx == 0x2)
			{
				for(i=0;i<tf->edx;i++)
				{
					serial_printc(*(char *)(tf->ecx + i));
				}
				tf->eax = tf->edx;
			}
			else
			{
				tf->eax = fs_write(tf->ebx - 3, (void *)tf->ecx, tf->edx);
			}
			/*asm volatile (".byte 0xd6" : : "a"(2), "c"(tf->ecx), "d"(tf->edx));*/
			break;
		case SYS_read:
			if(tf->ebx >= 0x3)
			{
				tf->eax = fs_read(tf->ebx - 3, (void *)tf->ecx, tf->edx);
			}
			break;
		case SYS_lseek:
			tf->eax = fs_lseek(tf->ebx - 3, tf->ecx, tf->edx);
			break;
		case SYS_close:
			tf->eax = fs_close(tf->ecx - 3);
			break;
		case SYS_open:
			prints("open:");
			prints((void*)tf->ebx);prints(",");
			tf->eax = fs_open((void *)tf->ebx, 0);
			printx(tf->eax);prints("\n");
			break;


		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}

