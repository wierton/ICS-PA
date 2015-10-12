#include "nemu.h"
#include <malloc.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, REG, NUM, HEX, EQ, NEQ, AND, OR, NOR, GEQ, LEQ

	/* TODO: Add more token types */

};

int priority(int _operator)
{
	switch(_operator)
	{
		case '+':return 4;
		case '-':return 4;
		case '*':return 3;
		case '/':return 3;
		case '(':return 0;
		case ')':return 1000;
		case '>':return 6;
		case '<':return 6;
		case GEQ:return 6;
		case LEQ:return 6;
		case EQ :return 7;
		case NEQ:return 7;
		case AND:return 11;
		case OR :return 12;
		case NOR:return 2;
		case '@':return 2;
		case '#':return 2000;
		default:return -1;
	}
}

char cmp_operator(int a, int b)
{
	if(a == '(' && b == ')')
		return '=';
	if(a == ')' && b == '(')
		return '$';
	if(a == '(' && b == '#')
		return '$';
	if(a == '#' && b == '#')
		return '=';
	if(a == '(' || b == '(')
		return '<';
	if(a == ')' || b == ')')
		return '>';
	if(a == b)
		return '>';
	if(priority(a) > priority(b))
		return '<';
	if(priority(a) < priority(b))
		return '>';
	if(priority(a) == priority(b))
		return '>';
	return '=';
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

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE},				// spaces
	{"\t+", NOTYPE},
	{"\\+", '+'},					// plus
	{"-", '-'},					//minus
	{"\\*", '*'},					//multiply
	{"/", '/'},					//divide
	{"\\(", '('},						//left parenthese
	{"\\)", ')'},						//right parethese
	{"0[xX][0-9a-fA-F]+", HEX},				//hex
	{"[0-9]+", NUM},						//number
	{"[%\\$][a-zA-Z]{3}", REG},
	{"==", EQ},						// equal
	{"!=", NEQ},
	{"&&", AND},
	{"\\|\\|",OR},
	{"\\!", NOR}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];

int nr_token;

#define UnitNum 64

struct EXPR {
	struct EXPR *next;
	int operand;
	int _operator;
} unit[UnitNum];

int pUnit;

void showlink(struct EXPR *start,int pStart)
{
    while(start != NULL)
    {
	    if(start->_operator != 0)
		{
			switch(start->_operator)
			{
				case EQ :printf("== ");break;
				case NEQ:printf("!= ");break;
				case AND:printf("&& ");break;
				case OR :printf("|| ");break;
				case NOR:printf("! ") ;break;
				default :printf("%c ",start->_operator);break;
			}
		}
		else
			printf("%d ",start->operand);
		start = start->next;
	}
	printf("\n");
}

void showarray(struct EXPR *start,int pStart)
{
	int i;
	for(i=0;i<pStart;i++)
	{
		if(start[i]._operator != 0)
		{
			switch(start[i]._operator)
			{
				case EQ :printf("== ");break;
				case NEQ:printf("!= ");break;
				case AND:printf("&& ");break;
				case OR :printf("|| ");break;
				case NOR:printf("! ") ;break;
				default :printf("%c ",start[i]._operator);break;	 
			}
		}
		else
			printf("%d ",start[i].operand);
	}
	printf("\n");
}

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	pUnit = 0;
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				assert(pUnit >= 0 && pUnit <= UnitNum);
				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				//printf("%s\n",substr_start);
				char reg[4];
				reg[3] = 0;

				if(pUnit > UnitNum)
				{
					printf("The expression is too long!\n");
					return false;
				}

				switch(rules[i].token_type) {
					case NOTYPE:break;
					case '+':unit[pUnit++]._operator = '+';break;
					case '-':unit[pUnit++]._operator = '-';break;
					case '*':
							 if(pUnit == 0 || (unit[pUnit-1]._operator != 0 && unit[pUnit-1]._operator != ')'))
							 {
								 unit[pUnit++]._operator = '@';
								 break;
							 }
							 else
								 unit[pUnit++]._operator = '*';break;
					case '/':unit[pUnit++]._operator = '/';break;
					case '(':unit[pUnit++]._operator = '(';break;
					case ')':unit[pUnit++]._operator = ')';break;
					case EQ :unit[pUnit++]._operator = EQ ;break;
					case NEQ:unit[pUnit++]._operator = NEQ;break;
					case AND:unit[pUnit++]._operator = AND;break;
					case OR :unit[pUnit++]._operator = OR ;break;
					case NOR:unit[pUnit++]._operator = NOR;break;
					case NUM:
							 unit[pUnit]._operator = '\0';
							 sscanf(substr_start,"%d", &(unit[pUnit++].operand));
							 break;
					case HEX:
							 unit[pUnit]._operator = '\0';
							 sscanf(substr_start + 2,"%x",&(unit[pUnit++].operand));
							 break;
					case REG:
						unit[pUnit]._operator = '\0';
						strncpy(reg,substr_start+1,3);
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
							printf("Warning:%s,no such reg!\n",reg);
						//printf("%s:%d\n",reg,unit[pUnit].operand);
						break;
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	unit[pUnit++]._operator = '#';

	return true; 
}

uint32_t eval(char *e, bool *success) {
	init_regex();
	if(e == NULL || !make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");

	bool is_valid = true;
	int result = 0;	
	struct EXPR *LinearTable = (struct EXPR*)malloc(pUnit*sizeof(struct EXPR));
	memset(LinearTable,0,pUnit*sizeof(struct EXPR));
	int pLinearTable = 0;
	struct EXPR *Stack = (struct EXPR*)malloc(pUnit*sizeof(struct EXPR));
	memset(Stack,0,pUnit*sizeof(struct EXPR));
	int pStack = 0;
	Stack[pStack++]._operator = '#';

	//showarray(unit,pUnit);
	int i = 0;
	while(is_valid && (Stack[pStack-1]._operator != '#' || unit[i]._operator != '#'))
	{
		//showarray(LinearTable,pLinearTable);
		//showarray(Stack,pStack);
		if(unit[i]._operator != 0)
		{
			assert(pStack > 0);
			if(cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '$')
			{
				if(Stack[pStack-1]._operator == '(' && unit[i]._operator == '#')
				{
					pStack--;
					if(pStack >0 && Stack[pStack-1]._operator == '@')
					{
						LinearTable[pLinearTable++]._operator = Stack[--pStack]._operator;
					}
				}
				else
					is_valid = false;
				break;
			}
			while(pStack != 0 && cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '>')
			{
				LinearTable[pLinearTable++]._operator = Stack[--pStack]._operator;
				//showarray(LinearTable,pLinearTable);
				//showarray(Stack,pStack);
			}
			if(cmp_operator(Stack[pStack-1]._operator,unit[i]._operator) == '=')
			{
				if(Stack[pStack-1]._operator == '(' && unit[i]._operator == ')')
					pStack--;
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
		//printf("2:");
		//showlink(LinearTable,0);
		if(LinearTable[0].next->_operator == '@' || LinearTable[0].next->_operator == NOR)
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
		{
			is_valid = false;
			break;
		}

		while(pOperator != NULL && pOperator->_operator == 0)
		{
			operand_1 = operand_1->next;
			operand_2 = operand_2->next;
			pOperator = pOperator->next;
		}
		assert(operand_1 != NULL && operand_2 != NULL && pOperator != NULL);
		assert(operand_1->_operator == 0 && operand_2->_operator == 0 && pOperator->_operator != 0);
		switch(pOperator->_operator)
		{
		case NOR:
			operand_2->operand = !operand_2->operand;
			operand_2->next = pOperator->next;
			break;
		case '@':
			//printf("%u\n",(uint32_t)(operand_2->operand));
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
		case EQ:
			operand_1->operand = operand_1->operand == operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		case NEQ:
			operand_1->operand = operand_1->operand != operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		case AND:
			operand_1->operand = operand_1->operand && operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		case OR:
			operand_1->operand = operand_1->operand || operand_2->operand;
			operand_1->next = pOperator->next;
			break;
		}
		operand_1 = &LinearTable[0];
	}

	if(is_valid && operand_1->_operator == 0)
	{
		*success = true;
		result = operand_1->operand;
	}
	else
	{
		*success = false;
		result = -1;
	}
	//printf("%d\n",result);
	free(LinearTable);
	free(Stack);
	return result;
}

