#include "timm.h"

static char c = 0, cmax = 127;
static unsigned char uc = 0, ucmax = 255;
static short s = 0, smax = 32767;
static unsigned short us = 0, usmax = 65535;
static int i = -1, imax = 2147483647;
static unsigned int ui = 0, uimax = 4294967295;
static long long l = 0, lbig = 2147483647;
static unsigned long long ul = 0, ulbig = 4294967295;
static float f = 256.0;
static double d = 1024.0;

static char *cp = 0;
static void *p = 0;

Foo global_foo;

//char ASGN(void) {
//	char c = 127;
//	unsigned char uc = 255;
//	short s = 32767;
//	unsigned short us = 65535;
//	int i = 2147483647;
//	unsigned int ui = 4294967295;
//	long long l = 2147483647;
//	unsigned long long ul = 4294967295;
//	float f = 1.0;
//	double d = 2.0;
//	return 1;
//}

short B(Foo s, Bar u) {
	Foo f = { 7, 7, 7, 7, 7.0, 7.7 }, *fp = &f;
	int ia[7], (*ipa)[7] = &ia, i;

	fp->p = u.p;
	fp->i = s.i;

	(*ipa)[0] = 1;
	ia[0] = 1;
	(*ipa)[1] = 2;
	ia[1] = 2;
	(*ipa)[2] = 3;
	ia[2] = 3;
	(*ipa)[3] = 4;
	ia[3] = 4;
	global_foo.a[0] = 'h';
	global_foo.a[1] = 'i';
	global_foo.a[2] = '!';
	return 2;
}

//int CV(void) {
//	char cc; unsigned char uc; short ss; unsigned short us; int ii; unsigned int ui; long ll; unsigned long ul; float ff; double dd; int *ip;
//	cc = ucmax;
//	cc = smax;
//	cc = usmax;
//	cc = imax;
//	cc = uimax;
//	cc = lbig;
//	cc = ulbig;
//
//	uc = cmax;
//	uc = smax;
//	uc = usmax;
//	uc = imax;
//	uc = uimax;
//	uc = lbig;
//	uc = ulbig;
//
//	ss = cmax;
//	ss = ucmax;
//	ss = usmax;
//	ss = imax;
//	ss = uimax;
//	ss = lbig;
//	ss = ulbig;
//
//	us = cmax;
//	us = ucmax;
//	us = smax;
//	us = imax;
//	us = uimax;
//	us = lbig;
//	us = ulbig;
//
//	ii = cmax;
//	ii = ucmax;
//	ii = smax;
//	ii = usmax;
//	ii = uimax;
//	ii = lbig;
//	ii = ulbig;
//	ii = f;
//	ii = d;
//
//	ui = cmax;
//	ui = ucmax;
//	ui = smax;
//	ui = usmax;
//	ui = imax;
//	ui = lbig;
//	ui = ulbig;
//	ui = (unsigned int)cp;
//	ui = (unsigned int)p;
//
//	ll = cmax;
//	ll = ucmax;
//	ll = smax;
//	ll = usmax;
//	ll = imax;
//	ll = uimax;
//	ll = ulbig;
//	ll = f;
//	ll = d;
//
//	ul = cmax;
//	ul = ucmax;
//	ul = smax;
//	ul = usmax;
//	ul = imax;
//	ul = uimax;
//	ul = lbig;
//	ul = (unsigned long)cp;
//	ul = (unsigned long)p;
//
//	ff = cmax;
//	ff = ucmax;
//	ff = smax;
//	ff = usmax;
//	ff = imax;
//	ff = uimax;
//	ff = lbig;
//	ff = ulbig;
//	ff = d;
//
//	d = cmax;
//	d = ucmax;
//	d = smax;
//	d = usmax;
//	d = imax;
//	d = uimax;
//	d = lbig;
//	d = ulbig;
//	d = f;
//
//	ip = (int *)ucmax;
//	ip = (int *)usmax;
//	ip = (int *)uimax;
//	ip = (int *)ulbig;
//
//	return 4;
//}
//
//long NEG(void) {
//	char cc; short ss; int ii; long ll; float ff; double dd;
//	cc = -cmax;
//	ss = -smax;
//	ii = -imax;
//	ll = -lbig;
//	ff = -f;
//	dd = -d;
//
//	return 8;
//}
//
//unsigned char arith(void) {
//	int i = 1, resi;
//	unsigned int j = 2, resui;
//	long l = 3, resl;
//	unsigned long m = 4, resul;
//	float f = 0.3, resf;
//	double d = 0.7, resd;
//
//	resi = i + i;
//	resui = j + j;
//	resi -= i;
//	resui -= j;
//	resi *= i;
//	resui *= j;
//	resi /= i;
//	resui /= j;
//	resi %= i;
//	resui %= j;
//
//	resl = l + l;
//	resul = m + m;
//	resl -= l;
//	resul -= m;
//	resl *= l;
//	resul *= m;
//	resl /= l;
//	resul /= m;
//	resl %= l;
//	resul %= m;
//
//	resf = f + f;
//	resf -= f;
//	resf *= f;
//	resf /= f;
//
//	resd = d + d;
//	resd -= d;
//	resd *= d;
//	resd /= d;
//
//	return -1;
//}
//
//unsigned short bops(void) {
//	int i = 1, resi;
//	unsigned int j = 2, resui;
//	long l = 3, resl;
//	unsigned long m = 4, resul;
//
//	resi = ~i;
//	resui = ~j;
//	resi = i & resi;
//	resui = j & resui;
//	resi = i | resi;
//	resui = j | resui;
//	resi = i ^ resi;
//	resui = j ^ resui;
//	resi = i << 1;
//	resui = j << 1;
//	resi = i >> 1;
//	resui = j >> 1;
//
//	resl = ~l;
//	resul = ~m;
//	resl = l & resl;
//	resul = m & resul;
//	resl = l | resl;
//	resul = m | resul;
//	resl = l ^ resl;
//	resul = m ^ resul;
//	resl = l << 1;
//	resul = m << 1;
//	resl = l >> 1;
//	resul = m >> 1;
//
//	return -2;
//}
//
//unsigned int cmp(void) {
//	if (c < 0);
//	if (s < 0);
//	if (i < 0);
//	if (l < 0);
//	if (f < 0);
//	if (d < 0);
//
//	if (c <= 0);
//	if (s <= 0);
//	if (i <= 0);
//	if (l <= 0);
//	if (f <= 0);
//	if (d <= 0);
//
//	if (c == 0);
//	if (s == 0);
//	if (i == 0);
//	if (l == 0);
//	if (f == 0);
//	if (d == 0);
//
//	if (c >= 0);
//	if (s >= 0);
//	if (i >= 0);
//	if (l >= 0);
//	if (f >= 0);
//	if (d >= 0);
//
//	if (c > 0);
//	if (s > 0);
//	if (i > 0);
//	if (l > 0);
//	if (f > 0);
//	if (d > 0);
//
//	if (c != 0);
//	if (s != 0);
//	if (i != 0);
//	if (l != 0);
//	if (f != 0);
//	if (d != 0);
//
//	if (c < 0 || s <= 0 || i == 0 || l >= 0 || f > 0 || d != 0)
//		c = 't';
//	else
//		c = 'f';
//
//	if (false)
//		c = 'f';
//	else if (true)
//		c = 't';
//
//	return -4;
//}
//
//unsigned long misc(const char *a, const char b) {
//	Foo st = { 0, 0, 0, 0, 0, 0, NULL, { 't', 'e', 's', 't' } };
//	Bar un;
//	st.c = b;
//	st.s = 2;
//	st.i = 4;
//	st.l = 8;
//	st.f = 3.2;
//	st.d = 6.4;
//	st.p = alloc(sizeof(*st.p));
//	st.bit = true;
//	st.bits++;
//
//	un.i = true;
//	B(st, un);
//
//	switch (st.c) {
//	case -1:
//		break;
//	case 0:
//	case 2:
//	case 4:
//		memcpy(st.a, "even", 5);
//		break;
//	case 1:
//	case 3:
//	case 5:
//		memcpy(st.a, "odd", 4);
//		break;
//	default:
//		st.bit = false;
//		break;
//	}
//	return -64;
//}
//
//void *ptrs(Foo * fptr) {
//	char *cp, ca[] = { 0, 1, 2 }, **cpp = NULL;
//	short *sp, sa[] = { 3, 4, 5 };
//	int *ip, ia[] = { 6, 7, 8 };
//	long *lp, la[] = { 9, 10, 11 };
//	float *fp, fa[] = { 1.2, 1.3, 1.4 };
//	double *dp, da[] = { 1.5, 1.6, 1.7 };
//	void *vp;
//	cp = ca;
//	cpp = (char **)&sp;
//	sp = sa;
//	ip = ia;
//	lp = la;
//	fp = fa;
//	dp = da;
//	vp = p;
//
//	*cp = 'a';
//	*sp = 1;
//	*ip = -1;
//	*lp = 10;
//	*fp = 1.0;
//	*dp = -1.0;
//
//	sa[1] = 2;
//}
//
//int main(int argc, const char *argv[]) {
//	const char *prog_name = argv[0], *arg1 = argv[1];
//	unsigned long dif = arg1 - prog_name;
//
//	char cr;
//	unsigned char ucr;
//	int ir;
//	unsigned int uir;
//	long lr;
//	unsigned long ulr;
//	void *vpr;
//	void *(*ptrs_fptr)(Foo *) = NULL;
//
//	cr = ASGN();
//	uir = cmp();
//	ir = CV();
//	lr = NEG();
//	ucr = arith();
//	vpr = (*ptrs_fptr)(NULL);
//	ulr = misc(prog_name, *(arg1 + 1));
//
//	return 0;
//}
