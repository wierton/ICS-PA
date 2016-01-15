#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/readelf.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

extern swaddr_t stop_eip;

void cpu_exec(uint32_t);

inline void fix_cost_start();

uint32_t hwaddr_read(hwaddr_t, size_t);
hwaddr_t page_translate(lnaddr_t addr);
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg);

void print_cache_info_by_addr(swaddr_t addr);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_bt(char *args);

static int cmd_cache(char *args);

static int cmd_b(char *args);

static int cmd_addr(char *args);

static int cmd_xp(char *args);

static int cmd_fix(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{ "si", "Execute the program x steps",cmd_si},
	{ "info", "Print the reg or watch_point",cmd_info},
	{ "x", "Read memory from given address",cmd_x},
	{ "p", "Calculate the expression",cmd_p},
	{ "w", "Add a watchpoint",cmd_w},
	{ "d", "Delete a watchpoint",cmd_d},
	{ "bt", "Print the stack frame chain",cmd_bt},
	{ "cache", "printf cache info by address", cmd_cache},
	{ "b", "break eip", cmd_b},
	{ "addr", "addr", cmd_addr},
	{ "xp", "xp", cmd_xp},
	{ "fix", "fix", cmd_fix}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_fix(char *args)
{
	fix_cost_start();
	return 0;
}

static int cmd_addr(char *args)
{
	bool IsValid;
	if(args == NULL) return 0;
	swaddr_t addr = eval(args, &IsValid);
	if(!IsValid)
		return 0;
	
	lnaddr_t lnaddr = seg_translate(addr, 4, R_DS);
	printf("sreg:0x%x --> 0x%x\n", addr, lnaddr);
	
	/* page translate */
	PDE pdir;
	PTE ptable;
	
	if(!cpu.CR0.protect_enable || !cpu.CR0.paging)
		return addr;
	PageAddr pageaddr;
	pageaddr.val = addr;

	/* read page dir */
	pdir.val = hwaddr_read((cpu.CR3.page_directory_base << 12) | pageaddr.pagedir * 4, 4);

	if(!pdir.present)
	{
		printf("pdir not present!\n");
	}
	else
	{
		ptable.val = hwaddr_read((pdir.page_frame << 12) | pageaddr.pagetab * 4, 4);
	
		printf("cr3 base:0x%x\n", cpu.CR3.page_directory_base << 12);
		printf("page dir:0x%x\n", pageaddr.pagedir);
		printf("page_frame:0x%x\n", pdir.page_frame);
		printf("logic addr:0x%x\n",addr);
		printf("lnaddr:0x%x\n", (pdir.page_frame << 12) + pageaddr.pagetab * 4);

		if(!ptable.present)
		{
			printf("pdir not present!\n");
		}
		else
		{
			/* calc physic address */
			uint32_t hwaddr = (ptable.page_frame << 12) | pageaddr.off;
			printf("page:0x%x --> 0x%x\n\n", lnaddr, hwaddr);
		
		}
	}

	return 0;
}

static int cmd_xp(char *args)
{
	int i=0;
	char ch,*para, *expression;
	int len_para, len_args;
	size_t len;
	uint32_t read_addr = 0;
	bool para_success = false, addr_success = false;

	if(args == NULL)
		return 0;

	len_args = strlen(args);

	para = strtok(args, " ");
	if(para == NULL)
	{
		printf("too few parameters!\n");
		return 0;
	}

	len_para = strlen(para);
	if(!(args + len_args > para + len_para))
	{
		printf("too few parameters!\n");
		return 0;
	}

	expression = strtok(NULL, " ");

	read_addr = eval(expression, &addr_success);

	if(strcmp(para, "s")==0)
	{
		printf("0x%x:",read_addr);
		do
		{
			ch = hwaddr_read(read_addr+i, 1);
			printf("%c",ch);
			i++;
		}while(ch != 0);
		printf("\n");
		return 0;
	}

	len = eval(para, &para_success);
	if(!(para_success && addr_success))
	{
		return 0;
	}

	for(i=0;i<len;i++)
	{
		uint32_t value = hwaddr_read(read_addr+4*i, 4);
		printf("$0x%0x:\t0x%0x\t%u\n",read_addr+4*i,value,value);
	}

	return 0;
}

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args)
{
	int steps;
	bool is_valid;
	
	if(args == NULL)
	{
		cpu_exec(1);
		return 0;
	}
	
	steps = eval(args, &is_valid);
	assert(is_valid);
	cpu_exec(steps);
	return 0;
}

static int cmd_info(char *args)
{

	if(strcmp(args,"r")==0)
	{
		printf("eax\t$0x%08x\t%d\n",cpu.eax,cpu.eax);
		printf("ecx\t$0x%08x\t%d\n",cpu.ecx,cpu.ecx);
		printf("edx\t$0x%08x\t%d\n",cpu.edx,cpu.edx);
		printf("ebx\t$0x%08x\t%d\n",cpu.ebx,cpu.ebx);
		printf("esp\t$0x%08x\t$0x%08x\n",cpu.esp,cpu.esp);
		printf("ebp\t$0x%08x\t$0x%08x\n",cpu.ebp,cpu.ebp);
		printf("esi\t$0x%08x\t%d\n",cpu.esi,cpu.esi);
		printf("edi\t$0x%08x\t%d\n",cpu.edi,cpu.edi);
		printf("eip\t$0x%08x\t$0x%08x\n",cpu.eip,cpu.eip);
		printf("CS\t$0x%04x\n",cpu.CS.val);
		printf("SS\t$0x%04x\n",cpu.SS.val);
		printf("DS\t$0x%04x\n",cpu.DS.val);
		printf("ES\t$0x%04x\n",cpu.ES.val);
		printf("CR0\t$0x%08x\n",cpu.CR0.val);
		printf("CR3\t$0x%08x\n",cpu.CR3.val);
	}
	else if(strcmp(args,"w")==0)
	{
		print_wp();
	}
	else if(strcmp(args,"f")==0)
	{
		printf("OF SF ZF PF CF\n");
		printf("%d  %d  %d  %d  %d\n", cpu.OF, cpu.SF, cpu.ZF, cpu.PF, cpu.CF);
	}
	else if(strcmp(args, "symtab") == 0)
	{
		show_symtab();
	}
	else if(strcmp(args, "gdtr") == 0)
	{
		printf("GDTR [base:0x%x limit:0x%x]\n", cpu.GDTR.base, cpu.GDTR.limit);
	}
	else if(strcmp(args, "ldtr") == 0)
	{
		printf("LDTR [base:0x%x limit:0x%x]\n", cpu.LDTR.base, cpu.LDTR.limit);
	}
	else if(strcmp(args, "idtr") == 0)
	{
		printf("IDTR [base:0x%x limit:0x%x]\n", cpu.IDTR.base, cpu.IDTR.limit);
	}
	else
		printf("Unknown parameter:%s\n",args);
	return 0;
}

static int cmd_x(char *args)
{
	int i=0;
	char ch,*para, *expression;
	int len_para, len_args;
	size_t len;
	uint32_t read_addr = 0;
	bool para_success = false, addr_success = false;

	if(args == NULL)
		return 0;

	len_args = strlen(args);

	para = strtok(args, " ");
	if(para == NULL)
	{
		printf("too few parameters!\n");
		return 0;
	}
	
	len_para = strlen(para);
	if(!(args + len_args > para + len_para))
	{
		printf("too few parameters!\n");
		return 0;
	}

	expression = strtok(NULL, " ");
		
	read_addr = eval(expression, &addr_success);

	if(strcmp(para, "s")==0)
	{
		printf("0x%x:",read_addr);
		do
		{
			ch = swaddr_read(read_addr+i,1,R_DS);
			printf("%c",ch);
			i++;
		}while(ch != 0);
		printf("\n");
		return 0;
	}

	len = eval(para, &para_success);
	if(!(para_success && addr_success))
	{
		return 0;
	}

	for(i=0;i<len;i++)
	{
		uint32_t value = swaddr_read(read_addr+4*i,4, R_DS);
		printf("$0x%0x:\t0x%0x\t%u\n",read_addr+4*i,value,value);
	}
		
	return 0;
}

static int cmd_p(char *args)
{
	int result = 0;
	bool is_success = true;
	result = eval(args,&is_success);
	if(is_success)
		printf("%d\n",result);
	else
		printf("Invalid Expression!\n");
	return 0;
}

static int cmd_w(char *args)
{
	int value;
	size_t len;
	WP *wp;
	bool is_valid;
	value = eval(args, &is_valid);
	if(is_valid)
	{
		wp = new_wp();
		len = strlen(args);
		if(len <= WP_EXPR-1)
		{
			strncpy(wp->expr, args, len);
			wp->value = value;
		}
		else
			printf("Expression is too long!\n");
	}
	return 0;
}

static int cmd_d(char *args)
{
	int no;
	bool is_valid;
	no = eval(args,&is_valid);
	if(is_valid)
		free_wp(find_wp(no));
	return 0;
}

static int cmd_bt(char *args)
{
	int no = 0;
	swaddr_t now_ebp, prev_ebp, ret_addr;
	char func_name[20];
	if(args != NULL)
	{
		printf("Invalid usage!\n");
		return 0;
	}
	prev_ebp = cpu.ebp;
	ret_addr = cpu.eip;
	while(find_func(ret_addr, func_name))
	{
		now_ebp = prev_ebp;
		if(now_ebp >= (1 << (10 + 10 + 3 +(27 - 10 - 10 - 3))))
		{
			printf("No stack!\n");
			return 0;
		}
		printf("#%d\t0x%08x in %s\n", no++, ret_addr, func_name);
		prev_ebp = swaddr_read(now_ebp, 4, R_SS);
		ret_addr = swaddr_read(now_ebp + 4, 4, R_SS);
	}
	return 0;
}

static int cmd_cache(char *args)
{
	swaddr_t addr = 0xffffffff;
	bool is_success = false;
	addr = eval(args, &is_success);
	if(is_success)
		print_cache_info_by_addr(addr);
	return 0;
}

static int cmd_b(char *args)
{
	swaddr_t addr = 0xffffffff;
	bool is_success = false;
	addr = eval(args, &is_success);
	if(is_success)
	{
		stop_eip = addr;
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
