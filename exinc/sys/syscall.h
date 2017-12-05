#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#define SYS_read 3
#define SYS_write 4
#define SYS_open 5
#define SYS_close 6
#define SYS_lseek 19
#define SYS_brk 45

#define __NR_read	SYS_read
#define __NR_write	SYS_write
#define __NR_open	SYS_open
#define __NR_close	SYS_close
#define __NR_lseek	SYS_lseek
#define __NR_brk	SYS_brk

#endif
