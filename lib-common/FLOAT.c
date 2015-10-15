#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	int frac = 0xffff&(((a&0xffff)*(b&0xffff))>>16);
	int num = (((a>>16)*(b>>16))<<16)&0xffff0000;
	return frac|num;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	return ((a/b)<<16);
}

FLOAT f2F(float a) {
	int result;
	int uf = *((int*)(&a));
	int sign=(!!(uf&0x80000000));
	int frac=((uf&0x7fffff)|0x800000);
//	int q=(uf&0x807fffff)|((j>>8)|0x800000);
	int power=((uf&0x7f800000)>>23)-127-23;
	if(power>=-16)
	{
		result = frac<<(power+16);
		if(sign)
		{
			result = ~result+1;
			return result;
		}
		else
			return result;
	}
	result = (frac>>(-16-power));
	if(sign)
	{
		result = ~result+1;
		return result;
	}
	else
		return result;
}

FLOAT Fabs(FLOAT a) {
	int sa = (!!(a&0x80000000));
	if(sa)
		return ~a+1;
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

