#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	long long t = a*b;
	int *p = (int*)&t;
	int x=*p;
	int y=*(p+1);
	return (y<<16)|((x>>16)&0xffff);
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	int abs_a=a,abs_b=b;
	int sa = (!!(a&0x80000000));
	int sb = (!!(b&0x80000000));
	if(sa)
		abs_a=~a+1;
	if(sb)
		abs_b=~b+1;
	int frac_a = abs_a & 0xffff;
	int frac_b = abs_b & 0xffff;
	int n_a = abs_a>>16;
	int n_b = abs_b>>16;
	int frac,num;
	if(n_a == 0)
	{
		if(sa^sb)
			return ~((frac_a<<16)/abs_b)+1;
		return (frac_a<<16)/abs_b;
	}
	if(n_b && frac_b)
	{
		int t=(n_a*65536/n_b)*frac_b;
		if(t>0)
			frac = (frac_a*65536/abs_b-(n_a*65536/n_b)*frac_b/abs_b);
		else
		{
			return (abs_a*1024/(abs_b>>6));
		}
		num = (n_a*65536/n_b);
	}
	else if(frac_b)
	{
		if(sa^sb)
			return ~((abs_a/frac_b)<<16)+1;
		return (abs_a/frac_b)<<16;
	}
	else if(n_b)
	{
		if(sa^sb)
			return ~(abs_a/n_b)+1;
		return abs_a/n_b;
	}
	if(sa^sb)
		return ~(frac+num)+1;
	return frac+num;
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

