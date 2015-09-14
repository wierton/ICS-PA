#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

char cmp_table[][8] =
{
	{'>','>','<','<','<','>','<','>'},
	{'>','>','<','<','<','>','<','>'},
	{'>','>','>','>','<','>','<','>'},
	{'>','>','>','>','<','>','<','>'},
	{'<','<','<','<','<','=','<','$'},
	{'>','>','>','>','$','>','<','>'},
	{'>','>','>','>','<','$','=','>'},
	{'<','<','<','<','<','$','<','='}
};

int swap_table(char c)
{
	switch(c)
	{
		case '+':return 0;
		case '-':return 1;
		case '*':return 2;
		case '/':return 3;
		case '(':return 4;
		case ')':return 5;
		case '@':return 6;
		case '#':return 7;
	}
	return -1;
}

void strupr(char *args)
{
	int i,len = strlen(args);
	for(i=0;i<len;i++)
	{
		if(args[i]>='a' && args[i]<='z')
		{
			args[i] = args[i]-'a'+'A';
		}
	}
}

struct EXPR 
{
	struct EXPR *next;
	uint32_t operand;
	char _operator;
};

char cmp_operator(char a,char b)
{
	return cmp_table[swap_table(a)][swap_table(b)];
}


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

static int calc(char *args);

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
	{ "p", "Calculate the expression",cmd_p}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

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
	if(args == NULL)
		steps=1;
	else
		sscanf(args,"%u",&steps);
	cpu_exec(steps);
	return 0;
}

static int cmd_info(char *args)
{

	if(strcmp(args,"r")==0)
	{
		printf("eax\t0x%04x\t%d\necx\t0x%04x\t%d\nedx\t0x%04x\t%d\nebx\t0x%04x\t%d\nesp\t0x%04x\t0x%04x\nebp\t0x%04x\t0x%04x\nesi\t0x%04x\t%d\nedi\t0x%04x\t%d\neip\t0x%04x\t0x%04x\n",cpu.eax,cpu.eax,cpu.ecx,cpu.ecx,cpu.edx,cpu.edx,cpu.ebx,cpu.ebx,cpu.esp,cpu.esp,cpu.ebp,cpu.ebp,cpu.esi,cpu.esi,cpu.edi,cpu.edi,cpu.eip,cpu.eip);
	}
	if(strcmp(args,"w")==0)
	{
/*		WP *p = head;
		while(p != NULL)
		{
			printf("%d\n",p->NO);
			p = p->next;
		}
*/	}
	return 0;
}

static int cmd_x(char *args)
{
	char xar;
	size_t len;
	uint32_t read_addr;
	char *para = args;

	if(*para == 'x' || *para == 'X')
	{
		sscanf(para+1,"0x%x",&read_addr);
		printf("0x%0x\t0x%0x\n",read_addr,swaddr_read(read_addr,4));
	}
	else if(*para == 's' || *para == 'S')
	{
//		char ch;
		sscanf(para+1,"0x%x",&read_addr);
		printf("0x%0x\t0x%0x\n",read_addr,swaddr_read(read_addr,1));
	}
	else if(*para == 'c' || *para == 'C')
	{
	}
	else if(*para >= '0' && *para <= '9')
	{
		sscanf(para,"%u%c0x%x",&len,&xar,&read_addr);
//		printf("%u :%c: %x",len,xar,read_addr);
		if(xar == 'x' || xar == 'X')
		{}
		else if(xar == 's' || xar == 'S')
		{

		}
		else if(xar == 'c' || xar == 'C')
		{}
		else if(xar == ' ' || xar == '\t')
		{
			int i;
			for(i=0;i<len;i++)
				printf("0x%0x\t0x%0x\n",read_addr+4*i,swaddr_read(read_addr+4*i,4));
		}
	}
	else
		printf("%s\n","Unknown parameter!");
	return 0;
}

static int cmd_p(char *args)
{
	int result = calc(args);
	if(result == -1)
		printf("Invalid Expression!\n");
	else
		printf("%d\n",result);
	return 0;
}

static int calc(char *args)
{
	bool is_innum = false,is_valid = true;
	char *sz_p = args,reg[4];
	int i,pUnit = 0,result = 0,len = strlen(args);
	args[len] = '#';
	args[len+1] = '\0';
	len++;
	struct EXPR *unit = (struct EXPR*)malloc(len*sizeof(struct EXPR));
	memset(unit,0,len*sizeof(struct EXPR));

	for(i=0;i<len && is_valid;i++)
	{
		switch(*sz_p)
		{
		case '+':
		case '-':
		case '*':
			if(pUnit == 0 || (unit[pUnit-1]._operator != 0 && unit[pUnit-1]._operator != ')'))
			{
				unit[pUnit++]._operator = '@';
				break;
			}
		case '/':
		case '(':
		case ')':
		case '#':
			is_innum = false;
			sscanf(sz_p,"%c",&unit[pUnit++]._operator);
			break;
		case '%':
			strncpy(reg,sz_p+1,3);
			reg[3] = 0;
			strupr(reg);
			if(strcmp(reg,"EAX") == 0)
				unit[pUnit++].operand = cpu.eax;
			else if(strcmp(reg,"EBX") == 0)
				unit[pUnit++].operand = cpu.ebx;
			else if(strcmp(reg,"ECX") == 0)
				unit[pUnit++].operand = cpu.ecx;
			else if(strcmp(reg,"EDX") == 0)
				unit[pUnit++].operand = cpu.edx;
			else if(strcmp(reg,"ESI") == 0)
				unit[pUnit++].operand = cpu.esi;
			else if(strcmp(reg,"EDI") == 0)
				unit[pUnit++].operand = cpu.edi;
			else if(strcmp(reg,"ESP") == 0)
				unit[pUnit++].operand = cpu.esp;
			else if(strcmp(reg,"EBP") == 0)
				unit[pUnit++].operand = cpu.ebp;
			else if(strcmp(reg,"EIP") == 0)
				unit[pUnit++].operand = cpu.eip;
			else
				is_valid = false;
			i+=3;
			sz_p+=3;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if(!is_innum)
			{
				is_innum = true;
				sscanf(sz_p,"%d",&unit[pUnit++].operand);
			}
			break;
		default:is_valid = false;break;
		}
		sz_p++;
	}

	struct EXPR *LinearTable = (struct EXPR*)malloc(pUnit*sizeof(struct EXPR));
	memset(LinearTable,0,pUnit*sizeof(struct EXPR));
	int pLinearTable = 0;
	struct EXPR *Stack = (struct EXPR*)malloc(pUnit*sizeof(struct EXPR));
	memset(Stack,0,pUnit*sizeof(struct EXPR));
	int pStack = 0;
	Stack[pStack++]._operator = '#';

	i = 0;
	while(is_valid && (Stack[pStack-1]._operator != '#' || unit[i]._operator != '#'))
	{
		//show(LinearTable,pLinearTable);
		//show(Stack,pStack);
		if(unit[i]._operator != 0)
		{
			if(cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '$')
			{
				is_valid = false;
				break;
			}
			while(pStack != 0 && cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '>')
			{
				LinearTable[pLinearTable++]._operator = Stack[--pStack]._operator;
			}
			if(cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '=')
			{
				if(Stack[pStack-1]._operator == '(' && unit[i]._operator == ')')
				{
					pStack--;
					if(pStack >0 && Stack[pStack-1]._operator == '@')
					{
						LinearTable[pLinearTable++]._operator = Stack[--pStack]._operator;
					}
				}
			}
			if(cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '<')
			{
				Stack[pStack++]._operator = unit[i]._operator;
			}
		}
		else
		{
			LinearTable[pLinearTable++].operand = unit[i].operand;
		}
		if(unit[i]._operator != '#')
			i++;
	}

	for(i=0;i<pLinearTable-1 && is_valid;i++)
	{
		LinearTable[i].next = &LinearTable[i+1];
	}
	LinearTable[i].next = NULL;

	struct EXPR *operand_1 = &LinearTable[0],*operand_2 = NULL,*pOperator = NULL;
	while(is_valid && operand_1->next != NULL)
	{
		//printf("%d %d %c\n",operand_1->operand,operand_2->operand,pOperator->_operator);
		//show(LinearTable,0);
		if(LinearTable[0].next->_operator == '@')
		{
			//printf("is in @\n");
			operand_2 = &LinearTable[0];
			pOperator = LinearTable[0].next;
		}
		else if(LinearTable[0].next->next != NULL)
		{
			operand_2 = LinearTable[0].next;
			pOperator = LinearTable[0].next->next;
		}
		else
			is_valid = false;

		while(pOperator != NULL && pOperator->_operator == 0)
		{
			operand_1 = operand_1->next;
			operand_2 = operand_2->next;
			pOperator = pOperator->next;
		}

		switch(pOperator->_operator)
		{
		case '@':
			operand_2->operand = swaddr_read(((uint32_t)(operand_2->operand)),4);
			operand_2->next = pOperator->next;
			break;
		case '+':
			operand_1->operand = operand_1->operand + operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		case '-':
			operand_1->operand = operand_1->operand - operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		case '*':
			operand_1->operand = operand_1->operand * operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		case '/':
			operand_1->operand = operand_1->operand / operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		}
		operand_1 = &LinearTable[0];
	}

	if(is_valid)
		result = operand_1->operand;
	else
		result = -1;
	free(LinearTable);
	free(Stack);
	free(unit);
	return result;
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
