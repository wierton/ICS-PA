#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "trap.h"

typedef int FLOAT;

static inline int F2int(FLOAT a) {
	int sa = (!!(a&0x80000000));
	if(sa)
		return ~((~a)>>16)+1;
	return a>>16;
}

static inline FLOAT int2F(int a) {
	int sa = (!!(a&0x80000000));
	if(sa)
		return ~((~a+1)<<16);
	return a<<16;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
	int sa = (!!(a&0x80000000));
	int sb = (!!(a&0x80000000));
	if(sa && !sb)
		return ~(((~a)*b)>>16);
	if(!sa && sb)
		return ~((a*(~b+1))>>16);
	if(sa && sb)
		return (((~a)*(~b+1))>>16);
	return ((a*b)>>16);
}

static inline FLOAT F_div_int(FLOAT a, int b) {
	int sa = (!!(a&0x80000000));
	int sb = (!!(a&0x80000000));
	if(sa && !sb)
		return ~((~a)/b);
	if(!sa && sb)
		return ~(a/(~b+1));
	if(sa && sb)
		return ((~a)/(~b+1));
	return (a/b);

}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT sqrt(FLOAT);
FLOAT pow(FLOAT, FLOAT);

#endif
