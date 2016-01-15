#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)

//%TOP_START
#include <stdio.h>
#ifndef NULL
#define NULL 0
#endif

#define ELEM(t,n) ((t)(n->elem))

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

//Doubly linked list, last node's next is left NULL to avoid looping forever (head->prev is valid tho)
typedef struct munode {
	void *elem;
	struct munode *next, *prev;
} *MuNode;
static MuNode muprepend(MuNode*, void*, int);
static MuNode muappend(MuNode*, void*, int);

//Global types/vars
//Maps an LCC type to a mutype
typedef struct mutype {
	Type type;
	char *name, *mutype;
} *MuType;
//Maps an LCC const to a mu const name
typedef struct cnst {
	Type type;
	char *name;
	char *val;
} *Const;

typedef struct mufunc {
	char *name;
	MuNode param_types;
	MuType ret;
} *MuFunc;

//define a new mu type
static MuType def_type(Type, char *, char *);
//Get MuType struct for type
static MuType get_mutype(Type);
//get mu name for type
static char *type_name(Type);
//get or create a const
static char *const_name(Type, char *);

//elem is a MuType/Const/MuFunc
static MuNode mutype_list = NULL;
static MuNode muconst_list = NULL;
static MuNode func_list = NULL;

static Symbol intreg[32], fltreg[32];
static Symbol intregw, fltregw;

char *outf;

//Stmt types/vars
typedef struct muinst {
	char inst[1024];
	struct muinst *parent;
} *MuInst;

//elem is a char*
static MuNode arg_list = NULL;
//elem is MuInst
static MuNode inst_list = NULL;

static Symbol g;
char *exporting = NULL;
//%TOP_END

static short *_nts[];
static char  *_isinstruction[];

//%BOT_START
static void progbeg(int argc, char *argv[])
{
	parseflags(argc, argv);
	outf = string(argv[argc - 1]);
	freopen(stringf("%s.tmp", outf), "w", stdout);

	MuFunc f;
	muappend(&func_list, NEW0(f, PERM), PERM);
	f->name = string("void");

	def_type(voidtype, "void", "void");

	def_type(chartype, "char", "int<8>");
	def_type(unsignedchar, "uchar", "int<8>");

	def_type(shorttype, "short", "int<16>");
	def_type(unsignedshort, "ushort", "int<16>");

	def_type(inttype, "int", "int<32>");
	def_type(unsignedtype, "uint", "int<32>");

	def_type(longtype, "long", "int<64>");
	def_type(unsignedlong, "ulong", "int<64>");

	def_type(floattype, "float", "float");
	def_type(doubletype, "double", "double");

	def_type(voidptype, "ptr_void", "uptr<@void>");
	def_type(charptype, "ptr_char", "uptr<@char>");
	def_type(funcptype, "ptr_void_func", "ufuncptr<@void_func>");

	print("\n");

	const_name(voidptype, "NULL");
	//0b10000000000000000000000000000000, useful for negation (ie could mult by -1 but why not use xor)
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

char *fgetl(char **buf, size_t *sz, FILE *f) {
	size_t s = *sz;
	char *res = fgets(*buf, s, f), *tmp;
	while (res && !ferror && (*buf)[strlen(*buf) - 1] != '\n') {
		s *= 2;
		tmp = allocate(s, PERM);
		memcpy(tmp, *buf, s / 2);
		*buf = tmp;
		//buf + (s / 2) - 1 because we want to overwrite the trailing \0 char
		res = fgets(*buf + (s / 2) - 1, s / 2 + 1, f);
	}
	if (ferror(f))
		perror("I/O ERROR");
	*sz = s;
	return res;
}
enum InstState
{
	UNFINISHED_INST = 1,
	CALL_INST = 2,
	CALL_PARAMS = 4
};
static void progend(void) {
	size_t bufsz = 256;
	char *tmpf = stringf("%s.tmp", outf), *buf = allocate(bufsz, PERM);

	FILE *srcf, *dstf;
	if (ferror(srcf = fopen(tmpf, "r"))) {
		fprintf(stderr, "error opening %s\n", tmpf);
	} else if (ferror(dstf = fopen(outf, "w"))) {
		fprintf(stderr, "error opening %s\n", outf);
		fclose(srcf);
	}

	do {
		Const c = ELEM(Const, muconst_list);
		fprintf(dstf, ".const @%s <@%s> = %s\n", c->name, type_name(c->type), c->val);
	} while ((muconst_list = muconst_list->next) != NULL);
	fputc('\n', dstf);

	do {
		MuFunc f = ELEM(MuFunc, func_list);
		MuNode m = f->param_types;
		MuType t;
		fprintf(dstf, ".funcsig @%s_sig = (", f->name);
		if (m) fputc(' ', dstf);
		for (; m; m = m->next) {
			t = ELEM(MuType, m);
			fprintf(dstf, "@%s ", t->name);
		}
		fprint(dstf, ") -> (");
		if (f->ret) fprintf(dstf, " @%s ", f->ret->name);
		fprint(dstf, ")\n");
		fprintf(dstf, ".typedef @%s_ref = funcref<@%s_sig>\n", f->name, f->name);
	} while ((func_list = func_list->next) != NULL);
	fputc('\n', dstf);

	do {
		MuType t = ELEM(MuType, mutype_list);
		fprintf(dstf, ".typedef @%s = %s\n", t->name, t->mutype);
	} while ((mutype_list = mutype_list->next) != NULL);

	//the following state needs to persist between lines
	MuInst inst;
	size_t inst_state = 0, depth = 0;
	muappend(&inst_list, NEW0(inst, PERM), PERM);
	while (fgetl(&buf, &bufsz, srcf) != NULL) {
		//ignore trailing newline
		int len = strlen(buf) - 1;
		if (len && buf[len - 1] == ')') {
			size_t inst_buf_idx = strlen(inst->inst), uinst_buf_idx;
			for (size_t i = 0; i < len; i++) {
				char c = buf[i];
				switch (c) {
				case '\t':
					break;
				case '(':
					if (inst_state & CALL_INST) {
						inst_state |= CALL_PARAMS;
						inst->inst[inst_buf_idx++] = c;
						break;
					}
					depth++;
					inst_state |= UNFINISHED_INST;
					uinst_buf_idx = inst_buf_idx;
					inst_buf_idx = 0;
					MuInst parent = inst;
					muprepend(&inst_list, NEW0(inst, PERM), PERM);
					inst->parent = parent;
					break;
				case ')':
					if (inst_state & CALL_PARAMS) {
						inst_state ^= CALL_PARAMS;
						inst->inst[inst_buf_idx++] = c;
						break;
					}
					depth--;
					inst = inst->parent;
					break;
				case 'C':
					if (memcmp(buf + i, "CALL", 4) == 0)
						inst_state |= CALL_INST;
				case ' ':
					if (inst_state & UNFINISHED_INST)
						inst_state ^= UNFINISHED_INST;
				default:
					inst->inst[inst_buf_idx++] = c;
					if (inst_state & UNFINISHED_INST)
						inst->parent->inst[uinst_buf_idx++] = c;
					break;
				}
			}

			if (!depth) {
				do {
					fputs("\t\t", dstf);
					fputs(ELEM(MuInst, inst_list)->inst, dstf);
					if (inst_list->next)
						fputc('\n', dstf);
				} while ((inst_list = inst_list->next) != NULL);

				muappend(&inst_list, NEW0(inst, PERM), PERM);
			}
		} else
			fputs(buf, dstf);
	}

	if ((!feof(srcf) && ferror(srcf)) || ferror(dstf))
		fprintf(stderr, "I/O error");
	fclose(srcf);
	fclose(dstf);
}

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
	char *cn;
	switch (suffix) {
	case F:
		if (size == 4)
			cn = const_name(floattype, stringf("%f", v.d));
		else if (size == 8)
			cn = const_name(doubletype, stringf("%f", v.d));
		break;
	case I:
		if (size == 1)
			cn = const_name(chartype, stringf("%d", v.d));
		else if (size == 2)
			cn = const_name(shorttype, stringf("%d", v.d));
		else if (size == 4)
			cn = const_name(inttype, stringf("%d", v.d));
		else if (size == 8)
			cn = const_name(longtype, stringf("%d", v.d));
		break;
	case U:
		if (size == 1)
			cn = const_name(unsignedchar, stringf("%u", v.d));
		else if (size == 2)
			cn = const_name(unsignedshort, stringf("%u", v.d));
		else if (size == 4)
			cn = const_name(unsignedtype, stringf("%u", v.d));
		else if (size == 8)
			cn = const_name(unsignedlong, stringf("%u", v.d));
		break;
	case P:
		print("//TODO: implement pointer in def const\n");
		break;
	}
	if (cn && exporting) {
		printf("@%s = @%s\n\n", exporting, cn);
		exporting = NULL;
	}
}
//TODO: use HAIL
static void defstring(int n, char *str)
{
	//for (size_t i = 0; i < n; i++)
	//	const_name(chartype, stringf("%d", str[i]));
	//print("\n");
	//Type t = array(chartype, n, 1);
	//char *type = type_name(t), *name = g->x.name;
	//print("@%s = NEW <@%s>\n", name, type);
	//print("@%s_iref0 = GETIREF <@%s> @%s\n", name, type, name);
	//for (size_t i = 0; i < n; i++)
	//	print("STORE <@%s> @%s_iref%d @%s\n@%s_iref%d = SHIFTIREF <@%s @%s> @%s_iref%d @%s\n", type_name(chartype), name, i, const_name(chartype, stringf("%d", str[i])), name, i + 1, type, type_name(longtype), name, i, const_name(longtype, "1"), const_name(chartype, stringf("%d", str[i])));
}
static void defsymbol(Symbol p)
{
	if (p->generated && p->scope != LABELS)
		p->x.name = stringf("%s", p->name);
	else if (p->generated)
		p->x.name = stringf("L%s", p->name);
	else if (p->scope >= LOCAL)
		p->x.name = stringf("%d_%s", p->scope, p->name);
	else if (p->type && isscalar(p->type)) {
		if (isunsigned(p->type))
			p->x.name = stringf("%U", p->u.value);
		else
			p->x.name = stringf("%D", p->u.value);
		const_name(p->type, p->x.name);
	} else
		p->x.name = p->name;
}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls)
{
	if (f->type->u.f.oldstyle) {
		printf("//There are Ph.D.'s who are younger than ANSI C, dawg\n//Omitted Pre-ANSI C function %s.\n", f->name);
		return;
	}

	usedmask[0] = usedmask[1] = 0;
	freemask[0] = freemask[1] = ~(unsigned)0;

	MuFunc mf;
	MuType mt;
	muappend(&func_list, NEW0(mf, PERM), PERM);
	mf->name = string(f->name);
	mf->ret = get_mutype(f->type->type);
	for (size_t i = 0; callee[i]; i++) {
		muappend(&(mf->param_types), get_mutype(caller[i]->type), PERM);
	}

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
	exporting = p->x.name;
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
		//TODO: this is probably what is messing up multi-condition ifs :(
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
		//should have a "register" allocated to it
		assert(p->syms[2]);
		p->syms[2] = newtemp(AUTO, optype(p->op), opsize(p->op));
		p->syms[2]->x.name = stringf("var_%s.%d", p->syms[2]->name, p->syms[2]->scope - LOCAL);
	}
	mugen_var(p->kids[0], child + 1);
	mugen_var(p->kids[1], child + 1);
}

static void mugen_walk(Node n) {
	if (n == NULL)
		return;
	mugen_walk(n->kids[0]);
	mugen_walk(n->kids[1]);
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

	mugen_var(forest, 0);

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
		//case ADD:
		//	//Ptrs are always 8 bytes so we cast to a long
		//	char *tmp1 = stringf("%%%d_tmp", genlabel(1)), *tmp2 = stringf("%%%d_tmp", genlabel(1)), *tfrom;
		//	k0 = p->kids[0];
		//	while (k0->kids[0]) k0 = k0->kids[0];
		//	tfrom = type_name(k0->syms[0]->type);
		//	nts = _nts[_rule(p->x.state, p->x.inst)];
		//	p->syms[2] = newtemp(AUTO, P, 8);
		//	p->syms[2]->x.name = stringf("var_%s", p->syms[2]->name);
		//	print("\n\t\t%s = PTRCAST <@%s @long> ", tmp1, tfrom);
		//	emitasm(p->kids[0], nts[0]);
		//	print("\n\t\t%s = ADD <@long> %s ", tmp2, tmp1);
		//	emitasm(p->kids[1], nts[1]);
		//	print("\n\t\t%%%s = PTRCAST <@long @%s> %s\n", p->syms[2]->x.name, tfrom, tmp2);
		//	break;
		case ARG:
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
				muappend(&arg_list, k0->syms[2]->x.name, STMT);
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
					//TODO: FIX THIS
					print("\t\t%%%s = PTRCAST <@%s @%s> %s\n", s0->x.name, type_name(s1->type), type_name(s0->type), s1->x.name);
				}
			} else {
				nts = _nts[_rule(p->x.state, p->x.inst)];
				if (!(p->kids[1]->x.emitted) && p->kids[1]->x.inst)
					emitasm(p->kids[1], nts[1]);
				print("\t\t%%%s = %%", s0->x.name);
				emitasm(p->kids[1], nts[1]);
				print("\n");
			}
			break;
		case INDIR:
			k0 = p->kids[0];
			short indir = 0;
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
			print("\t\t%%%s = LOAD PTR <@%s> ", p->syms[2]->x.name, t);
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
			k0 = p->kids[0];
			while (k0 && !k0->x.inst) k0 = k0->kids[0];
			if (k0) { //TODO: fix this (might be because printf is not declared in 8q.c)
				emitasm(k0, k0->x.inst);
				k0->x.emitted = 1;
				muappend(&arg_list, k0->syms[2]->x.name, STMT);
			}
			break;
		case CALL:
			if (optype(p->op) == V)
				printf("\t\t");
			printf("CALL <@%s_sig> @%s_ref ( ", p->kids[0]->syms[0]->name, p->kids[0]->syms[0]->name);
			while (arg_list != NULL) {
				print("%%%s ", arg_list->elem);
				arg_list = arg_list->next;
			}
			print(")\n");
			break;
		default:
			print("//OP %d NOT RECOGNIZED\n", p->op);
			break;
		}
	}

	if (p->x.registered)
		p->x.emitted = 1;
}
static void doarg(Node p) {}
static void target(Node p) {}
static void clobber(Node p) {}

static MuNode muprepend(MuNode *headp, void* e, int scope) {
	MuNode n = NEW0(n, scope), head = *headp;
	if (head == NULL) {
		head = n;
		head->prev = head;
	}

	*headp = n;
	n->next = head;
	n->prev = head->prev;
	head->prev = n;
	n->elem = e;

	return n;
}
static MuNode muappend(MuNode *headp, void* e, int scope) {
	MuNode n = NEW0(n, scope), head = *headp;
	if (head == NULL) {
		head = n;
		*headp = head;
	} else {
		//update old end of list
		head->prev->next = n;
	}

	//put the new node at the end
	n->prev = head->prev;
	head->prev = n;

	n->elem = e;
	return n;
}

static MuType def_type(Type t, char *name, char *mu_t) {
	MuNode mt_node = mutype_list;
	MuType mt;
	for (; mt_node; mt_node = mt_node->next) {
		mt = ELEM(MuType, mt_node);
		if (isptr(t)) {
			if (!isptr(mt->type)) continue;

			Type mt_tmp, t_tmp;
			mt_tmp = mt->type;
			t_tmp = t;
			//check to see if the number of levels of indirection and the base types are the same
			while (isptr(mt_tmp) && isptr(t_tmp)) {
				mt_tmp = mt_tmp->type;
				t_tmp = t_tmp->type;
			}
			if (mt_tmp->u.sym == t_tmp->u.sym)
				return mt;
		} else if (mt->type->u.sym == t->u.sym)
			return mt;
	}

	muappend(&mutype_list, NEW0(mt, PERM), PERM);

	mt->type = t;
	mt->name = string(name);
	mt->mutype = string(mu_t);
	return mt;
}

static MuType get_mutype(Type t) {
	MuNode mtnode = mutype_list;
	MuType mt;
	Type mt_tmp, t_tmp;
	if (isptr(t)) {
		for (; mtnode; mtnode = mtnode->next) {
			mt = ELEM(MuType, mtnode);
			if (!isptr(mt->type)) continue;
			mt_tmp = mt->type;
			t_tmp = t;
			while (isptr(mt_tmp) && isptr(t_tmp)) {
				mt_tmp = mt_tmp->type;
				t_tmp = t_tmp->type;
			}
			if (mt_tmp->u.sym == t_tmp->u.sym)
				return mt;
		}
		return def_type(t, stringf("ptr_%s", type_name(t->type)), stringf("uptr<@%s>", type_name(t->type)));
	} else if (isarray(t)) {
		for (; mtnode; mtnode = mtnode->next)
			if (ELEM(MuType, mtnode)->type->u.sym == t->u.sym)
				return ELEM(MuType, mtnode);
		return def_type(t, stringf("arr_%s_%d", type_name(t->type), t->size), stringf("array< @%s %d >", type_name(t->type), t->size));
	} else {
		for (; mtnode; mtnode = mtnode->next)
			if (ELEM(MuType, mtnode)->type->u.sym == t->u.sym)
				return ELEM(MuType, mtnode);
		assert(0); //base type that hasn't been defined so...
	}
	return NULL;
}

static char *type_name(Type t) {
	if (t == longlong)
		t = longtype;
	else if (t == unsignedlonglong)
		t = unsignedlong;
	else if (t == longdouble)
		t = doubletype;

	MuType mt = get_mutype(t);
	if (mt)
		return mt->name;
	return NULL;
}

static char *const_name(Type t, char *val) {
	char *tmp = val, *tmp2 = val;
	do {
		char c = *tmp;
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '.' || c == '-')
			continue;
		*tmp = '.';
	} while (*(++tmp) != 0);
	char *tn = type_name(t), *v = string(tmp2);
	MuNode mcnode = muconst_list;
	Const c;
	for (; mcnode; mcnode = mcnode->next)
		if (ELEM(Const, mcnode)->type == t && ELEM(Const, mcnode)->val == v)
			return ELEM(Const, mcnode)->name;

	muappend(&muconst_list, NEW0(c, PERM), PERM);
	c->name = stringf("%s_%s", tn, v);
	c->val = v;
	c->type = t;
	return c->name;
}