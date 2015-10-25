//Copied from the top of mu.md
enum { EAX = 0, ECX = 1, EDX = 2, EBX = 3, ESI = 6, EDI = 7 };
#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)
static void address(Symbol, Symbol, long);
static void blkfetch(int, int, int, int);
static void blkloop(int, int, int, int, int, int[]);
static void blkstore(int, int, int, int);
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static void doarg(Node);
static void emit2(Node);
static void export(Symbol);
static void clobber(Node);
static void function(Symbol, Symbol[], Symbol[], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char **);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);
static Symbol charreg[32], shortreg[32], intreg[32];
static Symbol fltreg[32];

static Symbol charregw, shortregw, intregw, fltregw;

static int cseg;

static Symbol quo, rem;

//The following is copied from the last section of mu.md so that I can use autocomplete, etc when editing it
static void progbeg(int argc, char *argv[])
{
	parseflags(argc, argv);
	print("%s called\n", __FUNCTION__);

	intreg[EAX] = mkreg("eax", EAX, 1, IREG);
	intreg[EDX] = mkreg("edx", EDX, 1, IREG);
	intreg[ECX] = mkreg("ecx", ECX, 1, IREG);
	intreg[EBX] = mkreg("ebx", EBX, 1, IREG);
	intreg[ESI] = mkreg("esi", ESI, 1, IREG);
	intreg[EDI] = mkreg("edi", EDI, 1, IREG);

	shortreg[EAX] = mkreg("ax", EAX, 1, IREG);
	shortreg[ECX] = mkreg("cx", ECX, 1, IREG);
	shortreg[EDX] = mkreg("dx", EDX, 1, IREG);
	shortreg[EBX] = mkreg("bx", EBX, 1, IREG);
	shortreg[ESI] = mkreg("si", ESI, 1, IREG);
	shortreg[EDI] = mkreg("di", EDI, 1, IREG);

	charreg[EAX] = mkreg("al", EAX, 1, IREG);
	charreg[ECX] = mkreg("cl", ECX, 1, IREG);
	charreg[EDX] = mkreg("dl", EDX, 1, IREG);
	charreg[EBX] = mkreg("bl", EBX, 1, IREG);
	for (size_t i = 0; i < 8; i++)
		fltreg[i] = mkreg("%d", i, 0, FREG);
	charregw = mkwildcard(charreg);
	shortregw = mkwildcard(shortreg);
	intregw = mkwildcard(intreg);
	fltregw = mkwildcard(fltreg);

	tmask[IREG] = (1 << EDI) | (1 << ESI) | (1 << EBX)
		| (1 << EDX) | (1 << ECX) | (1 << EAX);
	vmask[IREG] = 0;
	tmask[FREG] = 0xff;
	vmask[FREG] = 0;
}
static void progend(void)
{
	print("%s called\n", __FUNCTION__);
}

static void address(Symbol q, Symbol p, long n)
{
	print("%s called\n", __FUNCTION__);
}

static void defaddress(Symbol p)
{
	print("%s called\n", __FUNCTION__);
}
static void defconst(int suffix, int size, Value v)
{
	print("%s called\n", __FUNCTION__);
}
static void defstring(int n, char *str)
{
	print("%s called\n", __FUNCTION__);
}
static void defsymbol(Symbol p)
{
	print("%s called\n", __FUNCTION__);
	if (p->scope >= LOCAL && p->sclass == STATIC)
		p->x.name = stringf("L%d", genlabel(1));
	else if (p->generated)
		p->x.name = stringf("L%s", p->name);
	else if (p->scope == GLOBAL || p->sclass == EXTERN)
		p->x.name = stringf("_%s", p->name);
	else if (p->scope == CONSTANTS && (isint(p->type) || isptr(p->type)) && p->name[0] == '0' && p->name[1] == 'x')
		p->x.name = stringf("0%sH", &p->name[2]);
	else
		p->x.name = p->name;
}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls)
{
	usedmask[0] = usedmask[1] = 0;
	freemask[0] = freemask[1] = ~(unsigned)0;
	print("%s called\n", __FUNCTION__);
	print("%s:\n", f->name);
	for (size_t i = 0; callee[i]; i++)
		print("\t%s->%s\n", caller[i]->name, callee[i]->name);
	offset = 16 + 4;
	for (size_t i = 0; callee[i]; i++) {
		Symbol p = callee[i];
		Symbol q = caller[i];
		p->x.offset = q->x.offset = offset;
		p->x.name = q->x.name = stringf("%d", p->x.offset);
		p->sclass = q->sclass = AUTO;
		offset += roundup(q->type->size, 4);
	}
	offset = maxoffset = 0;
	gencode(caller, callee);
	fflush(stdout);
	emitcode();
	fflush(stdout);
}

static void import(Symbol p)
{
	print("%s called\n", __FUNCTION__);
}
static void export(Symbol p)
{
	print("%s called\n", __FUNCTION__);
}
static void global(Symbol p)
{
	print("%s called\n", __FUNCTION__);
}
static void local(Symbol p)
{
	print("%s called\n", __FUNCTION__);
	p->sclass = AUTO;
	offset = roundup(offset + p->type->size, p->type->align < 4 ? 4 : p->type->align);
	p->x.offset = -offset;
	p->x.name = stringd(-offset);
}

static void segment(int n)
{
	print("%s called\n", __FUNCTION__);
}

static void space(int n)
{
	print("%s called\n", __FUNCTION__);
}

//XInterface funcs
static Symbol rmap(int opk) {
	switch (optype(opk)) {
	case B: case P:
		return intregw;
	case I: case U:
		if (opsize(opk) == 1)
			return charregw;
		else if (opsize(opk) == 2)
			return shortregw;
		else
			return intregw;
	case F:
		return fltregw;
	default:
		return 0;
	}
}

static void blkfetch(int k, int off, int reg, int tmp)
{
	print("%s called\n", __FUNCTION__);
}
static void blkstore(int k, int off, int reg, int tmp)
{
	print("%s called\n", __FUNCTION__);
}
static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[])
{
	print("%s called\n", __FUNCTION__);
}

static void emit2(Node p)
{
	print("%s called\n", __FUNCTION__);
}
static void doarg(Node p)
{
	print("%s called\n", __FUNCTION__);
	mkactual(4, p->syms[0]->u.c.v.i);
}
static void target(Node p)
{
	print("%s called\n", __FUNCTION__);
}
static void clobber(Node p)
{
	print("%s called\n", __FUNCTION__);
	static int nstack = 0;

	assert(p);
	nstack = ckstack(p, nstack);
	switch (specific(p->op)) {
	case RSH + I: case RSH + U: case LSH + I: case LSH + U:
		if (generic(p->kids[1]->op) != CNST &&
			 !(generic(p->kids[1]->op) == INDIR &&
				specific(p->kids[1]->kids[0]->op) == VREG + P &&
				p->kids[1]->syms[RX]->u.t.cse &&
				generic(p->kids[1]->syms[RX]->u.t.cse->op) == CNST)) {
			spill(1 << ECX, 1, p);
		}
		break;
	case ASGN + B: case ARG + B:
		spill(1 << ECX | 1 << ESI | 1 << EDI, IREG, p);
		break;
	case EQ + F: case LE + F: case GE + F: case LT + F: case GT + F: case NE + F:
		spill(1 << EAX, IREG, p);
		if (specific(p->op) == EQ + F)
			p->syms[1] = findlabel(genlabel(1));
		break;
	case CALL + F:
		spill(1 << EDX | 1 << EAX | 1 << ECX, IREG, p);
		break;
	case CALL + I: case CALL + U: case CALL + P: case CALL + V:
		spill(1 << EDX | 1 << ECX, IREG, p);
		break;
	}
}