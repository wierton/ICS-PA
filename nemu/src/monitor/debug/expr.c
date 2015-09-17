#include "nemu.h"
#include <malloc.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, NUM, HEX, EQ, PLUS, MINUS, MUL, DIV, LPA, RPA

	/* TODO: Add more token types */

};


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

char cmp_operator(char a,char b)
{
	return cmp_table[swap_table(a)][swap_table(b)];
}

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"-", '-'},					//minus
	{"\\*", '*'},					//multiply
	{"/", '/'},					//divide
	{"\\(", '('},						//left parenthese
	{"\\)", ')'},						//right parethese
	{"0[xX][0-9a-fA-F]+", HEX},				//hex
	{"[0-9]+", NUM},						//number
	{"==", EQ}						// equal
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
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}


typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];

int nr_token;

struct EXPR {
	struct EXPR *next;
	int operand;
	char _operator;
} unit[64];

int pUnit;

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

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				printf("%s\n",substr_start);

				switch(rules[i].token_type) {
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
					case NUM:
							 unit[pUnit]._operator = '\0';
							 sscanf(substr_start,"%d", &(unit[pUnit++].operand));
							 break;
					case HEX:
							 unit[pUnit]._operator = '\0';
							 sscanf(substr_start + 2,"%d",&(unit[pUnit++].operand));
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
	if(!make_token(e)) {
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

	int i = 0;
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
	return result;
}

