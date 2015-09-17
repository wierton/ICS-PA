#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, NUM, HEX, EQ, PLUS, MINUS, MUL, DIV, LPA, RPA

	/* TODO: Add more token types */

};

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
	struct token *next;
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
	return 0;
}

