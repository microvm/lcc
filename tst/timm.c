char c = 0, cmax = 127;
unsigned char uc = 0, ucmax = 255;
short s = 0, smax = 32767;
unsigned short us = 0, usmax = 65535;
int i = -1, imax = 2147483647;
unsigned int ui = 0, uimax = 4294967295;
long long l = 0, lbig = 2147483647;
unsigned long long ul = 0, ulbig = 4294967295;
float f = 256.0;
double d = 1024.0;
void *p = 0;

char ASGN(void) {
	char c = 127;
	unsigned char uc = 255;
	short s = 32767;
	unsigned short us = 65535;
	int i = 2147483647;
	unsigned int ui = 4294967295;
	long long l = 2147483647;
	unsigned long long ul = 4294967295;
	float f = 1.0;
	double d = 2.0;
	return 'a';
}

long CV(void) {
	char cc; unsigned char uc; short ss; unsigned short us; int ii; unsigned int ui; long ll; unsigned long ul; float f; double d; void *pp;
	cc = ucmax;
	cc = smax;
	cc = usmax;
	cc = imax;
	cc = uimax;
	cc = lbig;
	cc = ulbig;

	uc = cmax;
	uc = smax;
	uc = usmax;
	uc = imax;
	uc = uimax;
	uc = lbig;
	uc = ulbig;

	ss = cmax;
	ss = ucmax;
	ss = usmax;
	ss = imax;
	ss = uimax;
	ss = lbig;
	ss = ulbig;

	us = cmax;
	us = ucmax;
	us = smax;
	us = imax;
	us = uimax;
	us = lbig;
	us = ulbig;

	ii = cmax;
	ii = ucmax;
	ii = smax;
	ii = usmax;
	ii = uimax;
	ii = lbig;
	ii = ulbig;
	ii = f;
	ii = d;

	ui = cmax;
	ui = ucmax;
	ui = smax;
	ui = usmax;
	ui = imax;
	ui = lbig;
	ui = ulbig;

	ll = cmax;
	ll = ucmax;
	ll = smax;
	ll = usmax;
	ll = imax;
	ll = uimax;
	ll = ulbig;
	ll = f;
	ll = d;

	ul = cmax;
	ul = ucmax;
	ul = smax;
	ul = usmax;
	ul = imax;
	ul = uimax;
	ul = lbig;
	ul = (unsigned long)p;

	f = cmax;
	f = ucmax;
	f = smax;
	f = usmax;
	f = imax;
	f = uimax;
	f = lbig;
	f = ulbig;
	f = d;

	d = cmax;
	d = ucmax;
	d = smax;
	d = usmax;
	d = imax;
	d = uimax;
	d = lbig;
	d = ulbig;
	d = f;

	p = (void *)ucmax;
	p = (void *)usmax;
	p = (void *)uimax;
	p = (void *)ulbig;

	return 0;
}

short cmp(void) {
	if (c < 0);
	if (s < 0);
	if (i < 0);
	if (l < 0);
	if (f < 0);
	if (d < 0);

	if (c <= 0);
	if (s <= 0);
	if (i <= 0);
	if (l <= 0);
	if (f <= 0);
	if (d <= 0);

	if (c == 0);
	if (s == 0);
	if (i == 0);
	if (l == 0);
	if (f == 0);
	if (d == 0);

	if (c >= 0);
	if (s >= 0);
	if (i >= 0);
	if (l >= 0);
	if (f >= 0);
	if (d >= 0);

	if (c > 0);
	if (s > 0);
	if (i > 0);
	if (l > 0);
	if (f > 0);
	if (d > 0);

	if (c != 0);
	if (s != 0);
	if (i != 0);
	if (l != 0);
	if (f != 0);
	if (d != 0);

	if (c < 0 || s <= 0 || i == 0 || l >= 0 || f > 0 || d != 0);

	return 128;
}

unsigned char NEG(void) {
	char cc; short ss; int ii; long ll; float ff; double dd;
	cc = -cmax;
	ss = -smax;
	ii = -imax;
	ll = -lbig;
	ff = -f;
	dd = -d;

	return 0;
}

unsigned short bops(void) {

	return 0;
}

unsigned int arith(void) {

	return 0;
}

int main(int argc, char *argv[]) {
	ASGN();
	cmp();
	CV();
	NEG();

	return 0;
}
