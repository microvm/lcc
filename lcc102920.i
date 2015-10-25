#line 1 "tst\timm.c"
void hello(const char *name) {
    char *hello = "hello";
}

int main(int argc, const char *argv[]) {
    if (argc < 2)
        return 1;
    hello(argv[1]);
    return 0;
}

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
