#line 1 "tst\incr.c"
void main(void) {}

void memchar(void) {
	char x, *p;

	&x, &p;
	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

void memint(void) {
	int x, *p;

	&x, &p;
	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

void regchar(void) {
	register char x, *p;

	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

void regint(void) {
	register int x, *p;

	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}
