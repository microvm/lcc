/* vim: set ft=c ts=4 sw=4 noexpandtab: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c.h"

#define J(x) json_ ## x
#define dJ(x) static void J(x)

#define UNLESS(x) if (!(x))

dJ(address)(Symbol, Symbol, long);
dJ(blockbeg)(Env *);
dJ(blockend)(Env *);
dJ(defaddress)(Symbol);
dJ(defconst)(int, int, Value);
dJ(defstring)(int, char *);
dJ(defsymbol)(Symbol);
dJ(emit)(Node);
dJ(export)(Symbol);
dJ(function)(Symbol, Symbol[], Symbol[], int);
static Node J(gen)(Node);
dJ(global)(Symbol);
dJ(import)(Symbol);
dJ(local)(Symbol);
dJ(progbeg)(int, char *[]);
dJ(progend)(void);
dJ(segment)(int);
dJ(space)(int);
dJ(stabblock)(int, int, Symbol *);
dJ(stabend)(Coordinate *, Symbol, Coordinate **, Symbol *, Symbol *);
dJ(stabfend)(Symbol, int);
dJ(stabinit)(char *, int, char *[]);
dJ(stabline)(Coordinate *);
dJ(stabsym)(Symbol);
dJ(stabtype)(Symbol);
dJ(emitconst)();

/* size summary */
Interface json64IR = {
	1, 1, 0,	/* char */
	2, 1, 0,	/* short */
	4, 1, 0,	/* int */
	8, 1, 0,	/* long */
	8, 1, 0,	/* long long */
	4, 1, 0,	/* float */
	8, 1, 0,	/* double */
	8, 1, 0,	/* long double */
	8, 1, 0,	/* pointer */
	0, 1, 0,	/* struct */
	1,		/* little endian */
	0,		/* multiply, divide, remainder intrinsic */
	0,		/* decompose calls of structure returning functions to simpler operations */
	0,		/* decompose calls of structure-taking functions to simpler operations */
	1,		/* left-to-right argument evaluation */
	1,		/* eliminate common subexpressions in front end */
	0,		/* unsigned characters */
	NULL,	/* no address() needed */
	J(blockbeg),
	J(blockend),
	J(defaddress),
	J(defconst),
	J(defstring),
	J(defsymbol),
	J(emit),
	J(export),
	J(function),
	J(gen),
	J(global),
	J(import),
	J(local),
	J(progbeg),
	J(progend),
	J(segment),
	J(space),
	J(stabblock),
	J(stabend),
	J(stabfend),
	J(stabinit),
	J(stabline),
	J(stabsym),
	J(stabtype)
};

#define PRINT_JSON_BOOLEAN(key, value)	print("\""key"\": %s,\n", (value) ? "true" : "false")
#define JSON_COMMA(first) ((first) ? "" : ", ")

static Symbol current_function=NULL;
static void print_symbol(Symbol);

/* For numbered types, mainly function types */
static unsigned int type_counter=0;
/* May mutate type */
static char *type_name(Type t)
{
	if (isarith(t)) return t->u.sym->name;
	if (isenum(t)) return t->u.sym->name;
	if ((t==voidtype) || (VOID == t->op)) return t->u.sym->name;
	if (isstruct(t) || isunion(t)) return t->u.sym->name;

	if (isarray(t)) {
		UNLESS (t->u.sym) {
			/* doesn't already have a symbol associated, so we'll note the name in a new symbol */
			t->u.sym=calloc(1, sizeof(struct symbol));
			UNLESS (t->u.sym) {
				error("Failed to allocate\n");
			}
		}

		UNLESS (t->u.sym->name) {
			/* Construct a new name by prepending [ to the element type's name */
			char *orig_name=type_name(t->type);
			int orig_len=strlen(orig_name);

			t->u.sym->name=calloc(12+orig_len, sizeof(char));
			UNLESS (t->u.sym->name) {
				error("Failed to allocate\n");
			}

			UNLESS (t->type->size) {
				error("Unknown size for type\n");
			}
			snprintf(t->u.sym->name, 12, "[%d", t->size/t->type->size);
			strncat(t->u.sym->name, orig_name, orig_len);
		}

		return t->u.sym->name;
	}

	if (isptr(t)) {
		/* Pointer types, as exported by the front end, share the same symbol. We'll fix that. */
		char *refname;
		int reflen;
		Symbol sym=calloc(1, sizeof(struct symbol));

		UNLESS (sym) {
			error("Failed to allocate\n");
		}
		memcpy(sym, t->u.sym, sizeof(struct symbol));

		refname=type_name(t->type);
		reflen=strlen(refname);
		sym->name=calloc(2+reflen, sizeof(char));
		UNLESS (sym->name) {
			error("Failed to allocate\n");
		}

		*(sym->name)='*';
		strncat(sym->name, refname, reflen);

		t->u.sym=sym;

		return t->u.sym->name;
	}

	/* Function types don't have symbols attached, but we can use part of the
	*  Xtype field (which the front end initializes to zero, as it happens)
	*/
	if (isfunc(t)) {
		UNLESS (t->x.xt) {
			t->x.xt=calloc(14, sizeof(char));
			UNLESS (t->x.xt) {
				error("Failed to allocate\n");
			}

			snprintf(t->x.xt, 14, "type%u", type_counter++);
			UNLESS (type_counter) {
				error("Overflow in type counter\n");
			}
		}

		return t->x.xt;
	}

	/* Qualified (const and/or volatile) versions of other types aren't allocated symbols by the front end */
	if (isconst(t) || isvolatile(t)) {
		UNLESS (t->u.sym) {
			char *qualifier;
			char *qualified;
			int qlen;
			char *ret;
			int diff=isconst(t)-isvolatile(t);

			qualified=type_name(t->type);
			qlen=strlen(qualified);

			if (diff > 0) {
				qualifier="const ";
				ret=calloc(7+qlen, sizeof(char));
			} else if (diff < 0) {
				qualifier="volatile ";
				ret=calloc(10+qlen, sizeof(char));
			} else {
				qualifier="const_volatile ";
				ret=calloc(16+qlen, sizeof(char));
			}

			UNLESS (ret) {
				error("Failed to allocate\n");
			}

			strncpy(ret, qualifier, 16);
			strncat(ret, qualified, qlen);

			t->u.sym=calloc(1, sizeof(struct symbol));
			UNLESS (t->u.sym) {
				error("Failed to allocate\n");
			}

			t->u.sym->name=ret;
		}

		return t->u.sym->name;
	}

	error("Uncovered type!");
}

struct worklist_node {
	void *item;
	char *name;
	struct worklist_node *next;
};

static struct worklist_node *type_worklist=NULL;
static struct worklist_node *symbol_worklist=NULL;
static struct worklist_node *node_worklist=NULL;

static void add_type_to_worklist(Type t)
{
	struct worklist_node *cur;
	struct worklist_node *nu;

	UNLESS (t) {
		return;
	}

	cur=type_worklist;
	while (cur) {
		if (cur->item == t) {
			return;
		}
		cur=cur->next;
	}

	nu=calloc(1, sizeof(struct worklist_node));
	UNLESS (nu) {
		error("Failed to allocate\n");
	}

	nu->item=t;
	nu->name=type_name(t);
	nu->next=type_worklist;
	type_worklist=nu;
}

static char *symbol_name(Symbol);

static void add_symbol_to_worklist(Symbol s)
{
	struct worklist_node *cur;
	struct worklist_node *nu;

	UNLESS (s) {
		return;
	}

	cur=symbol_worklist;
	while (cur) {
		if (cur->item == s) {
			return;
		}
		cur=cur->next;
	}

	nu=calloc(1, sizeof(struct worklist_node));
	UNLESS (nu) {
		error("Failed to allocate\n");
	}

	nu->item=s;
	nu->name=symbol_name(s);
	nu->next=symbol_worklist;
	symbol_worklist=nu;
}

static void print_symbol(Symbol);
static void print_types();

/* Prevent re-entry of print_symbols */
static int printing_symbols=0;

static void print_symbols() {
	if (printing_symbols) {
		return;
	}

	UNLESS (symbol_worklist) {
		return;
	}

	printing_symbols=1;
	while (symbol_worklist) {
		struct worklist_node *cur=symbol_worklist;
		symbol_worklist=cur->next;

		Symbol s=(Symbol)(cur->item);
		UNLESS (s->x.printed) {
			print_symbol(s);
		}
	}
	printing_symbols=0;

	print_types();
}

static void add_node_to_worklist(Node n)
{
	struct worklist_node *cur;
	struct worklist_node *nu;

	UNLESS (n) {
		return;
	}

	cur=node_worklist;
	while (cur) {
		if (cur->item == n) {
			return;
		}
		cur=cur->next;
	}

	nu=calloc(1, sizeof(struct worklist_node));
	UNLESS (nu) {
		error("Failed to allocate\n");
	}

	nu->item=n;
	nu->next=node_worklist;
	node_worklist=nu;
}

static int printing_nodes=0;

static void print_node(Node);

static void print_nodes() {
	if (printing_nodes) {
		return;
	}

	UNLESS (node_worklist) {
		return;
	}

	printing_nodes=1;
	while (node_worklist) {
		struct worklist_node *cur=node_worklist;
		node_worklist=cur->next;

		Node n=(Node)(cur->item);
		UNLESS (n->x.emitted) {
			print_node(n);
			n->x.emitted=1;
		}
	}
	printing_nodes=0;

	print_types();
	print_symbols();
}

static void print_type(Type t) {
	char *opstring;
	UNLESS (t) {
		return;
	}

	switch (t->op) {
		case INT:		opstring="INT";			break;
		case UNSIGNED:	opstring="UNSIGNED";	break;
		case FLOAT:		opstring="FLOAT";		break;
		case ENUM:		opstring="ENUM";		break;
		case ARRAY:		opstring="ARRAY";		break;
		case STRUCT:	opstring="STRUCT";		break;
		case UNION:		opstring="UNION";		break;
		case POINTER:	opstring="POINTER";		break;
		case FUNCTION:	opstring="FUNCTION";	break;
		case VOID:		opstring="VOID";		break;
		case CONST:		opstring="CONST";		break;
		case VOLATILE:	opstring="VOLATILE";	break;
		case CONST+VOLATILE:
			opstring="CONST+VOLATILE";	break;
		default:
			error("Unkown type operator\n");
	}

	print("{\n");
	print("\"op\": \"%s\",\n", opstring);
	if (t->type) {
		print("\"type\": \"t:%s\",\n", type_name(t->type));
		add_type_to_worklist(t->type);
	}
	print("\"align\": %d,\n", t->align);
	print("\"size\": %d,\n", t->size);

	if (isenum(t)) {
		Symbol *sp=t->u.sym->u.idlist;
		int first;

		if (sp && *(sp)) {
			print("\"enum-values\": [ ");
		}

		first=1;
		while (sp && *(sp)) {
			print("%s\"%s\"", JSON_COMMA(first), symbol_name(*sp));
			first=0;
			add_symbol_to_worklist(*(sp));
			sp++;
		}
		print(" ],\n");
		print("\"type\": \"%s\",\n", type_name(t->type));
		add_type_to_worklist(t->type);
	} else if (isstruct(t) || isunion(t)) {
		Field fp=t->u.sym->u.s.flist;
		int first;

		print("\"fields\": [ ");

		first=1;
		while (fp) {
			print("%s{\n", JSON_COMMA(first));
			first=0;
			print("\"name\": \"%s\",\n", fp->name);
			print("\"type\": \"%s\",\n", type_name(fp->type));
			add_type_to_worklist(fp->type);
			print("\"offset\": %d,\n", fp->offset);
			print("\"bitsize\": %d,\n", fp->bitsize);
			print("\"lsb\": %d,\n", fp->lsb);
			print("}\n");

			fp=fp->link;
		}

		print(" ],\n");
	} else if (isfunc(t)) {
		PRINT_JSON_BOOLEAN("oldstyle", t->u.f.oldstyle);
		UNLESS (t->u.f.oldstyle) {
			int first;
			Type *tp=t->u.f.proto;

			print("\"proto\": [ ");

			first=1;
			while (tp && (*tp)) {
				print("%s\"t:%s\"", JSON_COMMA(first), type_name(*tp));
				first=0;
				add_type_to_worklist(*(tp));
				tp++;
			}

			print(" ]\n");
		}
	}

	print("\"done\": 1},\n");
}

/* Prevent re-entry of print_types */
static int printing_types=0;

static void print_types() {
	if (printing_types) {
		return;
	}

	UNLESS (type_worklist) {
		return;
	}

	printing_types=1;
	while (type_worklist) {
		struct worklist_node *cur=type_worklist;
		type_worklist=cur->next;

		Type t=(Type)(cur->item);
		UNLESS (t->x.printed) {
			print("\"t:%s\": ", cur->name);
			print_type(t);
			t->x.printed=1;
		}
	}
	printing_types=0;

	print_symbols();
}

static void print_symbol_variable(Symbol p)
{
	char *sclass="";

	print("{\n");
	print("\"name\": \"%s\",\n", p->name);

	switch (p->sclass) {
		case STATIC:
			sclass="STATIC";
			break;
		case EXTERN:
			sclass="EXTERN";
			break;
		case AUTO:
			sclass="AUTO";
			break;
		case REGISTER:
			sclass="REGISTER";
			break;
		case ENUM:
			sclass="ENUM";
			print("\"const\": {\"v\": %d},\n", p->u.value);
			print("}\n");
			return;
		default:
			error("unknown sclass %d\n", p->sclass);
			return;
	}

	print("\"sclass\": \"%s\",\n", sclass);
	/* structarg should never be set, since wants_argb is false */
	PRINT_JSON_BOOLEAN("addressed", p->addressed);
	PRINT_JSON_BOOLEAN("computed", p->computed);
	PRINT_JSON_BOOLEAN("temporary", p->temporary);
	PRINT_JSON_BOOLEAN("generated", p->generated);
	PRINT_JSON_BOOLEAN("defined", p->defined);
	if (p->type) {
		print("\"type\": \"t:%s\",\n", type_name(p->type));
		add_type_to_worklist(p->type);
	}
	if ((GLOBAL == p->scope) &&
		(p->src.file))	{
		print("\"defined_in\": \"%s\",\n", p->src.file);
	}

	print("\"done\": 1}\n");
}

static void print_symbol_label(Symbol p)
{
	print("{\n");

	print("\"label\": %d\n", p->u.l.label);

	print("}\n");
}

static void print_symbol_constant(Symbol p)
{
	print("{\n");

	print("\"sclass\": \"STATIC\",\n");
	print("\"const\": {\n");
	print("\"type\": \"t:%s\",\n", type_name(p->type));
	add_type_to_worklist(p->type);
	if (isunsigned(p->type)) {
		print("\"v\": %u,\n", p->u.c.v.u);
	} else if (isint(p->type)) {
		print("\"v\": %d,\n", p->u.c.v.i);
	} else if (isfloat(p->type)) {
		print("\"v\": %f,\n", p->u.c.v.d);
	} else if (isfunc(p->type)) {
		print("\"v\": \"%p\",\n", p->u.c.v.g);
	} else if (isptr(p->type)) {
		print("\"v\": \"%p\",\n", p->u.c.v.p);
	} else {
		error("Unknown constant type\n");
		return;
	}

	if (p->u.c.loc) {
		print("\"loc\": \"s:g:%s\",\n", p->u.c.loc->name);
	}
	print("\"done\": 1}\n");

	print("}\n");
}

static int anon_symbol_count=0;

static char *symbol_name(Symbol p)
{
	int baselen;

	if (p->x.name) {
		return p->x.name;
	}

	UNLESS (p->name) {
		p->name=calloc(12, sizeof(char));
		UNLESS (p->name) {
			error("Failed to allocate\n");
		}
		snprintf(p->name, 11, "%%%u", anon_symbol_count++);
	}
	baselen=strlen(p->name);

	if (GLOBAL == p->scope) {
		p->x.name=calloc(5+baselen, sizeof(char));
		UNLESS (p->x.name) {
			error("Failed to allocate\n");
		}
		snprintf(p->x.name, 5+baselen, "s:g:%s", p->name);
	} else if (PARAM == p->scope) {
		int funclen=strlen(current_function->name);
		p->x.name=calloc(8+p->x.caller+funclen+baselen, sizeof(char));
		UNLESS (p->x.name) {
			error("Failed to allocate\n");
		}
		snprintf(p->x.name, 8+p->x.caller+funclen+baselen, "s:p%s:(%s) %s", (p->x.caller ? "r" : ""), current_function->name, p->name);
	} else if (LOCAL <= p->scope) {
		int funclen=strlen(current_function->name);
		p->x.name=calloc(18+funclen+baselen, sizeof(char));
		UNLESS (p->x.name) {
			error("Failed to allocate\n");
		}
		snprintf(p->x.name, 18+funclen+baselen, "s:%d:(%s) %s", p->scope - LOCAL, current_function->name, p->name);
	} else if (LABELS == p->scope) {
		p->x.name=calloc(5+baselen, sizeof(char));
		UNLESS (p->x.name) {
			error("Failed to allocate\n");
		}
		snprintf(p->x.name, 5+baselen, "s:l:%s", p->name);
	} else if (CONSTANTS == p->scope) {
		if (isint(p->type) || isptr(p->type)) {
			p->x.name=calloc(5+baselen, sizeof(char));
			UNLESS (p->x.name) {
				error("Failed to allocate\n");
			}
			snprintf(p->x.name, 5+baselen, "s:c:%s", p->name);
		} else {
			if (4 == p->type->size) {
				p->x.name=stringf("s:c:%f", (float)(p->u.c.v.d));
			} else {
				p->x.name=stringf("s:c:%f", p->u.c.v.d);
			}
		}
	} else {
		/* error("unknown symbol with scope %d", p->scope); */
		p->x.name=calloc(5+baselen, sizeof(char));
		UNLESS (p->x.name) {
			error("Failed to allocate\n");
		}
		snprintf(p->x.name, 5+baselen, "s:u:%s", p->name);
		fprintf(stderr, "Unknown symbol encountered\n");
		fprintf(stderr, "name: %s\n", p->name);
		fprintf(stderr, "scope: %d\n", p->scope);
		fprintf(stderr, "src: { %s, %d, %d }\n", p->src.file, p->src.y, p->src.x);
		fprintf(stderr, "sclass: %d\n", p->sclass);
	}

	return p->x.name;
}

static void print_symbol(Symbol p)
{
	if (p->x.printed) {
		return;
	}

	p->x.printed=1;

	print("\"%s\": ", symbol_name(p));

	if (p->scope == GLOBAL) {
		print_symbol_variable(p);
	} else if (p->scope == PARAM) {
		print_symbol_variable(p);
	} else if (p->scope >= LOCAL) {
		print_symbol_variable(p);
	} else if (p->scope == LABELS) {
		print_symbol_label(p);
	} else if (p->scope == CONSTANTS) {
		print_symbol_constant(p);
	}
	print(",\n");

	print_types();
}

static int node_count=1;

static void number_node(Node n) {
	UNLESS (n->x.num) {
		n->x.num=node_count;
		node_count++;
	}
}

#define OPSTR_CASE(x, str, var)	case x:	var=str; break;
static char *opstring(Node n)
{
	char *opstr;
	int generic_op;

	switch (generic_op=generic(n->op)) {
		OPSTR_CASE(ADDRF, "ADDRF", opstr);
		OPSTR_CASE(ADDRG, "ADDRG", opstr);
		OPSTR_CASE(ADDRL, "ADDRL", opstr);
		OPSTR_CASE(CNST, "CNST", opstr);
		OPSTR_CASE(BCOM, "BCOM", opstr);
		OPSTR_CASE(CVF, "CVF", opstr);
		OPSTR_CASE(CVI, "CVI", opstr);
		OPSTR_CASE(CVP, "CVP", opstr);
		OPSTR_CASE(CVU, "CVU", opstr);
		OPSTR_CASE(INDIR, "INDIR", opstr);
		OPSTR_CASE(NEG, "NEG", opstr);
		OPSTR_CASE(ADD, "ADD", opstr);
		OPSTR_CASE(BAND, "BAND", opstr);
		OPSTR_CASE(BOR, "BOR", opstr);
		OPSTR_CASE(BXOR, "BXOR", opstr);
		OPSTR_CASE(DIV, "DIV", opstr);
		OPSTR_CASE(LSH, "LSH", opstr);
		OPSTR_CASE(MOD, "MOD", opstr);
		OPSTR_CASE(MUL, "MUL", opstr);
		OPSTR_CASE(RSH, "RSH", opstr);
		OPSTR_CASE(SUB, "SUB", opstr);
		OPSTR_CASE(ASGN, "ASGN", opstr);
		OPSTR_CASE(EQ, "EQ", opstr);
		OPSTR_CASE(GE, "GE", opstr);
		OPSTR_CASE(GT, "GT", opstr);
		OPSTR_CASE(LE, "LE", opstr);
		OPSTR_CASE(LT, "LT", opstr);
		OPSTR_CASE(NE, "NE", opstr);
		OPSTR_CASE(ARG, "ARG", opstr);
		OPSTR_CASE(CALL, "CALL", opstr);
		OPSTR_CASE(RET, "RET", opstr);
		OPSTR_CASE(JUMP, "JUMP", opstr);
		OPSTR_CASE(LABEL, "LABEL", opstr);
		default:
			error("Unknown operator: %d\n", generic_op);
			return "";
	}

	return opstr;
}

static void print_node(Node n)
{
	char *opstr;
	char *opsuf;
	int generic_op;
	int op_suffix;

	number_node(n);

	print("\"n:%u\": {\n", n->x.num);
	opstr=opstring(n);
	generic_op=generic(n->op);
	switch (op_suffix=optype(n->op)) {
		OPSTR_CASE(F, "F", opsuf);
		OPSTR_CASE(I, "I", opsuf);
		OPSTR_CASE(U, "U", opsuf);
		OPSTR_CASE(P, "P", opsuf);
		OPSTR_CASE(V, "V", opsuf);
		OPSTR_CASE(B, "B", opsuf);
		default:
			error("Unknown operator type: %d\n", op_suffix);
	}
	print("\"op\": \"%s\",\n", opstr);
	print("\"optype\": \"%s\",\n", opsuf);
	print("\"opsize\": %d,\n", opsize(n->op));

	UNLESS ((generic_op == ADDRF) ||
			(generic_op == ADDRG) ||
			(generic_op == ADDRL) ||
			(generic_op == CNST) ||
			((generic_op == RET) && (op_suffix == V)) ||
			(generic_op == LABEL)) {
		UNLESS (n->kids[0]->x.num) {
			n->kids[0]->x.num=node_count;
			node_count++;
		}

		print("\"kid1\": \"n:%u\",\n", n->kids[0]->x.num);
		add_node_to_worklist(n->kids[0]);
	}

	if ((generic_op == ADD) ||
		(generic_op == BAND) ||
		(generic_op == BOR) ||
		(generic_op == BXOR) ||
		(generic_op == DIV) ||
		(generic_op == LSH) ||
		(generic_op == MOD) ||
		(generic_op == MUL) ||
		(generic_op == RSH) ||
		(generic_op == SUB) ||
		(generic_op == ASGN) ||
		(generic_op == EQ) ||
		(generic_op == GE) ||
		(generic_op == GT) ||
		(generic_op == LE) ||
		(generic_op == LT) ||
		(generic_op == NE) ||
		((generic_op == CALL) && (op_suffix == B))) {
		UNLESS (n->kids[1]->x.num) {
			n->kids[1]->x.num=node_count;
			node_count++;
		}

		print("\"kid2\": \"n:%u\",\n", n->kids[1]->x.num);
		add_node_to_worklist(n->kids[1]);
	}

	/* CALL nodes have a symbol, but it's only used for type information */
	if (generic_op == CALL) {
		print("\"type\": \"t:%s\",\n", type_name(n->syms[0]->type));
		add_type_to_worklist(n->syms[0]->type);
	}

	UNLESS ((generic_op == INDIR) ||
			(generic_op == NEG) ||
			(generic_op == ADD) ||
			(generic_op == BAND) ||
			(generic_op == BOR) ||
			(generic_op == BXOR) ||
			(generic_op == DIV) ||
			(generic_op == LSH) ||
			(generic_op == MOD) ||
			(generic_op == MUL) ||
			(generic_op == RSH) ||
			(generic_op == SUB) ||
			(generic_op == RET) ||
			(generic_op == JUMP) ||
			(generic_op == CALL)) {
		print("\"sym1\": \"%s\",\n", (n->syms[0]->x.name) ? n->syms[0]->x.name : symbol_name(n->syms[0]));
		add_symbol_to_worklist(n->syms[0]);
	}
	if ((generic_op == ASGN) ||
		(generic_op == ARG)) {
		print("\"sym2\": \"%s\",\n", (n->syms[1]->x.name) ? n->syms[1]->x.name : symbol_name(n->syms[1]));
		add_symbol_to_worklist(n->syms[1]);
	}
	print("\"done\": 1},\n");

	print_nodes();
	print_symbols();
}

/* progbeg: called at the beginning of compilation
 *  argc, argv: refer to the command-line arguments passed to this invocation of the compiler
 */
dJ(progbeg)(int argc, char *argv[])
{
	print("{\n");
}

/* progend: called at the end of compilation */
dJ(progend)(void)
{
	print_symbols();
	print_types();
	print_nodes();
	print("\"done\": 1}\n");
}

Symbol lastsym=NULL;
int cseg;

/* defsymbol: called to define a new symbol with scope CONSTANTS, LABELS, or GLOBAL, or a static variable */
dJ(defsymbol)(Symbol p)
{
	add_symbol_to_worklist(p);
}

/* export: announce an exported symbol */
dJ(export)(Symbol p)
{
	/* Not relevant to MicroVM */
}

/* import: announce an imported symbol */
dJ(import)(Symbol p)
{
	/* Not relevant to MicroVM */
}

/* global: define a global variable */
dJ(global)(Symbol p)
{
	add_symbol_to_worklist(p);
	switch (cseg) {
		case LIT:
		case DATA:
			lastsym=p;
			break;
		default:
			break;
	}
}

/* local: define a local variable */
dJ(local)(Symbol p)
{
	/* For now, it's just a symbol announcement */
	add_symbol_to_worklist(p);
}

/* address: creates an address in p for a base (from q) and offset */
dJ(address)(Symbol p, Symbol q, long n)
{
	/* Not actually sure if we really need this for MicroVM */
	/* NULLed out in the interface record */
}

struct byte_block {
	Symbol sym;
	int suffix;
	int size;
	int count;
	int cap;
	Value *arr;
};

static struct byte_block *bb=NULL;

/* segment: announce that we're in a new segment */
dJ(segment)(int seg)
{
	print_nodes();
	if (bb) {
		J(emitconst)();
	}
	cseg=seg;
}

/* defaddress: initialize an address */
dJ(defaddress)(Symbol p)
{
	/* Not really relevant to MicroVM */
}

static int block_count=0;

dJ(emitconst)()
{
	int i;
	Value *v=bb->arr;

	print("\"b:%u\": {\n", block_count);
	print("\"symbol\": \"%s\",\n", symbol_name(bb->sym));
	print("\"contents\": [ ");

	for (i=0; i<bb->count; i++, v++) {
		int szof;

		print("%s", JSON_COMMA(!i));

		switch (bb->suffix) {
			case F:		/* float, double, long double */
				switch (bb->size) {
					case 4:	/* float */
						print("%f", (float)(v->d));
						break;
					case 8: /* double, long double */
						print("%f", v->d);
						break;
					default:
						error("Unknown floating point size %d\n", bb->size);
				}
				break;
			case I:		/* char, short, int, long, long long */
				switch (bb->size) {
					case 1:	/* char */
						print("%d", (char)(v->i));
						break;
					case 2:	/* short */
						print("%d", (short)(v->i));
						break;
					case 4:
						print("%d", (int)(v->i));
						break;
					case 8:
						print("%d", v->i);
						break;
					default:
						error("Unknown integer size %d\n", bb->size);
				}
				break;
			case U:		/* unsigned integers of various sizes */
				switch (bb->size) {
					case 1:
						print("%u", (unsigned char)(v->u));
						break;
					case 2:
						print("%u", (unsigned short)(v->u));
						break;
					case 4:
						print("%u", (unsigned int)(v->u));
						break;
					case 8:
						print("%u", v->u);
						break;
					default:
						error("Unknown unsigned integer size %d\n", bb->size);
				}
				break;
			case P:
				error("Pointer constants not supported\n");
				break;
		}
	}
	print(" ]},\n");
	block_count++;

	if (bb->arr) {
		free(bb->arr);
	}
	free(bb);
	bb=NULL;
}

dJ(allocbb)()
{
	bb=calloc(1, sizeof(struct byte_block));

	UNLESS (bb) {
		error("Failed to allocate\n");
	}
}

/* defconst: define a constant */
dJ(defconst)(int suffix, int size, Value v)
{
	UNLESS (bb) {
		J(allocbb)();

		UNLESS (lastsym) {
			error("Attempt to defconst without announcing symbol\n");
			return;
		}
	}

	if (bb->sym != lastsym) {
		if (bb->sym) {
			/* Finished defining previous symbol, so emit and free */
			J(emitconst)();

			J(allocbb)();
		}

		bb->sym=lastsym;
		bb->suffix=suffix;
		bb->size=size;
		bb->count=0;
		bb->cap=1;
		bb->arr=calloc(bb->cap, sizeof(Value));
	}

	if ((suffix != bb->suffix) ||
		(size != bb->size)) {
		error("Type mismatch in constant %s\n", (bb->sym->name) ? bb->sym->name : "");
	}

	if (bb->count >= bb->cap) {
		Value *tmp=calloc(2*(bb->count), sizeof(Value));

		UNLESS (tmp) {
			error("Failed to allocate\n");
		}

		memcpy(tmp, bb->arr, bb->count * sizeof(Value));
		free(bb->arr);
		bb->arr=tmp;
		bb->cap=2*bb->count;
	}

	memcpy(bb->arr + bb->count, &v, sizeof(Value));
	bb->count++;
}

/* defstring: initialize a string constant */
dJ(defstring)(int n, char *s)
{
	int i;

	print("\"b:%u\": {\n", block_count);
	print("\"symbol\": \"%s\",\n", symbol_name(lastsym));
	print("\"contents\": [ ");

	for (i=0; i<n; i++, s++) {
		print("%s%u", JSON_COMMA(!i), (unsigned char)(*s));
	}

	print(" ]},\n");
	block_count++;
}

/* space: set aside n zero bytes */
dJ(space)(int n)
{
	/* ???? */
}

struct nodelist {
	Node n;
	struct nodelist *next;
};

static struct nodelist *body=NULL;
static struct nodelist *tail=NULL;

/* function: generate and emit code for a function */
dJ(function)(Symbol f, Symbol caller[], Symbol callee[], int ncalls)
{
	Symbol *cursym;
	struct nodelist *curnode;
	int first;

	current_function=f;
	body=NULL;
	add_symbol_to_worklist(f);

	gencode(caller, callee);

	print("\"f:%s\": {\n\"sym\": \"%s\",\n", f->name, symbol_name(f));
	print("\"caller\": [ ");
	cursym=caller;
	first=1;
	while (cursym && *cursym) {
		(*cursym)->x.caller=1;
		print("%s\"%s\"", JSON_COMMA(first), symbol_name(*cursym));
		first=0;
		add_symbol_to_worklist(*cursym);
		cursym++;
	}
	print(" ],\n");
	print("\"callee\": [ ");
	cursym=callee;
	first=1;
	while (cursym && *cursym) {
		print("%s\"%s\"", JSON_COMMA(first), symbol_name(*cursym));
		first=0;
		add_symbol_to_worklist(*cursym);
		cursym++;
	}
	print(" ],\n");
	print("\"body\": [ ");
	curnode=body;
	first=1;
	while (curnode) {
		struct nodelist *next;
		print("%s\"n:%u\"", JSON_COMMA(first), curnode->n->x.num);
		first=0;
		next=curnode->next;
		free(curnode);
		curnode=next;
	}
	print(" ]\n");
	print("},\n");

	emitcode();

	print_symbols();

	current_function=NULL;
}

/* emit: emits dags in list p */
dJ(emit)(Node p)
{
	while (p) {
		Node i=p;
		while (i) {
			add_node_to_worklist(i);
			i=i->x.next;
		}
		p=p->link;
	}

	print_nodes();
}

/* gen: generate code for dags in list p */
static Node J(gen)(Node p)
{
	Node i=p;
	while (i) {
		number_node(i);

		struct nodelist *nu=calloc(1, sizeof(struct nodelist));
		nu->n=i;
		nu->next=NULL;

		if (body) {
			tail->next=nu;
			tail=nu;
		} else {
			body=tail=nu;
		}

		i=i->link;
	}
	return p;
}

/* blockbeg: start a block */
dJ(blockbeg)(Env *e)
{
}

/* blockend: finish a block */
dJ(blockend)(Env *e)
{
}

/* Symbol table functions */
dJ(stabblock)(int brace, int lev, Symbol *p)
{
}

dJ(stabend)(Coordinate *cp, Symbol p, Coordinate **cpp, Symbol *sp, Symbol *stab)
{
}

dJ(stabfend)(Symbol p, int lineno)
{
}

dJ(stabinit)(char *file, int argc, char *argv[])
{
}

dJ(stabline)(Coordinate *cp)
{
}

dJ(stabsym)(Symbol p)
{
}

dJ(stabtype)(Symbol p)
{
}
