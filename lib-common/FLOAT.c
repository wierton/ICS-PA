#include "FLOAT.h"

#define MSB(a) (((a)>>31)&0x1)
#define H(a) (((a)>>16)&0xffff)
#define L(a) ((a)&0xffff)
#define B(a,b) ((((a)&0xffff)<<16)|((b)&0xffff))

unsigned cmp(unsigned a[],unsigned b[])
{
	if(a[2]<b[2])
		return 0;
	if(a[2]==b[2] && a[1]<b[1])
		return 0;
	if(a[2]==b[2] && a[1]==b[1] && a[0]<b[0])
		return 0;
	return 1;
}

unsigned sub(unsigned a[],unsigned b[])
{
	if(!cmp(a,b))
		return 0;
	unsigned CF=0,i;
	for(i=0;i<3;i++)
	{
		if(a[i]>=b[i]+CF)
		{
			a[i]=a[i]-b[i]-CF;
			CF=0;
		}
		else
		{
			a[i]=65536+a[i]-(b[i]+CF);
			CF=1;
		}
	}
	return 1;
}

unsigned mul(unsigned a[],unsigned b[],unsigned c[])
{
	unsigned CF,t;
	t=a[0]*b[0];
	CF=H(t);
	c[0]=L(t);

	t=L(a[0]*b[1])+L(a[1]*b[0])+CF;
	CF=H(t);
	c[1]=L(t);

	t=H(a[0]*b[1])+H(a[1]*b[0])+L(a[1]*b[1])+CF;
	CF=H(t);
	c[2]=L(t);

	t=H(a[1]*b[1])+CF;
	c[3]=L(t);

	return 1;
}

unsigned div(unsigned a[],unsigned b[],unsigned c[])
{
	unsigned x=B(a[2],a[1]);
	unsigned y=B(b[1],b[0]);
	if(b[1] != 0)
	{
		unsigned CF=x%y,t=x/y;
		c[1]=L(t);
		a[2]=H(CF);
		a[1]=L(CF);
		c[0]=0;
		while(sub(a,b)) c[0]++;
	}
	else
	{
		unsigned CF=x%y,t=x/y;
		c[2]=H(t);
		c[1]=L(t);
		a[2]=H(CF);
		a[1]=L(CF);
		c[0]=0;
		while(sub(a,b)) c[0]++;
	}
	return 1;
}


FLOAT F_mul_F(FLOAT a, FLOAT b) {

	int sa=MSB(a);
	int sb=MSB(b);
	int neg=sa^sb;
	unsigned x[4],y[4],z[4];
	if(sa) a=~a+1;
	if(sb) b=~b+1;
	x[0]=L(a);x[1]=H(a);
	y[0]=L(b);y[1]=H(b);
	mul(x,y,z);
	if(neg)
		return ~B(z[2],z[1])+1;
	else
		return B(z[2],z[1]);
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	int sa=MSB(a);
	int sb=MSB(b);
	int neg=sa^sb;
	unsigned x[4],y[4],z[4];
	if(sa) a=~a+1;
	if(sb) b=~b+1;
	x[0]=0;   x[1]=L(a);x[2]=H(a);
	y[0]=L(b);y[1]=H(b);y[2]=0;
	div(x,y,z);
	if(neg)
		return ~B(z[1],z[0])+1;
	else
		return B(z[1],z[0]);
}

FLOAT f2F(float a) {
	int result;
	int uf = *((int*)(&a));
	int sign=(!!(uf&0x80000000));
	int frac=((uf&0x7fffff)|0x800000);
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

