#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	int t = a*b;
	return t>>16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	return (a/b)<<16;
}

FLOAT f2F(float a) {
	int uf = *((int*)(&a));
	int sign=(!!(uf&0x80000000));
	int frac=((uf&0x7fffff)|0x800000);
//	int q=(uf&0x807fffff)|((j>>8)|0x800000);
	int power=((uf&0x7f800000)>>23)-127-23;
	if(power>=-16)
	{
		if(sign)
			return ~(frac<<(power+16));
		else
			return frac<<(power+16);
	}
	if(sign)
		return ~(frac>>(-16-power));
	else
		return frac>>(-16-power);
}

FLOAT Fabs(FLOAT a) {
	int s=(a>>31);
	if(s)
		return !a;
	return a;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

