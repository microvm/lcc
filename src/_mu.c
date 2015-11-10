//Copied from the top of mu.md
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
static void progbeg(int, char *[]);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);
static Node mugen(Node);

//helper funcs
//define a new mu type
static int def_type(Type, char *, char *);
//get mu name for type
static char *type_name(Type);
typedef struct types {
	Type type;
	char *name;
	struct types *next;
} *Types;

//get or create a const
static char *const_name(Type, char *);
typedef struct consts {
	Type type;
	char *name;
	char *val;
	struct consts *next;
} *Consts;

typedef struct locals {
	Symbol s;
	struct locals *next;
} *Locals;

static char* indent;

static Types types_head;
static Consts consts_head;
static Locals locals_head;

static Symbol intreg[32], fltreg[32];
static Symbol intregw, fltregw;

static Symbol g;

static int cseg;

//The following is copied from the last section of mu.md so that I can use autocomplete, etc when editing it
static void progbeg(int argc, char *argv[])
{
	parseflags(argc, argv);
	types_head = NULL;
	consts_head = NULL;
	print(".funcsig @void_func () -> ()\n\n");

	def_type(voidtype, "void", "void");

	//def_type(, "i1  ", "int<1>");
	def_type(chartype, "char", "int<8>");
	def_type(unsignedchar, "uchar", "int<8>");

	def_type(shorttype, "short", "int<16>");
	def_type(unsignedshort, "ushort", "int<16>");

	def_type(inttype, "int", "int<32>");
	def_type(unsignedtype, "uint", "int<32>");

	def_type(longtype, "long", "int<64>");
	def_type(unsignedlong, "ulong", "int<64>");
	def_type(longlong, "long", "int<64>");
	def_type(unsignedlonglong, "ulong", "int<64>");

	def_type(floattype, "float", "float");
	def_type(doubletype, "double", "double");
	def_type(longdouble, "double", "double");

	def_type(voidptype, "ptr_void", "uptr<@void>");
	def_type(charptype, "ptr_char", "uptr<@char>");
	def_type(funcptype, "ptr_func", "ufuncptr<@void_func>");

	print("\n");

	const_name(voidptype, "NULL");

	print("\n");

	for (size_t i = 0; i < 32; i++)
		intreg[i] = mkreg("i%d", i, 1, IREG);

	for (size_t i = 0; i < 32; i++)
		fltreg[i] = mkreg("f%d", i, 0, FREG);

	intregw = mkwildcard(intreg);
	fltregw = mkwildcard(fltreg);

	tmask[0] = tmask[1] = ~(unsigned)0;
	vmask[0] = vmask[1] = 0;
}
static void progend(void) {}

static void address(Symbol q, Symbol p, long n)
{
	print("//%s called\n", __FUNCTION__);
}

static void defaddress(Symbol p)
{
	print("//%s called\n", __FUNCTION__);
}
static void defconst(int suffix, int size, Value v)
{
	print("//%s called\n", __FUNCTION__);
}
static void defstring(int n, char *str)
{
	for (size_t i = 0; i < n; i++)
		const_name(chartype, stringf("%d", str[i]));
	print("\n");
	Type t = array(chartype, n, 1);
	char *type = type_name(t), *name = g->x.name;
	print("@%s = NEW <@%s>\n", name, type);
	print("@%s_iref0 = GETIREF <@%s> @%s\n", name, type, name);
	for (size_t i = 0; i < n; i++)
		print("STORE <@%s> @%s_iref%d @%s\n@%s_iref%d = SHIFTIREF <@%s @%s> @%s_iref%d @%s\n", type_name(chartype), name, i, const_name(chartype, stringf("%d", str[i])), name, i + 1, type, type_name(longtype), name, i, const_name(longtype, "1"), const_name(chartype, stringf("%d", str[i])));
}
static void defsymbol(Symbol p)
{
	if (p->generated && p->scope != LABELS)
		p->x.name = stringf("%s", p->name);
	else if (p->generated)
		p->x.name = stringf("L%s", p->name);
	else if (p->scope >= LOCAL)
		p->x.name = stringf("%d_%s", p->scope, p->name);
	else
		p->x.name = p->name;

	if (p->type && isscalar(p->type))
		const_name(p->type, p->name);
}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls)
{
	usedmask[0] = usedmask[1] = 0;
	freemask[0] = freemask[1] = ~(unsigned)0;
	for (size_t i = 0; caller[i]; i++)
		type_name(caller[i]->type);
	for (size_t i = 0; callee[i]; i++)
		type_name(callee[i]->type);
	print(".funcsig @%s_sig = ( ", f->name);
	for (size_t i = 0; callee[i]; i++)
		print("@%s ", type_name(caller[i]->type));
	print(") -> ( @%s )\n", type_name(f->type->u.f.proto[0]));
	print(".funcdef @%s VERSION %%v1 <@%s_sig> {\n", f->name, f->name);
	print("\t%%entry( ");
	for (size_t i = 0; callee[i]; i++)
		print("<@%s> %%%s ", type_name(caller[i]->type), caller[i]->name);
	print("):\n");
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
	emitcode();
	//TODO: unpin all pinned objects
	print("}\n\n");
}

static void import(Symbol p)
{
	print("//%s called\n", __FUNCTION__);
}
static void export(Symbol p)
{
	//TODO: implement this?
	print("//%s called\n", __FUNCTION__);
}
static void global(Symbol p)
{
	g = p;
	if (isptr(p->type))
		print("is ptr");
}
static void local(Symbol p)
{
	//TODO: print/store var decl here
	p->sclass = AUTO;
	offset = roundup(offset + p->type->size, p->type->align < 4 ? 4 : p->type->align);
	p->x.offset = -offset;
	p->x.name = p->name;

	//print("//%%%s <%s>\n", p->name, type_name(p->type));
}

static void segment(int n)
{
	print("//%s called (%d)\n", __FUNCTION__, n);
}

static void space(int n)
{
	print("//%s called\n", __FUNCTION__);
}

/*
 TODO: (list)
	0. add sym to store condition results
	1. remove branches after returns
	2. remove labels at end of functions or add default return val
	3. add RETV to all funcs that don't explicitly return (possibly sixed by 2)
 */
static Node mugen(Node forest) {
	return gen(forest);
}

//XInterface funcs
static Symbol rmap(int opk) {
	switch (optype(opk)) {
	case B:
	case P:
	case I:
	case U:
		return intregw;
	case F:
		return fltregw;
	default:
		return 0;
	}
}

static void blkfetch(int k, int off, int reg, int tmp)
{
	print("//%s called\n", __FUNCTION__);
}
static void blkstore(int k, int off, int reg, int tmp)
{
	print("//%s called\n", __FUNCTION__);
}
static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[])
{
	print("//%s called\n", __FUNCTION__);
}

static void emit2(Node p)
{
	Symbol s1, s2;
	Node k1, k2;
	switch (specific(p->op)) {
	//TODO: unpin stuff at func exit
	case ASGN + P:
		s1 = p->kids[0]->syms[0];
		s2 = p->kids[1]->syms[0];
		if (isarray(s2->type)) {
			Type t = s2->type;
			while (isarray(t))
				t = t->type;
			//TODO: if array is generated then copy it to new location
			char *tmp = stringf("%%%d_%s", genlabel(1), s1->x.name);
			print("\t\t%s = COMMINST @uvm.native.pin <@%s> @%s\n", tmp, type_name(t), s2->x.name);
			print("\t\t%%%s = PTRCAST <uptr<@%s> @%s> %s\n", s1->x.name, type_name(t), type_name(s1->type), tmp);
		} else {
			print("\t\t%%%s = PTRCAST <@%s @%s> %s\n", s1->x.name, type_name(s2->type), type_name(s1->type), s2->x.name);
		}
		break;
	default:
		print("//OP %d NOT RECOGNIZED\n", p->op);
		break;
	}
}
static void doarg(Node p)
{
	//print("//%s called\n", __FUNCTION__);
	mkactual(4, p->syms[0]->u.c.v.i);
}
static void target(Node p) {}
static void clobber(Node p) {}

//helper funcs
static int def_type(Type t, char *name, char *mu_t) {
	Types ts, n;
	Type ts_tmp, t_tmp;
	if (isptr(t)) {
		for (ts = types_head; ts; ts = ts->next) {
			n = ts;
			if (!isptr(ts->type)) continue;
			ts_tmp = ts->type;
			t_tmp = t;
			while (isptr(ts_tmp) && isptr(t_tmp)) {
				ts_tmp = ts_tmp->type;
				t_tmp = t_tmp->type;
			}
			if (ts_tmp->u.sym == t_tmp->u.sym)
				return -1;
		}
	} else {
		for (ts = types_head; ts; ts = ts->next) {
			n = ts;
			if (ts->type->u.sym == t->u.sym)
				return -1;
		}
	}

	ts = n;
	NEW0(n, PERM);
	if (ts)
		ts->next = n;
	else
		types_head = n;
	n->name = stringf("%s", name);
	n->type = t;
	print(".typedef @%s = %s\n", n->name, mu_t);
	return 0;
}

static char *type_name(Type t) {
	Types ts;
	Type ts_tmp, t_tmp;
	if (isptr(t)) {
		for (ts = types_head; ts; ts = ts->next) {
			if (!isptr(ts->type)) continue;
			ts_tmp = ts->type;
			t_tmp = t;
			while (isptr(ts_tmp) && isptr(t_tmp)) {
				ts_tmp = ts_tmp->type;
				t_tmp = t_tmp->type;
			}
			if (ts_tmp->u.sym == t_tmp->u.sym)
				return ts->name;
		}
		char *name = stringf("%s_%s", "ptr", type_name(t->type));
		def_type(t, name, stringf("uptr<@%s>", type_name(t->type)));
		return name;
	} else if (isarray(t)) {
		for (ts = types_head; ts; ts = ts->next)
			if (ts->type->u.sym == t->u.sym)
				return ts->name;
		char *name = stringf("arr_%s_%d", type_name(t->type), t->size);
		def_type(t, name, stringf("array< @%s %d >", type_name(t->type), t->size));
		return name;
	} else {
		for (ts = types_head; ts; ts = ts->next)
			if (ts->type->u.sym == t->u.sym)
				return ts->name;
	}
	assert(0);
	return NULL;
}

static char *const_name(Type t, char *val) {
	char *tn = type_name(t), *v = string(val);
	Consts c = consts_head, n;
	for (c = consts_head; c; c = c->next) {
		n = c;
		if (c->type == t && c->val == v)
			return c->name;
	}
	c = n;
	NEW0(n, PERM);
	if (consts_head)
		c->next = n;
	else
		consts_head = n;

	n->name = stringf("%s_%s", tn, v);
	n->val = v;
	n->type = t;
	print(".const @%s <@%s> = %s\n", n->name, tn, v);
	return n->name;
}