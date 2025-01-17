#include "trap.h"
#include "FLOAT.h"

FLOAT f(FLOAT x) { 
	/* f(x) = 1/(1+25x^2) */
	return F_div_F(int2F(1), int2F(1) + F_mul_int(F_mul_F(x, x), 25));
}

FLOAT computeT(int n, FLOAT a, FLOAT b, FLOAT (*fun)(FLOAT)) {
	int k;
	FLOAT s,h;
	h = F_div_int((b - a), n);
	s = F_div_int(fun(a) + fun(b), 2 );
	for(k = 1; k < n; k ++) {
		s += fun(a + F_mul_int(h, k));
	}
	s = F_mul_F(s, h);
	return s;
}

int main() {
	nemu_assert(Fabs(F_mul_F(f2F(3.4),f2F(4.2))-f2F(14.2799))<f2F(1e-4));
	nemu_assert(Fabs(F_div_F(f2F(1.0),f2F(6.25))-f2F(0.16))<f2F(1e-4));
	FLOAT a = computeT(10, f2F(-1.0), f2F(1.0), f);
	FLOAT ans = f2F(0.551222);

//	nemu_assert(Fabs(a - f2F(0.577682)) < f2F(1e-4));
	nemu_assert(Fabs(a - ans) < f2F(1e-4));
	HIT_GOOD_TRAP;
	return 0;
}
