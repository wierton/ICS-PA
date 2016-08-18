#include "cpu/exec/helper.h"
#include "cpu/exec/bit-func.h"

int anyEvenBit(int x) {
	int a=x|(x>>16);
	int b=a|(a>>8);
	int c=b|(b>>4);
	int d=c|(c>>2);
	return d&0x1;
}

int sign(int x) {
	return (x>>31)|(!!x);
}

int bitParity(int x) {
	int a=x^(x>>16);
	int b=a^(a>>8);
	int c=b^(b>>4);
	int d=c^(c>>2);
	int e=d^(d>>1);
	return e&0x1;
}

int subOK(int x, int y) {
	int a=x;
	int b=~y+1;
	int sum=a+b;
	int p=(~x&y&sum)|(x&~y&~sum);
	return !((p>>31)&0x1);
}

uint32_t addOK(uint32_t x, uint32_t y)
{
	uint32_t sum=x+y;
	uint32_t p=(sum < x || sum < y);
	return !p;
}
