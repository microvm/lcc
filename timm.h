#ifndef NULL
#define NULL 0
#endif

typedef struct foo {
	char c;
	short s;
	int i;
	long l;
	float f;
	double d;
	struct foo *p;
	char a[7];
	unsigned int bit : 1;
	unsigned int bits : 3;
} Foo;

typedef struct funcs {
	char (*cp)(char);
	short (*sp)(short);
	int (*ip)(int);
	long (*lp)(long);
	float (*fp)(float);
	double (*dp)(double);
	unsigned int (*ops[7])(int, int);
	void *(*meta)(void(*)());
} Funcs;

typedef union bar {
	char c;
	short s;
	int i;
	long l;
	float f;
	double d;
	void *p;
	Foo foo;
} Bar;

enum Boolean {
	false,
	true,
	maybe
};

typedef unsigned long long size_t;

void *alloc(size_t);
void *memcpy(void *, const void*, size_t);

char ASGN(void);
short B(Foo s, Bar u);
int CV(void);
long NEG(void);

unsigned char arith(void);
unsigned short bops(void);
unsigned int cmp(void);
unsigned long misc(const char *, const char);
void *ptrs(Foo *);
