%{
#include "../src/c.h"
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
%}

%start stmt

%term CNSTF4=4113 CNSTF8=8209
%term CNSTI1=1045 CNSTI2=2069 CNSTI4=4117 CNSTI8=8213
%term CNSTP8=8215
%term CNSTU1=1046 CNSTU2=2070 CNSTU4=4118 CNSTU8=8214

%term ARGB=41
%term ARGF4=4129 ARGF8=8225
%term ARGI4=4133 ARGI8=8229
%term ARGP8=8231
%term ARGU4=4134 ARGU8=8230

%term ASGNB=57
%term ASGNF4=4145 ASGNF8=8241
%term ASGNI1=1077 ASGNI2=2101 ASGNI4=4149 ASGNI8=8245
%term ASGNP8=8247
%term ASGNU1=1078 ASGNU2=2102 ASGNU4=4150 ASGNU8=8246

%term INDIRB=73
%term INDIRF4=4161 INDIRF8=8257
%term INDIRI1=1093 INDIRI2=2117 INDIRI4=4165 INDIRI8=8261
%term INDIRP8=8263
%term INDIRU1=1094 INDIRU2=2118 INDIRU4=4166 INDIRU8=8262

%term CVFF4=4209 CVFF8=8305
%term CVFI4=4213 CVFI8=8309

%term CVIF4=4225 CVIF8=8321
%term CVII1=1157 CVII2=2181 CVII4=4229 CVII8=8325
%term CVIU1=1158 CVIU2=2182 CVIU4=4230 CVIU8=8326

%term CVPU8=8342

%term CVUI1=1205 CVUI2=2229 CVUI4=4277 CVUI8=8373
%term CVUP8=8375
%term CVUU1=1206 CVUU2=2230 CVUU4=4278 CVUU8=8374

%term NEGF4=4289 NEGF8=8385
%term NEGI4=4293 NEGI8=8389

%term CALLB=217
%term CALLF4=4305 CALLF8=8401
%term CALLI4=4309 CALLI8=8405
%term CALLP8=8407
%term CALLU4=4310 CALLU8=8406
%term CALLV=216

%term RETF4=4337 RETF8=8433
%term RETI4=4341 RETI8=8437
%term RETP8=8439
%term RETU4=4342 RETU8=8438
%term RETV=248

%term ADDRGP8=8455
%term ADDRFP8=8471
%term ADDRLP8=8487

%term ADDF4=4401 ADDF8=8497
%term ADDI4=4405 ADDI8=8501
%term ADDP8=8503
%term ADDU4=4406 ADDU8=8502

%term SUBF4=4417 SUBF8=8513
%term SUBI4=4421 SUBI8=8517
%term SUBP8=8519
%term SUBU4=4422 SUBU8=8518

%term LSHI4=4437 LSHI8=8533
%term LSHU4=4438 LSHU8=8534

%term MODI4=4453 MODI8=8549
%term MODU4=4454 MODU8=8550

%term RSHI4=4469 RSHI8=8565
%term RSHU4=4470 RSHU8=8566

%term BANDI4=4485 BANDI8=8581
%term BANDU4=4486 BANDU8=8582

%term BCOMI4=4501 BCOMI8=8597
%term BCOMU4=4502 BCOMU8=8598

%term BORI4=4517 BORI8=8613
%term BORU4=4518 BORU8=8614

%term BXORI4=4533 BXORI8=8629
%term BXORU4=4534 BXORU8=8630

%term DIVF4=4545 DIVF8=8641
%term DIVI4=4549 DIVI8=8645
%term DIVU4=4550 DIVU8=8646

%term MULF4=4561 MULF8=8657
%term MULI4=4565 MULI8=8661
%term MULU4=4566 MULU8=8662

%term EQF4=4577 EQF8=8673
%term EQI4=4581 EQI8=8677
%term EQU4=4582 EQU8=8678

%term GEF4=4593 GEF8=8689
%term GEI4=4597 GEI8=8693
%term GEU4=4598 GEU8=8694

%term GTF4=4609 GTF8=8705
%term GTI4=4613 GTI8=8709
%term GTU4=4614 GTU8=8710

%term LEF4=4625 LEF8=8721
%term LEI4=4629 LEI8=8725
%term LEU4=4630 LEU8=8726

%term LTF4=4641 LTF8=8737
%term LTI4=4645 LTI8=8741
%term LTU4=4646 LTU8=8742

%term NEF4=4657 NEF8=8753
%term NEI4=4661 NEI8=8757
%term NEU4=4662 NEU8=8758

%term JUMPV=584

%term LABELV=600

%term LOADB=233
%term LOADF4=4321 LOADF8=8417
%term LOADI1=1253 LOADI2=2277 LOADI4=4325 LOADI8=8421
%term LOADP8=8423
%term LOADU1=1254 LOADU2=2278 LOADU4=4326 LOADU8=8422
%%
stmt:	ARGB(INDIRB(pval))            "#"
stmt:	ARGF4(fval)                   "#"
stmt:	ARGF8(dval)                   "#"
stmt:	ARGI4(ival)                   "#"
stmt:	ARGI8(lval)                   "#"
stmt:	ARGP8(pval)                   "#"
stmt:	ARGU4(ival)                   "#"
stmt:	ARGU8(lval)                   "#"

stmt:	ASGNB(addr, INDIRB(addr))     "#\n"
stmt:	ASGNP8(addr, pval)            "#%0 = PTRCAST <@typeof_1 @typeof_2> %1\n"
stmt:	ASGNI1(addr, cval)            "\t\t%0 = %1\n"
stmt:	ASGNU1(addr, cval)            "\t\t%0 = %1\n"
stmt:	ASGNI2(addr, sval)            "\t\t%0 = %1\n"
stmt:	ASGNU2(addr, sval)            "\t\t%0 = %1\n"
stmt:	ASGNI4(addr, ival)            "\t\t%0 = %1\n"
stmt:	ASGNU4(addr, ival)            "\t\t%0 = %1\n"
stmt:	ASGNI8(addr, lval)            "\t\t%0 = %1\n"
stmt:	ASGNU8(addr, lval)            "\t\t%0 = %1\n"
stmt:	ASGNF4(addr, fval)            "\t\t%0 = %1\n"
stmt:	ASGNF8(addr, dval)            "\t\t%0 = %1\n"

stmt:	ASGNI1(pval, cval)            "#\n"
stmt:	ASGNU1(pval, cval)            "#\n"
stmt:	ASGNI2(pval, sval)            "#\n"
stmt:	ASGNU2(pval, sval)            "#\n"
stmt:	ASGNI4(pval, ival)            "#\n"
stmt:	ASGNU4(pval, ival)            "#\n"
stmt:	ASGNI8(pval, lval)            "#\n"
stmt:	ASGNU8(pval, lval)            "#\n"
stmt:	ASGNF4(pval, fval)            "#\n"
stmt:	ASGNF8(pval, dval)            "#\n"

stmt:	LTI4(ival, ival)              "\t\t%%%b = SLT <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LTU4(ival, ival)              "\t\t%%%b = ULT <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LTI8(lval, lval)              "\t\t%%%b = SLT <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LTU8(lval, lval)              "\t\t%%%b = ULT <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LTF4(fval, fval)              "\t\t%%%b = FOLT <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LTF8(dval, dval)              "\t\t%%%b = FOLT <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:	LEI4(ival, ival)              "\t\t%%%b = SLE <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LEU4(ival, ival)              "\t\t%%%b = ULE <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LEI8(lval, lval)              "\t\t%%%b = SLE <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LEU8(lval, lval)              "\t\t%%%b = ULE <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LEF4(fval, fval)              "\t\t%%%b = FOLE <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	LEF8(dval, dval)              "\t\t%%%b = FOLE <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:	EQI4(ival, ival)              "\t\t%%%b = EQ <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	EQU4(ival, ival)              "\t\t%%%b = EQ <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	EQI8(lval, lval)              "\t\t%%%b = EQ <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	EQU8(lval, lval)              "\t\t%%%b = EQ <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	EQF4(fval, fval)              "\t\t%%%b = FOEQ <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	EQF8(dval, dval)              "\t\t%%%b = FOEQ <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:	GEI4(ival, ival)              "\t\t%%%b = SGE <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GEU4(ival, ival)              "\t\t%%%b = UGE <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GEI8(lval, lval)              "\t\t%%%b = SGE <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GEU8(lval, lval)              "\t\t%%%b = UGE <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GEF4(fval, fval)              "\t\t%%%b = FOGE <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GEF8(dval, dval)              "\t\t%%%b = FOGE <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:	GTI4(ival, ival)              "\t\t%%%b = SGT <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GTU4(ival, ival)              "\t\t%%%b = UGT <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GTI8(lval, lval)              "\t\t%%%b = SGT <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GTU8(lval, lval)              "\t\t%%%b = UGT <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GTF4(fval, fval)              "\t\t%%%b = FOGT <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	GTF8(dval, dval)              "\t\t%%%b = FOGT <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:	NEI4(ival, ival)              "\t\t%%%b = NE <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	NEU4(ival, ival)              "\t\t%%%b = NE <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	NEI8(lval, lval)              "\t\t%%%b = NE <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	NEU8(lval, lval)              "\t\t%%%b = NE <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	NEF4(fval, fval)              "\t\t%%%b = FONE <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:	NEF8(dval, dval)              "\t\t%%%b = FONE <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:	LABELV                        "\t%%%a:\n"
stmt:	JUMPV(ADDRGP8)                "#\t\tBRANCH %%%0\n"
stmt:	JUMPV(val)                    "#\n"
stmt:	CALLV(addr)                   "#CALL <@sig> @func_ref (%args)\n"
stmt:	RETI4(ival)                   "\t\tRET %0\n"
stmt:	RETU4(ival)                   "\t\tRET %0\n"
stmt:	RETI8(lval)                   "\t\tRET %0\n"
stmt:	RETU8(lval)                   "\t\tRET %0\n"
stmt:	RETP8(pval)                   "\t\tRET %0\n"
stmt:	RETF4(fval)                   "\t\tRET %0\n"
stmt:	RETF8(dval)                   "\t\tRET %0\n"
stmt:	RETV                          "\t\tRET ()\n"

addr:	ADDRGP8                       "@%a"
addr:	ADDRFP8                       "%%%a"
addr:	ADDRLP8                       "%%%a"

val:	cval                          "%0"
val:	sval                          "%0"
val:	ival                          "%0"
val:	lval                          "%0"
val:	pval                          "%0"
val:	fval                          "%0"
val:	dval                          "%0"

cval:	cvar                          "%0"
cval:	LOADI1(cval)                  "#"
cval:	LOADU1(cval)                  "#"
cval:	CNSTI1                        "@char_%a"
cval:	CNSTU1                        "@uchar_%a"
cval:	INDIRI1(addr)                 "%0"
cval:	INDIRU1(addr)                 "%0"

cvar:	CVII1(sval)                   "(%%%c = TRUNC <@short @char> %0)\n"
cvar:	CVII1(ival)                   "(%%%c = TRUNC <@int @char> %0)\n"
cvar:	CVII1(lval)                   "(%%%c = TRUNC <@long @char> %0)\n"
cvar:	CVIU1(cval)                   "(%%%c = %0 //char->uchar)\n"
cvar:	CVIU1(sval)                   "(%%%c = TRUNC <@short @uchar> %0)\n"
cvar:	CVIU1(ival)                   "(%%%c = TRUNC <@int @uchar> %0)\n"
cvar:	CVIU1(lval)                   "(%%%c = TRUNC <@long @uchar> %0)\n"
cvar:	CVUI1(cval)                   "(%%%c = %0 //uchar->char)\n"
cvar:	CVUI1(sval)                   "(%%%c = TRUNC <@ushort @char> %0)\n"
cvar:	CVUI1(ival)                   "(%%%c = TRUNC <@uint @char> %0)\n"
cvar:	CVUI1(lval)                   "(%%%c = TRUNC <@ulong @char> %0)\n"
cvar:	CVUU1(sval)                   "(%%%c = TRUNC <@ushort @uchar> %0)\n"
cvar:	CVUU1(ival)                   "(%%%c = TRUNC <@uint @uchar> %0)\n"
cvar:	CVUU1(lval)                   "(%%%c = TRUNC <@ulong @uchar> %0)\n"

sval:	svar                          "%0"
sval:	LOADI2(val)                   "#"
sval:	LOADU2(val)                   "#"
sval:	CNSTI2                        "@short_%a"
sval:	CNSTU2                        "@ushort_%a"
sval:	INDIRI2(addr)                 "%0"
sval:	INDIRU2(addr)                 "%0"

svar:	CVII2(cval)                   "(%%%c = SEXT <@char @short> %0)\n"
svar:	CVII2(ival)                   "(%%%c = TRUNC <@int @short> %0)\n"
svar:	CVII2(lval)                   "(%%%c = TRUNC <@long @short> %0)\n"
svar:	CVIU2(cval)                   "(%%%c = SEXT <@char @ushort> %0)\n"
svar:	CVIU2(sval)                   "(%%%c = %0 //short->ushort)\n"
svar:	CVIU2(ival)                   "(%%%c = TRUNC <@int @ushort> %0)\n"
svar:	CVIU2(lval)                   "(%%%c = TRUNC <@long @ushort> %0)\n"
svar:	CVUI2(cval)                   "(%%%c = ZEXT <@uchar @short> %0)\n"
svar:	CVUI2(ival)                   "(%%%c = TRUNC <@uint @short> %0)\n"
svar:	CVUI2(lval)                   "(%%%c = TRUNC <@ulong @char> %0)\n"
svar:	CVUU2(cval)                   "(%%%c = ZEXT <@uchar @ushort> %0)\n"
svar:	CVUI2(sval)                   "(%%%c = %0 //ushort->short)\n"
svar:	CVUU2(ival)                   "(%%%c = TRUNC <@uint @ushort> %0)\n"
svar:	CVUU2(lval)                   "(%%%c = TRUNC <@ulong @ushort> %0)\n"

ival:	ivar                          "%0"
ival:	LOADI4(val)                   "#"
ival:	LOADU4(val)                   "#"
ival:	CNSTI4                        "@int_%a"
ival:	CNSTU4                        "@uint_%a"
ival:	INDIRI4(addr)                 "%0"
ival:	INDIRU4(addr)                 "%0"

ivar:	NEGI4(ival)                   "(%%%c = XOR <@int> %0 @int_2147483648)\n"
ivar:	BCOMI4(ival)                  "(%%%c = XOR <@int> %0 @int_4294967295)\n"
ivar:	BCOMU4(ival)                  "(%%%c = XOR <@uint> %0 @int_4294967295)\n"
ivar:	BANDI4(ival, val)             "(%%%c = AND <@int> %0 %1)\n"
ivar:	BANDU4(ival, val)             "(%%%c = AND <@uint> %0 %1)\n"
ivar:	BORI4(ival, val)              "(%%%c = OR <@int> %0 %1)\n"
ivar:	BORU4(ival, val)              "(%%%c = OR <@uint> %0 %1)\n"
ivar:	BXORI4(ival, val)             "(%%%c = XOR <@int> %0 %1)\n"
ivar:	BXORU4(ival, val)             "(%%%c = XOR <@uint> %0 %1)\n"
ivar:	LSHI4(ival, val)              "(%%%c = SHL <@int> %0 %1)\n"
ivar:	LSHU4(ival, val)              "(%%%c = SHL <@uint> %0 %1)\n"
ivar:	RSHI4(ival, val)              "(%%%c = ASHR <@int> %0 %1)\n"
ivar:	RSHU4(ival, val)              "(%%%c = LSHR <@uint> %0 %1)\n"

ivar:	ADDI4(ival, ival)             "(%%%c = ADD <@int> %0 %1)\n"
ivar:	ADDU4(ival, ival)             "(%%%c = ADD <@uint> %0 %1)\n"
ivar:	SUBI4(ival, ival)             "(%%%c = SUB <@int> %0 %1)\n"
ivar:	SUBU4(ival, ival)             "(%%%c = SUB <@uint> %0 %1)\n"
ivar:	MULI4(ival, ival)             "(%%%c = MUL <@int> %0 %1)\n"
ivar:	MULU4(ival, ival)             "(%%%c = MUL <@uint> %0 %1)\n"
ivar:	DIVI4(ival, ival)             "(%%%c = SDIV <@int> %0 %1)\n"
ivar:	DIVU4(ival, ival)             "(%%%c = UDIV <@uint> %0 %1)\n"
ivar:	MODI4(ival, ival)             "(%%%c = SREM <@int> %0 %1)\n"
ivar:	MODU4(ival, ival)             "(%%%c = UREM <@int> %0 %1)\n"

ivar:	CALLI4(addr)                  "#CALL <@sig> @func_ref (%args)"
ivar:	CALLU4(addr)                  "#CALL <@sig> @func_ref (%args)"

ivar:	CVII4(cval)                   "(%%%c = SEXT <@char @int> %0)\n"
ivar:	CVII4(sval)                   "(%%%c = SEXT <@short @int> %0)\n"
ivar:	CVII4(lval)                   "(%%%c = TRUNC <@long @int> %0)\n"
ivar:	CVIU4(cval)                   "(%%%c = SEXT <@char @uint> %0)\n"
ivar:	CVIU4(sval)                   "(%%%c = SEXT <@short @uint> %0)\n"
ivar:	CVIU4(ival)                   "(%%%c = %0 //int->uint)\n"
ivar:	CVIU4(lval)                   "(%%%c = TRUNC <@long @uint> %0)\n"
ivar:	CVUI4(cval)                   "(%%%c = ZEXT <@uchar @int> %0)\n"
ivar:	CVUI4(sval)                   "(%%%c = ZEXT <@ushort @int> %0)\n"
ivar:	CVUI4(ival)                   "(%%%c = %0 //uint->int)\n"
ivar:	CVUI4(lval)                   "(%%%c = TRUNC <@ulong @int> %0)\n"
ivar:	CVUU4(cval)                   "(%%%c = ZEXT <@uchar @uint> %0)\n"
ivar:	CVUU4(sval)                   "(%%%c = ZEXT <@ushort @uint> %0)\n"
ivar:	CVUU4(lval)                   "(%%%c = TRUNC <@ulong @uint> %0)\n"
ivar:	CVFI4(fval)                   "(%%%c = FPTOSI <@float @int> %0)\n"
ivar:	CVFI4(dval)                   "(%%%c = FPTOSI <@double @int> %0)\n"

lval:	lvar                          "%0"
lval:	LOADI8(val)                   "#"
lval:	LOADU8(val)                   "#"
lval:	CNSTI8                        "@long_%a"
lval:	CNSTU8                        "@ulong_%a"
lval:	INDIRI8(addr)                 "%0"
lval:	INDIRU8(addr)                 "%0"

lvar:	NEGI8(lval)                   "(%%%c = XOR <@long> %0 @long_9223372036854775808)\n"
lvar:	BCOMI8(lval)                  "(%%%c = XOR <@long> %0 @long_18446744073709551615)\n"
lvar:	BCOMU8(lval)                  "(%%%c = XOR <@ulong> %0 @long_18446744073709551615)\n"
lvar:	BANDI8(lval, val)             "(%%%c = AND <@long> %0 %1)\n"
lvar:	BANDU8(lval, val)             "(%%%c = AND <@ulong> %0 %1)\n"
lvar:	BORI8(lval, val)              "(%%%c = OR <@long> %0 %1)\n"
lvar:	BORU8(lval, val)              "(%%%c = OR <@ulong> %0 %1)\n"
lvar:	BXORI8(lval, val)             "(%%%c = XOR <@long> %0 %1)\n"
lvar:	BXORU8(lval, val)             "(%%%c = XOR <@ulong> %0 %1)\n"
lvar:	LSHI8(lval, val)              "(%%%c = SHL <@long> %0 %1)\n"
lvar:	LSHU8(lval, val)              "(%%%c = SHL <@ulong> %0 %1)\n"
lvar:	RSHI8(lval, val)              "(%%%c = ASHR <@long> %0 %1)\n"
lvar:	RSHU8(lval, val)              "(%%%c = LSHR <@ulong> %0 %1)\n"

lvar:	ADDI8(lval, lval)             "(%%%c = ADD <@long> %0 %1)\n"
lvar:	ADDU8(lval, lval)             "(%%%c = ADD <@ulong> %0 %1)\n"
lvar:	SUBI8(lval, lval)             "(%%%c = SUB <@long> %0 %1)\n"
lvar:	SUBU8(lval, lval)             "(%%%c = SUB <@ulong> %0 %1)\n"
lvar:	MULI8(lval, lval)             "(%%%c = MUL <@long> %0 %1)\n"
lvar:	MULU8(lval, lval)             "(%%%c = MUL <@ulong> %0 %1)\n"
lvar:	DIVI8(lval, lval)             "(%%%c = SDIV <@long> %0 %1)\n"
lvar:	DIVU8(lval, lval)             "(%%%c = UDIV <@ulong> %0 %1)\n"
lvar:	MODI8(lval, lval)             "(%%%c = SREM <@long> %0 %1)\n"
lvar:	MODU8(lval, lval)             "(%%%c = UREM <@long> %0 %1)\n"

lvar:	CALLI8(addr)                  "#CALL <@sig> @func_ref (%args)"
lvar:	CALLU8(addr)                  "#CALL <@sig> @func_ref (%args)"

lvar:	CVII8(cval)                   "(%%%c = SEXT <@char @long> %0)\n"
lvar:	CVII8(sval)                   "(%%%c = SEXT <@short @long> %0)\n"
lvar:	CVII8(ival)                   "(%%%c = SEXT <@int @long> %0)\n"
lvar:	CVIU8(cval)                   "(%%%c = SEXT <@char @ulong> %0)\n"
lvar:	CVIU8(sval)                   "(%%%c = SEXT <@short @ulong> %0)\n"
lvar:	CVIU8(ival)                   "(%%%c = SEXT <@int @ulong> %0)\n"
lvar:	CVIU8(lval)                   "(%%%c = %0 //long->ulong)\n"
lvar:	CVUI8(cval)                   "(%%%c = ZEXT <@uchar @long> %0)\n"
lvar:	CVUI8(sval)                   "(%%%c = ZEXT <@ushort @long> %0)\n"
lvar:	CVUI8(ival)                   "(%%%c = ZEXT <@uint @long> %0)\n"
lvar:	CVUI8(lval)                   "(%%%c = %0 //ulong->long)\n"
lvar:	CVUU8(cval)                   "(%%%c = ZEXT <@uchar @ulong> %0)\n"
lvar:	CVUU8(sval)                   "(%%%c = ZEXT <@ushort @ulong> %0)\n"
lvar:	CVUU8(ival)                   "(%%%c = ZEXT <@uint @ulong> %0)\n"
lvar:	CVFI8(fval)                   "(%%%c = FPTOSI <@float @long> %0)\n"
lvar:	CVFI8(dval)                   "(%%%c = FPTOSI <@double @long> %0)\n"
lvar:	CVPU8(pval)                   "(%%%c = PTRCAST <@ptr_void @ulong> %0)\n"

pval:	pvar                          "%0"
pval:	LOADP8(val)                   "#"
pval:	CNSTP8                        "%0"
pval:	INDIRP8(addr)                 "%0"

pvar:	CALLP8(addr)                  "#CALL <@sig> @func_ref (%args)"
pvar:	CVUP8(cval)                   "#		%%%c = PTRCAST <@uchar @ptr_type> %0\n"
pvar:	CVUP8(sval)                   "#		%%%c = PTRCAST <@ushort @ptr_type> %0\n"
pvar:	CVUP8(ival)                   "#		%%%c = PTRCAST <@uint @ptr_type> %0\n"
pvar:	CVUP8(lval)                   "#		%%%c = PTRCAST <@ulong @ptr_type> %0\n"
pvar:	ADDP8(val, val)               "#\n"
pvar:	SUBP8(val, val)               "#\n"

fval:	fvar                          "%0"
fval:	LOADF4(val)                   "#"
fval:	CNSTF4                        "@float_%a"
fval:	INDIRF4(addr)                 "%0"

fvar:	NEGF4(fval)                   "(%%%c = FMUL <@float> %0 @float_neg1)\n"
fvar:	ADDF4(fval, fval)             "(%%%c = FADD <@float> %0 %1)\n"
fvar:	SUBF4(fval, fval)             "(%%%c = FSUB <@float> %0 %1)\n"
fvar:	MULF4(fval, fval)             "(%%%c = FMUL <@float> %0 %1)\n"
fvar:	DIVF4(fval, fval)             "(%%%c = FDIV <@float> %0 %1)\n"

fvar:	CALLF4(addr)                  "#CALL <@sig> @func_ref (%args)"

fvar:	CVIF4(cval)                   "(%%%c = SITOFP <@char @float> %0)\n"
fvar:	CVIF4(sval)                   "(%%%c = SITOFP <@short @float> %0)\n"
fvar:	CVIF4(ival)                   "(%%%c = SITOFP <@int @float> %0)\n"
fvar:	CVIF4(lval)                   "(%%%c = SITOFP <@long @float> %0)\n"
fvar:	CVFF4(dval)                   "(%%%c = FPTRUNC <@double @float> %0)\n"

dval:	dvar                          "%0"
dval:	LOADF8(val)                   "#"
dval:	CNSTF8                        "@double_%a"
dval:	INDIRF8(addr)                 "%0"

dvar:	NEGF8(dval)                   "(%%%c = FMUL <@double> %0 @double_neg1)\n"
dvar:	ADDF8(dval, dval)             "(%%%c = FADD <@double> %0 %1)\n"
dvar:	SUBF8(dval, dval)             "(%%%c = FSUB <@double> %0 %1)\n"
dvar:	MULF8(dval, dval)             "(%%%c = FMUL <@double> %0 %1)\n"
dvar:	DIVF8(dval, dval)             "(%%%c = FDIV <@double> %0 %1)\n"

dvar:	CALLF8(addr)                  "#CALL <@sig> @func_ref (%args)"

dvar:	CVIF8(cval)                   "(%%%c = SITOFP <@char @double> %0)\n"
dvar:	CVIF8(sval)                   "(%%%c = SITOFP <@short @double> %0)\n"
dvar:	CVIF8(ival)                   "(%%%c = SITOFP <@int @double> %0)\n"
dvar:	CVIF8(lval)                   "(%%%c = SITOFP <@long @double> %0)\n"
dvar:	CVFF8(fval)                   "(%%%c = FPEXT <@float @double> %0)\n"
%%
Interface muIR = {
    1, 1, 0,  /* char */
    2, 2, 0,  /* short */
    4, 4, 0,  /* int */
    8, 8, 0,  /* long */
    8, 8, 0,  /* long long */
    4, 4, 0,  /* float */
    8, 8, 0,  /* double */
    8, 8, 0,  /* long double */
    8, 8, 0,  /* T * */
    0, 1, 0,  /* struct */
    1,	/* little_endian */
    0,	/* mulops_calls */
    1,	/* wants_callb */
    1,	/* wants_argb */
    0,	/* left_to_right, allows pushing args onto args_list like a stack */
    0,	/* wants_dag */
    0,	/* unsigned_char */
    address,
    blockbeg, /* lcc */
    blockend, /* lcc */
    defaddress,
    defconst,
    defstring,
    defsymbol,
    emit,     /* lcc */
    export,
    function,
    mugen,
    global,
    import,
    local,
    progbeg,
    progend,
    segment,
    space,
    0, 0, 0, 0, 0, 0, 0,
    {
    1,	      //unsigned char max_unaligned_load;
    rmap,	   //Symbol(*rmap)(int);
    blkfetch,       //void(*blkfetch)(int size, int off, int reg, int tmp);
    blkstore,       //void(*blkstore)(int size, int off, int reg, int tmp);
    blkloop,	//void(*blkloop)(int dreg, int doff, int sreg, int soff, int size, int tmps[]);
    _label,	 //void(*_label)(Node);
    _rule,	  //int(*_rule)(void*, int);
    _nts,	   //short **_nts;
    _kids,	  //void(*_kids)(Node, int, Node*);
    _string,	//char **_string;
    _templates,     //char **_templates;
    _isinstruction, //char *_isinstruction;
    _ntname,	//char **_ntname;
    emit2,	  //void(*emit2)(Node);
    doarg,	  //void(*doarg)(Node);
    target,	 //void(*target)(Node);
    clobber	 //void(*clobber)(Node);
    }
};

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