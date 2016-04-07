#include "c.h"

//%TOP_START
#include <stdio.h>
#ifndef NULL
#define NULL 0
#endif

/// Get the element pointed to by n as an object of type t.
#define ELEM(t,n) ((t)(n->elem))

/// Get `sym->name` as a variable name.
/*!
Prepends an @ onto global symbol names and a % onto local symbol names.
\sa XNAME
*/
#define NAME(sym) (sym->scope == GLOBAL ? stringf("@%s", sym->name) : stringf("%%%s", sym->name))
/// Get `sym->x.name` as a variable name.
/*!
Prepends an @ onto global symbol names and a % onto local symbol names.
\sa NAME
*/
#define XNAME(sym) (sym->scope == GLOBAL ? stringf("@%s", sym->x.name) : stringf("%%%s", sym->x.name))

/// Check if op takes the address of a variable.
#define isaddr(op) (generic(op) == ADDRF || generic(op) == ADDRG || generic(op) == ADDRL)

/// Dereference t.
#define indir(t) (unqual(t)->type)

/// Print out a TODO comment
#define todo(str, ...) printf("//%s:%d(TODO): " str "\n", __FILE__, __LINE__, __VA_ARGS__)
/// Print out a PANIC comment
#define panic(str, ...) printf("//%s:%d(PANIC): " str "\n", __FILE__, __LINE__, __VA_ARGS__)

/// C doesn't have bool so we define it here
typedef enum { false /*! the false value */, true /*! the true value */ } bool_t;

typedef struct muconst * MuConst;
typedef struct munode * MuNode;
typedef struct muinst * MuInst;
typedef struct mutype * MuType;
typedef struct mufunc * MuFunc;
typedef struct muglobal * MuGlobal;

///Doubly linked list node
/*!
	The last node in the list has `next` left NULL to make looping through the list once easier.
*/
struct munode {
	void *elem; ///< Element pointed to by this node
	MuNode next; ///< Next node in the list
	MuNode prev; ///< Previous node in the list
};

///Contains a Mu IR insruction stored as a string
/*!
	Used by progend()
*/
struct muinst {
	char inst[1024]; ///< Mu IR instruction
	MuInst parent; ///< ::muinst that uses the result of this instuction as input
};

///Maps a ::Type to a Mu IR type
struct mutype {
	Type type; ///< ::Type being mapped
	char *name; ///< Mu IR type name
	char *mutype; ///< Mu IR type definition
};

///Maps an LCC const to a Mu const variable
struct muconst {
	Type type; ///< ::Type of the const
	char *name; ///< Mu variable name
	char *val; ///< Value of the constant
};

///Represents a method defined or used in the program
struct mufunc {
	char *name; ///< Name of the function
	MuNode param_types; ///< Head of the list of parameters. munode::elem is a ::Symbol
	MuType ret; ///< Return type of the function
	bool_t import; ///< `true` if this function needs to be imported
};

///Represents a .global definition
/*!
	If muglobal::s has a scalar ::Type (including ptrs) then muglobal::init_list is a single node with munode::elem pointing
	to an initialization value. However, because of how LCC works, we do not believe this case is possible.

	If muglobal::s has an array ::Type of size `n` then muglobal::init_list is a list of `m <= n` elements
	to intialize the first `m` elements of the array. All munode::elem point to the same element type.

	If muglobal::s has a struct ::Type with `n` fields then muglobal::init_list is a list of `m <= n` elements
	to intialize the first `m` fields of the struct. The type of each munode::elem depends on the struct field type.
*/
struct muglobal {
	Symbol s; ///< Global symbol to define
	MuNode init_list; ///< Head of the list of elements to initialize muglobal::s with
};

/*!
\defgroup lcc Methods called directly by LCC
@{
*/
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
/// @}

/*!
\defgroup util Utility Methods
@{
*/

static char *const_name(Type, char *);
static char *type_name(Type);

static MuType def_type_partial(Type, const char *);
static MuType def_type(Type, const char *, const char *);
static bool_t def_func(char*, Type, bool_t);

static MuNode muprepend(MuNode*, void*, int);
static MuNode muappend(MuNode*, void*, int);

static bool_t types_are_equal(Type, Type);

static void print_fields(Symbol);

static MuType get_mutype(Type);
static Type node_type(Node, bool_t);
///@}

/*!
\defgroup glists Global Lists
@{
*/
/// munode::elem points to a ::MuType
/*! This list stores a ::mutype for each ::Type used/declared in the program. */
static MuNode mutype_list = NULL;
/// munode::elem points to a ::muconst
/*! This list is used to store a ::muconst for each literal (int, float, ptr literal) used in the program. */
static MuNode muconst_list = NULL;
/// munode::elem points to a ::mufunc
/*! This list is used to store a ::mufunc for each method used/declared in the program. */
static MuNode func_list = NULL;
/// munode::elem points to a char * holding a Mu IR instruction
/*! This list is used to store instructions that are pre-processed by Mu (right now this just includes the .sizeof operator). */
static MuNode preproc_list = NULL;
/// munode::elem points to a ::muglobal
/*! This list is used to store a ::muglobal for each LCC global. Each global is declared in the main file and initialized in a HAIL file. */
static MuNode muglobal_list = NULL;
/// munode::elem points to a char * holding a Mu IR instruction
/*! This list is used to store instructions that unpin all variables that were pinned in a method. */
static MuNode unpin_insts = NULL;
///@}

static Symbol intreg[32], fltreg[32];
static Symbol intregw, fltregw;

/// Used to store the original name of the out file
/*! Since we redirect stdout we need to keep the name of the file we should eventually output to. */
char *outf;

/// munode::elem points to a char * holding a Mu variable name
/*! This list is used to store variables that are passed as args to the next CALL instruction. The list is cleared by every CALL instruction. */
static MuNode arg_list = NULL;
/// munode::elem points to a ::muinst
/*! This list is used in ::progend to store instructions from the current line being processed. */
static MuNode inst_list = NULL;

///The name of the exported variable being initialized
char *exporting = NULL;

///Are we initializing a global?
bool_t initializing_global = false;
//%TOP_END

//These are declared at the top of the generated mu.c, we declare them here so intelisense works
static short *_nts[];
static char  *_isinstruction[];

//%BOT_START
///Initialize variables and do program-wide setup
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

	printf("\n");

	const_name(voidptype, "NULL");
	//0b10000000000000000000000000000000, useful for negation (ie could mult by -1 but why not use xor)
	const_name(inttype, "2147483648");
	const_name(longtype, "9223372036854775808");

	//INT(32)_MAX and LONG(64)_MAX respectively, needed for binary complement
	const_name(inttype, "4294967295");
	const_name(longtype, "18446744073709551615");

	printf("\n");

	for (size_t i = 0; i < 32; i++)
		intreg[i] = mkreg("i%d", i, 1, IREG);

	for (size_t i = 0; i < 32; i++)
		fltreg[i] = mkreg("f%d", i, 0, FREG);

	intregw = mkwildcard(intreg);
	fltregw = mkwildcard(fltreg);

	tmask[0] = tmask[1] = ~(unsigned)0;
	vmask[0] = vmask[1] = 0;
}

enum InstState
{
	UNFINISHED_INST = 1,
	CALL_INST = 2,
	CALL_PARAMS = 4
};
char *fgetl(char **buf, size_t *sz, FILE *f)
{
	size_t s = *sz;
	char *res = fgets(*buf, s, f), *tmp;
	while (res && !ferror(f) && (*buf)[strlen(*buf) - 1] != '\n') {
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
static void progend(void)
{
	size_t bufsz = 256;
	char *tmpf = stringf("%s.tmp", outf), *buf = allocate(bufsz, PERM);

	FILE *srcf;
	if (ferror(srcf = fopen(tmpf, "r"))) {
		fprintf(stderr, "error opening %s\n", tmpf);
	} else if (ferror(freopen(outf, "w", stdout))) {
		fprintf(stderr, "error reopening stdout to %s\n", outf);
		fclose(srcf);
	}

	MuNode mn;

	for (mn = preproc_list; mn; mn = mn->next)
		puts(ELEM(char *, mn));
	printf("\n");

	for (mn = muglobal_list; mn; mn = mn->next) {
		MuGlobal mg = ELEM(MuGlobal, mn);
		printf(".global @%s = ", mg->s->x.name);
		for (MuNode n = mg->init_list; n; n = n->next) {
			printf("@%s", ELEM(char *, n));
			if (n->next)
				printf(", ");
		}
		printf("\n");
		if (isstruct(mg->s->type))
			print_fields(mg->s);
	}
	printf("\n");

	//there is a gaurentee that there will be something in mutype, muconst, and mufunc lists
	mn = mutype_list;
	do {
		MuType t = ELEM(MuType, mn);
		if (t->mutype == NULL) continue;
		printf(".typedef @%s = %s\n", t->name, t->mutype);
		if (!isptr(t->type)) {
			printf(".typedef @%s_ref = ref<@%s>\n", t->name, t->name);
			printf(".typedef @%s_iref = iref<@%s>\n", t->name, t->name);
		}
	} while ((mn = mn->next) != NULL);
	printf("\n");

	mn = muconst_list;
	do {
		MuConst c = ELEM(MuConst, mn);
		printf(".const @%s <@%s> = %s\n", c->name, type_name(c->type), c->val);
	} while ((mn = mn->next) != NULL);
	printf("\n");

	mn = func_list;
	do {
		MuFunc f = ELEM(MuFunc, mn);
		MuNode m = f->param_types;
		MuType t;
		if (f->import)
			puts("//import");
		printf(".funcsig @%s_sig = (", f->name);
		for (; m; m = m->next) {
			t = ELEM(MuType, m);
			printf("@%s", t->name);
			if (m->next)
				printf(" ");
		}
		printf(") -> (");
		if (f->ret) printf("@%s", f->ret->name);
		printf(")\n");
		printf(".typedef @%s_ref = funcref<@%s_sig>\n", f->name, f->name);
	} while ((mn = mn->next) != NULL);

	fflush(stdout);

	//the following state needs to persist between lines
	MuInst inst;
	size_t inst_state = 0, depth = 0;
	muappend(&inst_list, NEW0(inst, STMT), STMT);
	while (fgetl(&buf, &bufsz, srcf) != NULL) {
		int len = strlen(buf) - 1;
		buf[len] = '\0'; //we use puts so the buffer shouldn't also end with a newline

		inst_state &= ~CALL_INST; //need to keep UNFINISHED_INST flag, but both CALL flags should be cleared
		if (len && buf[len - 1] == ')' && strncmp(buf, "//", 2)) {
			size_t inst_buf_idx = strlen(inst->inst), uinst_buf_idx;
			for (size_t i = 0; i < len; i++) {
				char c = buf[i];
				switch (c) {
				case '\t':
					break;
				case '(':
					if (inst_state & CALL_INST) {
						inst_state &= ~CALL_INST;
						inst_state |= CALL_PARAMS;
						inst->inst[inst_buf_idx++] = c;
					} else if (i) {
						depth++;
						inst_state |= UNFINISHED_INST;
						uinst_buf_idx = inst_buf_idx;
						inst_buf_idx = 0;
						MuInst parent = inst;
						muprepend(&inst_list, NEW0(inst, STMT), STMT);
						inst->parent = parent;
						if (strncmp(buf + i - 1, "%(", 2) == 0) //special case for when a symbol name is itself an instruction
							inst->inst[inst_buf_idx++] = '%';
					} else
						/*for muti argument instructions where both args can be trees of instructions the whole thing has to be wrapped in ()
						  but we don't want to emit the instuction name as if it were a var name */
						depth++;
					break;
				case ')':
					if (depth) {
						if (--depth == 0 && inst_state & CALL_PARAMS) { //for call instuctions where the function takes params.
							inst_state &= ~CALL_PARAMS;
							inst->inst[inst_buf_idx++] = c;
							inst->inst[inst_buf_idx++] = '\n';
						}
						if (inst->parent)
							inst = inst->parent;
					} else if (inst_state & CALL_PARAMS) {
						inst_state &= ~CALL_PARAMS;
						inst->inst[inst_buf_idx++] = c;
						inst->inst[inst_buf_idx++] = '\n';
					}
					break;
				case ' ':
					if (inst_state & UNFINISHED_INST)
						inst_state &= ~UNFINISHED_INST;
				case 'C':
					if (strncmp(buf + i, "CALL", 4) == 0)
						inst_state |= CALL_INST;
				default:
					inst->inst[inst_buf_idx++] = c;
					if (inst_state & UNFINISHED_INST)
						inst->parent->inst[uinst_buf_idx++] = c;
					break;
				}
			}

			if (depth == 0 && !(inst_state & CALL_PARAMS)) {
				size_t num = 0;
				do {
					printf("\t\t%s", ELEM(MuInst, inst_list)->inst);
					if (num++ == 0 || inst_list->next)
						printf("\n");
				} while ((inst_list = inst_list->next) != NULL);
				deallocate(STMT);
				muappend(&inst_list, NEW0(inst, STMT), STMT);
			}
		} else
			puts(buf);
	}

	if (!feof(srcf) && ferror(srcf))
		fprintf(stderr, "I/O error");
	fclose(srcf);
	fclose(stdout);
}

static void address(Symbol p, Symbol q, long n)
{
	char *name = string("");
	if (isunion(q->type))
		panic("address called with a union");
	else if (isstruct(q->type)) {
		Field f;
		for (f = q->type->u.sym->u.s.flist; f; f = f->link)
			if (f->link && f->link->offset >= n)
				break;
		if (f->offset != n) {
			todo("add support for arrays in structs in address");
		} else
			name = stringf("%s_%s", q->name, f->name);
	} else if (isarray(q->type)) {
		Type t = unqual(indir(q->type));
		Symbol s = intconst(n / t->size);
		name = stringf("(%s_%s = GETELEMIREF <@%s @%s> %%%s_iref @%s)\n",
							q->name, s->name, type_name(q->type), type_name(s->type), q->name, const_name(s->type, s->name));
		p->type = ptr(t);
	} else if (isptr(q->type)) {
		Type t = unqual(indir(q->type));
		if (isarray(t)) {
			Type t2 = unqual(indir(t));
			Symbol s = intconst(n / t2->size);
			name = stringf("(%s_%s = GETELEMIREF <@%s @%s> %%%s_iref @%s)\n",
								q->name, s->name, type_name(q->type), type_name(s->type), q->name, const_name(s->type, s->name));
			p->type = ptr(t);
		} else
			panic("was expecting an array ptr in address");
	} else
		panic("add a case for type %s in address", type_name(q->type));

	p->x.name = name;
}

static void defaddress(Symbol p)
{
	muappend(&(ELEM(MuGlobal, muglobal_list)->init_list), p->x.name, PERM);
}
static void defconst(int suffix, int size, Value v)
{
	char *cn, *val;
	switch (suffix) {
	case F:
		val = stringf("%f", v.d);
		if (size == 4)
			cn = const_name(floattype, val);
		else if (size == 8)
			cn = const_name(doubletype, val);
		break;
	case I:
		val = stringf("%d", v.i);
		if (size == 1)
			cn = const_name(chartype, val);
		else if (size == 2)
			cn = const_name(shorttype, val);
		else if (size == 4)
			cn = const_name(inttype, val);
		else if (size == 8)
			cn = const_name(longtype, val);
		break;
	case U:
		val = stringf("%u", v.u);
		if (size == 1)
			cn = const_name(unsignedchar, val);
		else if (size == 2)
			cn = const_name(unsignedshort, val);
		else if (size == 4)
			cn = const_name(unsignedtype, val);
		else if (size == 8)
			cn = const_name(unsignedlong, val);
		break;
	case P:
		val = stringf("%u", v.p);
		//there is no way to know what the pointer type actually is, it'll get converted to the right type when being used
		cn = const_name(voidptype, val);
		break;
	}
	if (cn && exporting) {
		printf("@%s = IDENT @%s\n\n", exporting, cn);
		exporting = NULL;
	} else if (cn && initializing_global)
		muappend(&(ELEM(MuGlobal, muglobal_list)->init_list), cn, PERM);
}

static void defstring(int n, char *str)
{
	MuGlobal mg = ELEM(MuGlobal, muglobal_list);
	for (size_t i = 0; i < n; i++) {
		MuNode mn = muappend(&(mg->init_list), NULL, PERM);
		mn->elem = const_name(chartype, stringf("%u", str[i]));
	}
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
		if (isfloat(p->type))
			p->x.name = p->name;
		else if (isunsigned(p->type))
			p->x.name = stringf("%U", p->u.value);
		else
			p->x.name = stringf("%D", p->u.value);

		const_name(p->type, p->x.name);
	} else
		p->x.name = p->name;
}

static bool_t def_func(char *fname, Type ft, bool_t import)
{
	if (ft->u.f.oldstyle) {
		printf("//There are Ph.D.'s who are younger than ANSI C, dawg\n//Omitted Pre-ANSI C function %s.\n", fname);
		return false;
	}

	MuFunc mf;
	Type *args = ft->u.f.proto;
	muappend(&func_list, NEW0(mf, PERM), PERM);
	mf->name = string(fname);
	mf->ret = get_mutype(ft->type);
	mf->import = import;
	for (size_t i = 0; args[i]; i++)
		muappend(&(mf->param_types), get_mutype(args[i]), PERM);
	return true;
}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls)
{
	if (!def_func(f->name, f->type, false))
		return;

	usedmask[0] = usedmask[1] = 0;
	freemask[0] = freemask[1] = ~(unsigned)0;

	printf(".funcdef @%s VERSION %%v1 <@%s_sig> {\n", f->name, f->name);
	printf("\t%%entry( ");
	for (size_t i = 0; callee[i]; i++) {
		printf("<@%s> %%%s ", type_name(caller[i]->type), caller[i]->name);
		Symbol p = callee[i];
		Symbol q = caller[i];
		p->x.name = q->x.name = p->name;
		p->sclass = q->sclass = AUTO;
	}
	printf("):\n");

	for (size_t i = 0; callee[i]; i++)
		if (!isscalar(caller[i]->type)) {
			Type t = caller[i]->type;
			char *name = caller[i]->name;
			printf("\t\t%%%s_ref = GETREF <@%s> %%%s\n", name, type_name(t), name);
			printf("\t\t%%%s_iref = GETIREF <@%s> %%%s_ref\n", name, type_name(t), name);

			print_fields(caller[i]);
		}

	gencode(caller, callee);
	emitcode();
	printf("}\n\n");
}

static void import(Symbol p)
{
	if (isfunc(p->type))
		def_func(p->name, p->type, true);
	else
		todo("Add a case for %s in import", type_name(p->type));
}
static void export(Symbol p)
{
	if (!isfunc(p->type))
		exporting = p->x.name;
	else
		printf("COMMINST @uvm.native.expose [#DEFAULT] <[@%s_sig]>\n", p->x.name);
}
static void global(Symbol p)
{
	MuGlobal mg;
	muprepend(&muglobal_list, NEW0(mg, PERM), PERM);
	mg->s = p;
	initializing_global = true;
}
static void local(Symbol p)
{
	printf("\t\t%s_iref = ALLOCA <@%s>\n", NAME(p), type_name(p->type));
	printf("\t\t%s = COMMINST @uvm.native.pin <@%s_iref> %s_iref\n", NAME(p), type_name(p->type), NAME(p));
	if (!isscalar(p->type)) {
		//TODO: work on pinning, need to get an ref after pinning so that we can pin the ptr
		printf("\t\t%s_ptr = GETREF <@%s> %s_ptr_ref\n", NAME(p), type_name(ptr(p->type)), NAME(p));
		if (isstruct(p->type))
			print_fields(p);
	}

	p->x.name = p->name;
}

static void segment(int n) {}

static void space(int n)
{
	printf("//%s(%d) called\n", __FUNCTION__, n);
}

//Create a copy of a sym, assign it to the origonal location, and return the origonal symbol
static Symbol clone_sym(Symbol *s)
{
	Symbol snew = NEW(snew, FUNC), sold = *s;
	memcpy(snew, *s, sizeof(*snew));
	*s = snew;
	return sold;
}

/*
ONLY CALL WITH ROOT NODES
Generates asgn nodes for fuctions that return values but the program has not stored that value
We need this because everything except CALLV is not a statement
*/
static void mugen_fasgn(Node p, Node *forest)
{
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
	fix storing to arrays (ex. arr[0] = 1), need to get reference to first elem of array and store to that
	also fix other issues with storing to pointers?
*/
static void fix_asgn_to_ptr(Node n)
{
	Node k0 = n->kids[0], k1 = n->kids[1], tmp;
	Type t0 = indir(node_type(k0, false)), t1 = node_type(k1, false);
	Symbol s;
	assert(generic(n->op) == ASGN);
	assert(isptr(t0) || isarray(t0));
	if (isaddr(k0->op) && isarray(t0)) {
		s = clone_sym(&(k0->syms[0]));
		k0->syms[0]->x.name = stringf("(%s_0 = GETELEMIREF <@%s @%s> %s @%s)\n",
												s->name, type_name(s->type), type_name(unsignedtype), NAME(s), const_name(unsignedtype, "0"));
		k0->syms[0]->type = ptr(indir(t0));
	}
}

//TODO: do this in emit2
//static void fix_ptr_arith(Node n) {
//	Node k0 = n->kids[0], k1 = n->kids[1];
//	Type t0 = indir(node_type(k0, false)), t1 = node_type(k1, false);
//	assert(t1 == longtype || t1 == unsignedlong);
//
//	if (isscalar(t0)) {
//		Value v;
//		v.u = unqual(t0)->size;
//		n->kids[1] = newnode(DIV + ttob(t1), k1, newnode(CNST + ttob(t1), NULL, NULL, constant(t1, v)), NULL);
//	} else if (isarray(t0) || isstruct(t0)) {
//		size_t off;
//		if (generic(k1->op) == CNST) {
//			off = k1->syms[0]->u.value;
//		} else if (isarray(t0)) {
//			/*we want to emit
//				%tmp = GETELEMREF <@type_name(t1)> k0 k1
//			*/
//		} else
//			panic("we do not support adding arbitrary values to struct ptrs");
//	} else
//		assert(false);
//}

static void mugen_walk(Node n, Node parent)
{
	if (n == NULL)
		return;

	Node k0 = n->kids[0], k1 = n->kids[1];
	Type t = node_type(n, true), t0 = node_type(k0, true), t1 = node_type(k1, true);
	int op = generic(n->op), opt = optype(n->op), opsz = opsize(n->op);

	mugen_walk(k0, n);
	mugen_walk(k1, n);

	if (isaddr(n->op) && indir(t) && isunion(indir(t))) {
		//TODO: figure out unions :(
		//Symbol s = clone_sym(&(n->syms[0]));
		//Type pt = node_type(parent, false);
		//n->syms[0]->x.name = stringf("%s_iref_%s", s->x.name, type_name(pt));
		//n->syms[0]->type = ptr(pt);
	}

	////adds LOAD instuctions for retrieving values from a union
	//if (isaddr(n->op) && isunion(indir(t)) && optype(parent->op) != B) {
	//	Symbol tmp = newtemp(AUTO, optype(parent->op), opsize(parent->op)), s = clone_sym(&(n->syms[0]));
	//	char *ptname = type_name(btot(optype(parent->op), opsize(parent->op)));
	//	n->syms[0]->x.name = stringf("(%s_tmp = LOAD <@%s> %%%s_iref_%s)\n", tmp->name, ptname, s->name, ptname);
	//}

	// Generates symbols and a node to bridge between LCC conditional branches (one node) and
	// Mu branches (comparison then cond branch)
	if (op == GT || op == GE || op == EQ || op == NE || op == LE || op == LT) {
		n->syms[1] = newtemp(AUTO, opt, opsz);
		n->syms[1]->x.name = stringf("cond_%s", n->syms[1]->name);
		n->syms[2] = findlabel(genlabel(1));
		Node tmp = newnode(LABEL + V, NULL, NULL, n->syms[2]);
		tmp->link = n->link;
		n->link = tmp;
	}

	//normalize ptr ops to have the ptr be the left child
	if (op == ADD && opt == P && (isptr(t1) || isarray(t1))) {
		n->kids[0] = k1;
		n->kids[1] = k0;
	}

	if (op == ASGN && (isptr(indir(t0)) || isarray(indir(t0))))
		fix_asgn_to_ptr(n);

	//For sanity
	for (size_t i = 0; i < 2; i++)
		if (n->syms[i] && n->syms[i]->temporary) {
			if (strncmp(n->syms[i]->name, "tmp", 3) == 0) continue;
			n->syms[i]->name = stringf("tmp_%s", n->syms[i]->name);
			n->syms[i]->x.name = n->syms[i]->name;
		} else if (n->syms[i] && n->syms[i]->generated && n->syms[i]->scope >= LOCAL) {
			if (strncmp(n->syms[i]->name, "gen", 3) == 0) continue;
			n->syms[i]->name = stringf("gen_%s", n->syms[i]->name);
			n->syms[i]->x.name = n->syms[i]->name;
		}
}

/*
Generates a variable name for each instruction's result
*/
static void mugen_var(Node p, int child)
{
	if (p == NULL)
		return;
	if (child && optype(p->op) != B && (p->x.inst || generic(p->op) == INDIR)) {
		//should have a "register" allocated to it
		assert(p->syms[2]);
		p->syms[2] = newtemp(AUTO, optype(p->op), opsize(p->op));
		p->syms[2]->x.name = stringf("%%var_%s.%d", p->syms[2]->name, p->syms[2]->scope - LOCAL);
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
static Node mugen(Node forest)
{
	for (Node p = forest; p; p = p->link) {
		mugen_walk(p, NULL);
		mugen_fasgn(p, &forest);
	}

	gen(forest);

	for (Node p = forest; p; p = p->link)
		mugen_var(p, 0);

	return forest;
}

//XInterface funcs
static Symbol rmap(int opk)
{
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
	printf("//%s called\n", __FUNCTION__);
}
static void blkstore(int k, int off, int reg, int tmp)
{
	printf("//%s called\n", __FUNCTION__);
}
static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[])
{
	printf("//%s called\n", __FUNCTION__);
}

static Type node_type(Node p, bool_t arrays_as_ptrs)
{
	if (p == NULL)
		return NULL;
	int opg = generic(p->op);

	//if the op is a statement (inst executed for side effects) then its type is void
	if (opg == ARG || opg == ASGN ||
		 opg == LT || opg == LE || opg == EQ || opg == GE || opg == GT || opg == NE ||
		 opg == LABEL || opg == JUMP || p->op == CALL + V)
		return voidtype;

	if (optype(p->op) == B) {
		if (p->op == INDIR + B)
			return p->kids[0]->syms[0]->type;
		panic("unexpected B op in node_type (should only encounter INDIRB)");
		return voidtype;
	}
	if (optype(p->op) != P)
		return btot(p->op, opsize(p->op));

	Type t1 = node_type(p->kids[0], arrays_as_ptrs), t2 = node_type(p->kids[1], arrays_as_ptrs);
	switch (generic(p->op)) {
	case ADDRL:
	case ADDRF:
	case ADDRG:
		if (p->syms[0]->type == NULL) //for labels
			return voidtype;
		if (isarray(p->syms[0]->type) && arrays_as_ptrs)
			return ptr(ptr(indir(p->syms[0]->type)));
		return ptr(p->syms[0]->type);
	case INDIR:
		return t1->type;
	case CALL:
		return t1;
	case CNST:
		return p->syms[0]->type;
	case CVU:
		return voidptype;
	case SUB:
		if ((isptr(t1) || isarray(t1)) && (isptr(t2) || isarray(t2)))
			return longtype;
	case ADD:
		if (isptr(t1) || isarray(t1))
			return t1;
		else if (isptr(t2) || isarray(t2))
			return t2;
		else
			panic("unknown type for addp/subp (neither subtree was a ptr)");
		assert(false);
		break;
	case RET:
		return voidtype;
		break;
	default:
		assert(false);
		break;
	}

	return NULL;
}

static void emit_children(Node p)
{
	short *nts = _nts[_rule(p->x.state, p->x.inst)];
	if (p->kids[0])
		emitasm(p->kids[0], nts[0]);
	if (p->kids[1]) {
		printf(" ");
		emitasm(p->kids[1], nts[1]);
	}
}

static void emit_asgnp(Node p)
{
	Node k0 = p->kids[0], k1 = p->kids[1];
	Symbol s0 = k0->syms[0], s1 = k1->syms[0];
	Type t0 = node_type(k0, true), t1 = node_type(k1, true), t;
	//TODO: look at this. Still need to pin but can't assume global. also need to add a list of pinned objects that will get unpinned after func exit
	switch (generic(k1->op)) {
	case INDIR: //In this case there are some number of INDIR nodes until we get to an ADDR node so we need to perform some number of LOADs
		if (generic(k1->kids[0]->op) == ADDRL || generic(k1->kids[0]->op) == ADDRF || generic(k1->kids[0]->op) == ADDRG) {
			if (isptr(t0)) {
				printf("(STORE <@%s> ", type_name(t0));
				emit_children(p);
				printf(")\n");
			} else {
				printf("\t\t%s = PTRCAST <@%s @%s> ", s0->x.name, type_name(t1), type_name(t0));
				emit_children(k1);
			}
			printf("\n");
		} else if (generic(k1->kids[0]->op) == INDIR) {
			size_t indirs = 1;
			//we need to construct the chain of LOADs already knowing how many indirs there are since we need to know what type we are loading at each step
			while (generic((k1 = k1->kids[0])->op) == INDIR) indirs++;
			//at this point k1 should be an ADDR{L,F,G} and so k1->syms[0] is the var we are loading from
			s1 = k1->syms[0];
			Type base_t = unqual(s1->type);
			t = base_t;

			for (size_t i = indirs; i; i--) t = t->type;

			//initialize res to loc since if the tree is just INDIR(ADDR(var)) we will never enter the loop
			char *loc = s1->x.name, *res = loc;
			//TODO: the loop condition needs work: we don't want to get to the base type, we probably want to get to some less indirect type
			//i think the whole method of doing this needs to be reworked
			while ((t = ptr(t)) != base_t) {
				res = stringf("%d_tmp", genlabel(1));
				printf("\t\t%%%s = LOAD PTR <@%s> %%%s\n", res, type_name(t), loc);
				loc = res;
			}
			printf("\t\t%s = PTRCAST <@%s @%s> %%%s\n", XNAME(s0), type_name(t->type), type_name(s0->type), res);
		} else if (k1->kids[0]->x.inst) {
			short *nts = _nts[_rule(p->x.state, p->x.inst)];
			printf("\t\t%s = ", XNAME(s0));
			emitasm(k1, nts[1]);
			printf("\n");
		} else
			panic("ASGNP->kids[1] was an INDIRP but ASGNP->kids[1]->kids[0] was neither an INDIR/ADDR or an instruction so...");
		break;
	case ADDRL: //Tree is ASGNP(ADDR,ADDR)
		t = s1->type;
		char *tmp = stringf("%%%d_%s", genlabel(1), s0->x.name);
		/*this does need to be done in two steps (we can't just asign to the result) because the following example is valid:
		short *sp = NULL;
		char **cpp = (char **)&sp;
		*/
		printf("\t\t%s = COMMINST @uvm.native.pin <@%s> %s\n", tmp, type_name(t), XNAME(s1));
		printf("\t\t%s = PTRCAST <@%s @%s> %s\n", XNAME(s0), type_name(ptr(t)), type_name(s0->type), tmp);
		break;
	case CNST: //Tree is ASGNP(ADDR,CNST)
		printf("\t\t%s = PTRCAST <@%s @%s> @%s\n", XNAME(s0), type_name(voidptype), type_name(s0->type), const_name(voidptype, s1->x.name));
		break;
	default: //Tree is ASGNP(ADDR,?)
		if (k1->x.inst) {
			printf("\t\t%%%s = ", k0->syms[0]->x.name);
			emitasm(k1, _nts[_rule(p->x.state, p->x.inst)][1]);
			printf("\n");
		} else
			todo("ASGNP->kids[1] was not an instruction and op not recognized");
		break;
	}
}

//TODO: gen -> emit conversion nodes for INDIRI1(INDIRI4(...)) as well as ASGNP when node_type(k0) != node_type(k1)
static void emit2(Node p)
{
	Node k0 = p->kids[0], k1 = p->kids[1];
	Symbol s0, s1;
	Type t0 = node_type(k0, true), t1 = node_type(k1, true);
	switch (generic(p->op)) {
	case ADD:
		//TODO: figure out if ADDP(ADDR,...) or ADDP(...,ADDR) need anything different
		printf("(%s = PADD <@%s @%s> ", p->syms[2]->x.name, type_name(t0), type_name(t1));
		emit_children(p);
		printf(")\n");
		break;
	case ARG:
		muappend(&arg_list, p, STMT);
		break;
	case ASGN:
		if (optype(p->op) == B) {
			//ASGNB(addr, INDIRB(addr)) is the template, so we know exactly the structure of the tree and don't have to loop or anything
			Symbol global = k1->kids[0]->syms[0], var = k0->syms[0];
			if (!types_are_equal(global->type, var->type))
				todo("implement conversion for ASGNB");
			if (isstruct(var->type) || isarray(var->type)) {
				char *tmp = stringf("%%%d_tmp", genlabel(1));
				printf("\t\t%s = LOAD <@%s> @%s\n", tmp, type_name(global->type), global->name);
				printf("\t\tSTORE <@%s> %%%s_iref %s\n", type_name(var->type), var->name, tmp);
			} else
				panic("OP %d NOT RECOGNIZED", p->op);
		} else if (optype(p->op) == P) {
			emit_asgnp(p);
		} else {
			assert(isptr(t0));
			printf("STORE <@%s> //TODO\n", type_name(t0));
		}
		break;
	case CALL:
		char *sig, *ref;
		if (isaddr(k0->op)) {
			sig = k0->syms[0]->name;
			ref = stringf("@%s_ref", k0->syms[0]->name);
		} else { //k0 results in a ufuncptr
			sig = type_name(indir(node_type(k0, false)));
			ref = k0->syms[2]->x.name;
			//this load is in addition to any emited by the children
			printf("(%s = LOAD <@%s_ref> ", ref, sig);
			emit_children(p);
			printf(")\n");
		}
		printf("(%s = CALL <@%s_sig> %s (", p->syms[2]->x.name, sig, ref);
		while (arg_list != NULL) {
			emit_children(ELEM(Node, arg_list));
			if (arg_list = arg_list->next)
				printf(" ");
		}
		printf("))\n");
		break;
	case CNST:
		printf("@%s", const_name(p->syms[0]->type, p->syms[0]->name));
		break;
	case CVU:
		if (!isaddr(p->x.next->kids[0]->op))
			panic("expected CVUP->x.next to be an ASGN node with kids[0] being an ADDR node");
		printf("(%s = PTRCAST <@%s @%s> ", p->syms[2]->x.name, type_name(t0), type_name(p->x.next->kids[0]->syms[0]->type));
		emit_children(p);
		printf(")\n");
		break;
	case CVP:
		printf("(%s = PTRCAST <@%s @%s> ", p->syms[2]->x.name, type_name(t0), type_name(unsignedlong));
		emit_children(p);
		printf(")\n");
		break;
	case INDIR:
		//If
		if (types_are_equal(indir(t0), node_type(p, true))) {
			printf("(%s = LOAD PTR <@%s> ", p->syms[2]->x.name, type_name(node_type(p, false)));
			emit_children(p);
			printf(")\n");
		}

		break;
	case JUMP:
		if (generic(k0->op) == ADDRG)
			printf("\t\tBRANCH %%%s\n", k0->syms[0]->x.name);
		else
			printf("//TODO: implement branch to arbitrary locations (switch statements, for example)\n");
		break;
	default:
		todo("OP %d NOT RECOGNIZED", p->op);
		break;
	}

	if (p->x.registered)
		p->x.emitted = 1;
}
static void doarg(Node p) {}
static void target(Node p) {}
static void clobber(Node p) {}

/*! \relates munode

*/
static MuNode muprepend(MuNode *headp, void* e, int scope)
{
	MuNode n = NEW0(n, scope), head = *headp;
	if (head == NULL) {
		head = n;
		head->prev = head;
	} else
		n->next = head;

	*headp = n;
	n->prev = head->prev;
	head->prev = n;
	n->elem = e;

	return n;
}

/*! \relates munode

*/
static MuNode muappend(MuNode *headp, void* e, int scope)
{
	MuNode n = NEW0(n, scope), head = *headp;
	if (head == NULL) {
		head = n;
		*headp = head;
	} else
		//update old end of list
		head->prev->next = n;

	//put the new node at the end
	n->prev = head->prev;
	head->prev = n;

	n->elem = e;
	return n;
}
//TODO: do this at runtime since ptrs are a thing -> in mugen add a cast node to proper type
/// Prints references to all the fields of a struct or union.
/*!

*/
static void print_fields(Symbol s)
{
	Type t = unqual(s->type);
	if (!isstruct(t))
		return;

	size_t i = 0;
	if (isunion(t))
		for (Field f = t->u.sym->u.s.flist; f; f = f->link)
			printf("\t\t%%%s_iref_%s = REFCAST<@%s_iref @%s_iref> %%%s_iref\n", s->name, type_name(f->type), type_name(t), type_name(f->type), s->name);
	else {
		char *name;
		if (s->scope == GLOBAL)
			name = stringf(".global @%s", s->name);
		else
			name = stringf("\t\t%%%s", s->name);

		//alias p->name to the first field since LCC uses p->name as the address of the first elem
		printf("%s = GETFIELDIREF <@%s 0> %%%s_iref\n", name, type_name(t), s->name);
		for (Field f = t->u.sym->u.s.flist; f; f = f->link)
			if (f->bitsize && f->link && f->offset == f->link->offset)
				printf("%s_%s = GETFIELDIREF<@%s %zu> %s_iref\n", name, f->name, type_name(t), i, NAME(s));
			else
				printf("%s_%s = GETFIELDIREF<@%s %zu> %s_iref\n", name, f->name, type_name(t), i++, NAME(s));
	}
}

/// Checks if two types are structurally equal
/*!
	Returns true if t1 and t2 have the same base type, same number of levels of indirection, and that each level of indirection is the same (ptr or array).
	Ignores CONST/VOLITILE qualifiers.

		types_are_equal(inttype, const(inttype)) //true
		types_are_equal(ptr(inttype), array(inttype)) //false

	\param[in] t1 first type to compare
	\param[in] t2 second type to compare
	\return `true` if the two types are equal, `false` otherwise
*/
static bool_t types_are_equal(Type t1, Type t2)
{
	t1 = unqual(t1);
	t2 = unqual(t2);
	if (t1 == NULL || t2 == NULL) //we get here from one of the types being either an array or a ptr and the other not having as many levels of indirection
		return false;
	if (isptr(t1) && isptr(t2))
		return types_are_equal(t1->type, t2->type);
	if (isarray(t1) && isarray(t2))
		return (t1->size == t2->size) && types_are_equal(t1->type, t2->type);
	if (isfunc(t1) && isfunc(t2)) {
		if (t1->type != t2->type || t1->u.f.oldstyle || t2->u.f.oldstyle)
			return false;
		size_t i = 0;
		for (; t1->u.f.proto[i] && t2->u.f.proto[i]; i++)
			if (!types_are_equal(t1->u.f.proto[i], t2->u.f.proto[i]))
				return false;
		if (t1->u.f.proto[i] || t2->u.f.proto[i])
			return false;
		return true;
	}
	if (t1->u.sym && t2->u.sym && t1->u.sym == t2->u.sym)
		return true;
	return false;
}

/*define a new mu type's name and LCC type, but not it's mu type definition (effectively a C typedef)
MuTypes with NULL mutype are not emited
*/
static MuType def_type_partial(Type t, const char *name)
{
	MuNode mt_node = mutype_list;
	MuType mt;
	for (; mt_node; mt_node = mt_node->next)
		if (types_are_equal(t, (mt = ELEM(MuType, mt_node))->type))
			return mt;

	muappend(&mutype_list, NEW0(mt, PERM), PERM);

	mt->type = t;
	if (name)
		mt->name = string(name);
	return mt;
}

static MuType def_type(Type t, const char *name, const char *mu_t)
{
	MuType mt = def_type_partial(t, name);
	mt->mutype = string(mu_t);
	return mt;
}

static MuType get_mutype(Type t)
{
	if (t == longlong)
		t = longtype;
	else if (t == unsignedlonglong)
		t = unsignedlong;
	else if (t == longdouble)
		t = doubletype;
	t = unqual(t);
	for (MuNode mtnode = mutype_list; mtnode; mtnode = mtnode->next)
		if (types_are_equal(t, ELEM(MuType, mtnode)->type))
			return ELEM(MuType, mtnode);

	if (isptr(t)) {
		char *tname = type_name(t->type);
		if (isfunc(t->type))
			return def_type(t, stringf("fptr_%s", tname), stringf("ufuncptr<@%s_sig>", tname));
		return def_type(t, stringf("ptr_%s", tname), stringf("uptr<@%s>", tname));
	}
	if (isfunc(t)) {
		//generated naming convention for funcs is _<return type>__<arg type>(__<arg type>...)_
		Type *args = t->u.f.proto;
		char *fname = stringf("_%s_", type_name(t->type));
		for (size_t i = 0; args[i]; i++)
			fname = stringf("%s_%s_", fname, type_name(args[i]));
		def_func(fname, t, false);
		//def_type_partial is used since functions are not mu types, but it simplifies a lot if we lump them in while generating code
		return def_type_partial(t, fname);
	}
	if (isarray(t))
		return def_type(t, stringf("arr_%s_%d", type_name(t->type), t->size / t->type->size), stringf("array< @%s %d >", type_name(t->type), t->size / t->type->size));
	if (isunion(t)) {
		char *name = stringf(".sizeof #MAX @%s_sz (", t->u.sym->name);
		for (Field f = t->u.sym->u.s.flist; f; f = f->link)
			name = stringf("%s@%s ", name, type_name(f->type));
		name[strlen(name) - 1] = ')';
		muappend(&preproc_list, name, PERM);
		return def_type(t, t->u.sym->name, stringf("int<@%s_sz>", t->u.sym->name));
	}
	if (isstruct(t)) {
		MuType res = def_type_partial(t, t->u.sym->name);
		char *name = string("struct <");
		for (Field f = t->u.sym->u.s.flist; f; f = f->link) {
			/*
			LCC automatically generates instructions to extract bitfields.
			we only generate a single field for each "block" of bitfields
			*/
			if (f->bitsize && f->link && f->offset == f->link->offset)
				continue;
			name = stringf("%s@%s ", name, type_name(f->type));
		}
		name[strlen(name) - 1] = '>';
		res->mutype = string(name);
		return res;
	}

	assert(0); //base type that hasn't been defined so...
	return NULL;
}

static char *type_name(Type t)
{
	MuType mt = get_mutype(t);
	if (mt)
		return mt->name;
	return NULL;
}

static char *const_name(Type t, char *val)
{
	if (isptr(t) && string(val) == string("0"))
		val = string("NULL");
	char *tmp = val, *tmp2 = val;
	do {
		char c = *tmp;
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '.' || c == '-')
			continue;
		*tmp = '.';
	} while (*(++tmp) != 0);
	char *v = string(tmp2);
	MuNode mcnode = muconst_list;
	MuConst c;
	for (; mcnode; mcnode = mcnode->next)
		if (types_are_equal(ELEM(MuConst, mcnode)->type, t) && ELEM(MuConst, mcnode)->val == v)
			return ELEM(MuConst, mcnode)->name;

	muappend(&muconst_list, NEW0(c, PERM), PERM);
	c->name = stringf("%s_%s", type_name(t), v);
	c->val = v;
	c->type = t;
	return c->name;
}