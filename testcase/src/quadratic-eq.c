#include "trap.h"
#include "FLOAT.h"

/* solve 0.5x^2 + 0.5x - 0.5 = 0 */

int main() {
	FLOAT a = f2F(0.5);
	nemu_assert(a == 0x00008000);
	FLOAT b = f2F(0.5);
	nemu_assert(b == 0x00008000);
	FLOAT c = f2F(-0.5);
	nemu_assert(c == ~0x00008000);
	
	nemu_assert(F_mul_int(a, 4) == 0x00020000);
	nemu_assert(F_mul_F(F_mul_int(a, 4), c) == ~0x00010000);
	FLOAT dt = F_mul_F(b, b) - F_mul_F(F_mul_int(a, 4), c);
	FLOAT sqrt_dt = sqrt(dt);
	nemu_assert(0);

	FLOAT x1 = F_div_F(-b + sqrt_dt, F_mul_int(a, 2));
	FLOAT x2 = F_div_F(-b - sqrt_dt, F_mul_int(a, 2));

	FLOAT x1_ans = f2F(0.618);
	FLOAT x2_ans = f2F(-1.618);

	nemu_assert(Fabs(x1_ans - x1) < f2F(1e-4));
	nemu_assert(Fabs(x2_ans - x2) < f2F(1e-4));

	HIT_GOOD_TRAP;
	return 0;
}
