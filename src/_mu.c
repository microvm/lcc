#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)

//%TOP_START
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
static Symbol rmap(int);

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

static Types types_head;
static Consts consts_head;

static Symbol intreg[32], fltreg[32];
static Symbol intregw, fltregw;

static Symbol g;

typedef struct args {
	char *arg;
	struct args *next;
} *Args;

static Args args_list;

char *result;
//%TOP_END

static short *_nts[];
static char  *_isinstruction[];

//%BOT_START
static void progbeg(int argc, char *argv[])
{
	parseflags(argc, argv);
	types_head = NULL;
	consts_head = NULL;
	args_list = NULL;
	print(".funcsig @void_func () -> ()\n\n");

	def_type(voidtype, "void", "void");

	def_type(chartype, "char", "int<8>");
	def_type(unsignedchar, "uchar", "int<8>");

	def_type(shorttype, "short", "int<16>");
	def_type(unsignedshort, "ushort", "int<16>");

	def_type(inttype, "int", "int<32>");
	def_type(unsignedtype, "uint", "int<32>");

	def_type(longtype, "long", "int<64>");
	def_type(unsignedlong, "ulong", "int<64>");
	def_type(longlong, "longlong", "int<64>");
	def_type(unsignedlonglong, "ulonglong", "int<64>");

	def_type(floattype, "float", "float");
	def_type(doubletype, "double", "double");
	def_type(longdouble, "longdouble", "double");

	def_type(voidptype, "ptr_void", "uptr<@void>");
	def_type(charptype, "ptr_char", "uptr<@char>");
	def_type(funcptype, "ptr_func", "ufuncptr<@void_func>");

	print("\n");

	const_name(voidptype, "NULL");
	//0b10000000000000000000000000000000, useful for negation
	const_name(inttype, "2147483648");
	const_name(longtype, "9223372036854775808");

	//INT(32)_MAX and LONG(64)_MAX respectively, needed for binary complement
	const_name(inttype, "4294967295");
	const_name(longtype, "18446744073709551615");

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
	if (f->type->u.f.oldstyle) {
		printf("//There are Ph.D.'s who are younger than ANSI C, dawg\n//Omitted Pre-ANSI C function %s.\n", f->name);
		return;
	}

	usedmask[0] = usedmask[1] = 0;
	freemask[0] = freemask[1] = ~(unsigned)0;
	for (size_t i = 0; caller[i]; i++)
		type_name(caller[i]->type);
	for (size_t i = 0; callee[i]; i++)
		type_name(callee[i]->type);
	print(".funcsig @%s_sig = ( ", f->name);
	for (size_t i = 0; callee[i]; i++)
		print("@%s ", type_name(caller[i]->type));
	print(") -> ( @%s )\n", type_name(f->type->type));
	print(".typedef @%s_ref = funcref<@%s_sig>\n", f->name, f->name);
	print(".funcdef @%s VERSION %%v1 <@%s_sig> {\n", f->name, f->name);
	print("\t%%entry( ");
	for (size_t i = 0; callee[i]; i++)
		print("<@%s> %%%s ", type_name(caller[i]->type), caller[i]->name);
	print("):\n");
	for (size_t i = 0; callee[i]; i++) {
		Symbol p = callee[i];
		Symbol q = caller[i];
		p->x.name = q->x.name = p->name;
		p->sclass = q->sclass = AUTO;
	}
	gencode(caller, callee);
	emitcode();
	//TODO: unpin all pinned objects
	print("}\n\n");
}

static void import(Symbol p)
{
	//TODO: implement import?
	print("//%s called\n", __FUNCTION__);
}
static void export(Symbol p)
{
	//TODO: implement export?
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
	p->sclass = AUTO;
	p->x.name = p->name;
}

static void segment(int n) {}

static void space(int n)
{
	print("//%s called\n", __FUNCTION__);
}

/*
	Generates symbols and a node to bridge between LCC conditional branches (one node) and Mu branches (comparison then cond branch)
 */
static void mugen_cond(Node p) {
	if (p == NULL)
		return;
	int op = generic(p->op);
	if (op == GT || op == GE || op == EQ || op == NE || op == LE || op == LT) {
		p->syms[1] = newtemp(AUTO, optype(p->op), opsize(p->op));
		p->syms[1]->x.name = stringf("cond_%s", p->syms[1]->name);
		p->syms[2] = findlabel(genlabel(1));
		p->link = newnode(LABEL + V, NULL, NULL, p->syms[2]);
	}
	mugen_cond(p->kids[0]);
	mugen_cond(p->kids[1]);
}

/*
	ONLY CALL WITH ROOT NODES
	Generates asgn nodes for fuctions that return values but the program has not stored that value
	We need this because everything except CALLV is not a statement
*/
static void mugen_fasgn(Node p, Node *forest) {
	if (generic(p->op) != CALL || optype(p->op) == V) return;
	int type = optype(p->op), sz = opsize(p->op);
	Symbol s = newtemp(AUTO, type, sz), ssz = intconst(sz);
	s->x.name = stringf("%s_tmp", s->name);
	Node addr = newnode(ADDRL + P + sizeop(8), NULL, NULL, s), asgn = newnode(ASGN + type + sizeop(sz), addr, p, ssz);
	asgn->syms[1] = ssz;
	asgn->link = p->link;
	addr->count = 0;
	p->count = 0;
	if (p == *forest)
		(*forest) = asgn;
	else {
		Node np;
		for (np = *forest; np->link != p; np = np->link);
		np->link = asgn;
	}
}

/*
	Generates a variable name for each instruction's result
 */
static void mugen_var(Node p, int child) {
	if (p == NULL)
		return;
	if (p->x.inst && child) {
		//should have a register allocated to it
		assert(p->syms[2]);
		p->syms[2] = newtemp(AUTO, optype(p->op), opsize(p->op));
		p->syms[2]->x.name = stringf("var_%s", p->syms[2]->name);
	}
	mugen_var(p->kids[0], child + 1);
	mugen_var(p->kids[1], child + 1);
}

/*
 TODO: <list>
	1. remove lables after returns
	2. remove labels at end of functions or add default return val
	3. add RETV to all funcs that don't explicitly return (possibly fixed by 2)
 */
static Node mugen(Node forest) {
	for (Node p = forest; p; p = p->link) {
		mugen_cond(p);
		mugen_fasgn(p, &forest);
	}

	gen(forest);

	for (Node p = forest; p; p = p->link)
		mugen_var(p, 0);

	return forest;
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
	Symbol s0, s1;
	Node k0, k1;
	short *nts;
	if (optype(p->op) == P) {
		switch (generic(p->op)) {
		case ADD:
			//Ptrs are always 8 bytes so we cast to a long
			char *tmp1 = stringf("%%%d_tmp", genlabel(1)), *tmp2 = stringf("%%%d_tmp", genlabel(1)), *tfrom;
			k0 = p->kids[0];
			while (k0->kids[0]) k0 = k0->kids[0];
			tfrom = type_name(k0->syms[0]->type);
			nts = _nts[_rule(p->x.state, p->x.inst)];
			p->syms[2] = newtemp(AUTO, P, 8);
			p->syms[2]->x.name = stringf("var_%s", p->syms[2]->name);
			print("\n\t\t%s = PTRCAST <@%s @long> ", tmp1, tfrom);
			emitasm(p->kids[0], nts[0]);
			print("\n\t\t%s = ADD <@long> %s ", tmp2, tmp1);
			emitasm(p->kids[1], nts[1]);
			print("\n\t\t%%%s = PTRCAST <@long @%s> %s\n", p->syms[2]->x.name, tfrom, tmp2);
			break;
		case ARG:
			Args arg = (Args)allocate(sizeof(*arg), STMT);
			k0 = p->kids[0];
			while (k0 && !k0->x.inst) k0 = k0->kids[0];
			if (k0) { //TODO: fix this (might be because printf is not declared in 8q.c)
				if (!k0->x.emitted) {
					nts = _nts[_rule(k0->x.state, k0->x.inst)];
					if (k0->kids[0])
						emitasm(k0->kids[0], nts[0]);
					if (k0->kids[1])
						emitasm(k0->kids[1], nts[1]);
				}
				arg->arg = k0->syms[2]->x.name;

				arg->next = args_list;
				args_list = arg;
			}
			break;
		case ASGN: //TODO: unpin stuff at func exit
			s0 = p->kids[0]->syms[0];
			s1 = p->kids[1]->syms[0];
			if (s1) {
				if (isarray(s1->type)) {
					Type t = s1->type;
					while (isarray(t))
						t = t->type;
					//TODO: if array is generated (ie was not declared global) then copy it to new location
					char *tmp = stringf("%%%d_%s", genlabel(1), s0->x.name);
					print("\t\t%s = COMMINST @uvm.native.pin <@%s> @%s\n", tmp, type_name(t), s1->x.name);
					print("\t\t%%%s = PTRCAST <uptr<@%s> @%s> %s\n", s0->x.name, type_name(t), type_name(s0->type), tmp);
				} else {
					print("\t\t%%%s = PTRCAST <@%s @%s> %s\n", s0->x.name, type_name(s1->type), type_name(s0->type), s1->x.name);
				}
			} else {
				nts = _nts[_rule(p->x.state, p->x.inst)];
				if(!(p->kids[1]->x.emitted) && p->kids[0]->x.inst)
					emitasm(p->kids[1], nts[1]);
				print("\t\t%%%s = ", s0->name);
				emitasm(p->kids[1], nts[1]);
				print("\n");
			}
			break;
		case INDIR:
			short indir = 0;
			k0 = p->kids[0];
			print("\t\t%%%s = LOAD PTR <@", p->syms[2]->name);
			while (k0->kids[0]) {
				if (generic(k0->op) == INDIR)
					indir++;
				else if (generic(k0->op) == ADDRF || generic(k0->op) == ADDRL || generic(k0->op) == ADDRG)
					indir--;
				k0 = k0->kids[0];
			}
			char *t = type_name(k0->syms[0]->type);
			if (indir < 0)
				for (size_t i = indir; i <= 0; i++)
					print("ptr_");
			else
				for (size_t i = 0; i <= indir; i++)
					t = t + 4;
			print("%s> %%", t);
			nts = _nts[_rule(p->x.state, p->x.inst)];
			emitasm(p->kids[0], nts[0]);
			print("\n");
			break;
		default:
			print("//PTR OP %d NOT RECOGNIZED\n", p->op);
			break;
		}
	} else {
		switch (generic(p->op)) {
		case ARG:
			Args arg = (Args)allocate(sizeof(*arg), STMT), a = args_list;
			arg->next = NULL;
			k0 = p->kids[0];
			while (k0 && !k0->x.inst) k0 = k0->kids[0];
			if (k0) { //TODO: fix this (might be because printf is not declared in 8q.c)
				emitasm(k0, k0->x.inst);
				k0->x.emitted = 1;
				arg->arg = k0->syms[2]->x.name;

				while (a != NULL && a->next != NULL)
					a = a->next;
				if (a)
					a->next = arg;
				else
					args_list = arg;
			}
			break;
		case CALL:
			printf("\t\tCALL <@%s_sig> @%s_ref ( ", p->kids[0]->syms[0]->name, p->kids[0]->syms[0]->name);
			while (args_list != NULL) {
				print("%%%s ", args_list->arg);
				args_list = args_list->next;
			}
			print(")\n");
			break;
		default:
			print("//OP %d NOT RECOGNIZED\n", p->op);
			break;
		}
	}

	if(p->x.registered)
		p->x.emitted = 1;
}
static void doarg(Node p) {}
static void target(Node p) {}
static void clobber(Node p) {}

//helper funcs
static int def_type(Type t, char *name, char *mu_t) {
	Types ts = NULL, n = NULL;
	if (isptr(t)) {
		for (ts = types_head; ts; ts = ts->next) {
			Type ts_tmp, t_tmp;
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
		char *name = stringf("ptr_%s", type_name(t->type));
		def_type(t, name, stringf("uptr<@%s>", type_name(t->type)));
		return name;
		assert(0);
	} else if (isarray(t)) {
		for (ts = types_head; ts; ts = ts->next)
			if (ts->type->u.sym == t->u.sym)
				return ts->name;
		char *name = stringf("arr_%s_%d", type_name(t->type), t->size);
		def_type(t, name, stringf("array< @%s %d >", type_name(t->type), t->size));
		return name;
		assert(0);
	} else {
		for (ts = types_head; ts; ts = ts->next)
			if (ts->type->u.sym == t->u.sym)
				return ts->name;
		assert(0);
	}
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