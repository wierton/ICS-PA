#include "cpu/exec/helper.h"
#include "arith-bits.h"

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
