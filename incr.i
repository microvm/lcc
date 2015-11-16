#line 1 "tst\incr.c"
void main() {}

void memchar() {
	char x, *p;

	&x, &p;
	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

void memint() {
	int x, *p;

	&x, &p;
	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

void regchar() {
	register char x, *p;

	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

void regint() {
	register int x, *p;

	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}
