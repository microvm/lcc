%{
#include "../src/c.h"
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
%%
stmt:        ARGB(INDIRB(ptr))             "#\t\t// structure arguments unimplemented\n"
stmt:        ARGF4(float_var)              "#		\n"
stmt:        ARGF8(double_var)             "#		\n"
stmt:        ARGI4(int_var)                "#		\n"
stmt:        ARGI8(long_var)               "#		\n"
stmt:        ARGP8(ptr)                    "#		\n"
stmt:        ARGU4(int_var)                "#		\n"
stmt:        ARGU8(long_var)               "#		\n"

stmt:        ASGNB(ptr, INDIRB(ptr))       "#		\n"
stmt:        ASGNF4(ptr, float_var)        "#		\n"
stmt:        ASGNF8(ptr, double_var)       "#		\n"
stmt:        ASGNI1(ptr, char_var)         "#		\n"
stmt:        ASGNI2(ptr, short_var)        "#		\n"
stmt:        ASGNI4(ptr, int_var)          "#		\n"
stmt:        ASGNI8(ptr, long_var)         "#		\n"
stmt:        ASGNP8(ptr, ptr)              "#		%0 = PTRCAST <@typeof_1 @typeof_2> %1\n"
stmt:        ASGNU1(ptr, char_var)         "#		\n"
stmt:        ASGNU2(ptr, short_var)        "#		\n"
stmt:        ASGNU4(ptr, int_var)          "#		\n"
stmt:        ASGNU8(ptr, long_var)         "#		\n"

stmt:        LTI4(int_var, int_var)        "\t\t%%%b = SLT <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LTU4(int_var, int_var)        "\t\t%%%b = ULT <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LTI8(long_var, long_var)      "\t\t%%%b = SLT <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LTU8(long_var, long_var)      "\t\t%%%b = ULT <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LTF4(float_var, float_var)    "\t\t%%%b = FOLT <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LTF8(double_var, double_var)  "\t\t%%%b = FOLT <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:        LEI4(int_var, int_var)        "\t\t%%%b = SLE <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LEU4(int_var, int_var)        "\t\t%%%b = ULE <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LEI8(long_var, long_var)      "\t\t%%%b = SLE <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LEU8(long_var, long_var)      "\t\t%%%b = ULE <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LEF4(float_var, float_var)    "\t\t%%%b = FOLE <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        LEF8(double_var, double_var)  "\t\t%%%b = FOLE <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:        EQI4(int_var, int_var)        "\t\t%%%b = EQ <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        EQU4(int_var, int_var)        "\t\t%%%b = EQ <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        EQI8(long_var, long_var)      "\t\t%%%b = EQ <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        EQU8(long_var, long_var)      "\t\t%%%b = EQ <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        EQF4(float_var, float_var)    "\t\t%%%b = FOEQ <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        EQF8(double_var, double_var)  "\t\t%%%b = FOEQ <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:        GEI4(int_var, int_var)        "\t\t%%%b = SGE <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GEU4(int_var, int_var)        "\t\t%%%b = UGE <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GEI8(long_var, long_var)      "\t\t%%%b = SGE <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GEU8(long_var, long_var)      "\t\t%%%b = UGE <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GEF4(float_var, float_var)    "\t\t%%%b = FOGE <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GEF8(double_var, double_var)  "\t\t%%%b = FOGE <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:        GTI4(int_var, int_var)        "\t\t%%%b = SGT <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GTU4(int_var, int_var)        "\t\t%%%b = UGT <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GTI8(long_var, long_var)      "\t\t%%%b = SGT <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GTU8(long_var, long_var)      "\t\t%%%b = UGT <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GTF4(float_var, float_var)    "\t\t%%%b = FOGT <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        GTF8(double_var, double_var)  "\t\t%%%b = FOGT <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:        NEI4(int_var, int_var)        "\t\t%%%b = NE <@int> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        NEU4(int_var, int_var)        "\t\t%%%b = NE <@uint> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        NEI8(long_var, long_var)      "\t\t%%%b = NE <@long> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        NEU8(long_var, long_var)      "\t\t%%%b = NE <@ulong> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        NEF4(float_var, float_var)    "\t\t%%%b = FONE <@float> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"
stmt:        NEF8(double_var, double_var)  "\t\t%%%b = FONE <@double> %0 %1\n\t\tBRANCH2 %%%b %%%a %%%c\n"

stmt:        CALLV(ptr)                    "#		CALL <@sig> @func_ref (%args)\n"
stmt:        JUMPV(ptr)                    "\t\tBRANCH %%%0\n"
stmt:        LABELV                        "\t%%%a:\n"
stmt:        RETF4(float_var)              "\t\tRET %0\n"
stmt:        RETF8(double_var)             "\t\tRET %0\n"
stmt:        RETI4(int_var)                "\t\tRET %0\n"
stmt:        RETI8(long_var)               "\t\tRET %0\n"
stmt:        RETP8(ptr)                    "\t\tRET %0\n"
stmt:        RETU4(int_var)                "\t\tRET %0\n"
stmt:        RETU8(long_var)               "\t\tRET %0\n"
stmt:        RETV                          "\t\tRET ()\n"

char_var:    CNSTI1                        "@char_%a"
char_var:    CNSTU1                        "@uchar_%a"
char_var:    CVII1(short_var)              "\t\t%%%c = TRUNC <@short @char> %0 //short(%a)->char\n"
char_var:    CVII1(int_var)                "\t\t%%%c = TRUNC <@int @char> %0 //int(%a)->char\n"
char_var:    CVII1(long_var)               "\t\t%%%c = TRUNC <@long @char> %0 //long(%a)->char\n"
char_var:    CVIU1(char_var)               "\t\t%%%c = %%%0 //char(%a)->uchar\n"
char_var:    CVIU1(short_var)              "\t\t%%%c = TRUNC <@short @uchar> %0 //short(%a)->uchar\n"
char_var:    CVIU1(int_var)                "\t\t%%%c = TRUNC <@int @uchar> %0 //int(%a)->uchar\n"
char_var:    CVIU1(long_var)               "\t\t%%%c = TRUNC <@long @uchar> %0 //long(%a)->uchar\n"
char_var:    CVUI1(char_var)               "\t\t%%%c = %%%0 //uchar(%a)->char\n"
char_var:    CVUI1(short_var)              "\t\t%%%c = TRUNC <@ushort @char> %0 //ushort(%a)->char\n"
char_var:    CVUI1(int_var)                "\t\t%%%c = TRUNC <@uint @char> %0 //uint(%a)->char\n"
char_var:    CVUI1(long_var)               "\t\t%%%c = TRUNC <@ulong @char> %0 //ulong(%a)->char\n"
char_var:    CVUU1(short_var)              "\t\t%%%c = TRUNC <@ushort @uchar> %0 //ushort(%a)->uchar\n"
char_var:    CVUU1(int_var)                "\t\t%%%c = TRUNC <@uint @uchar> %0 //uint(%a)->uchar\n"
char_var:    CVUU1(long_var)               "\t\t%%%c = TRUNC <@ulong @uchar> %0 //ulong(%a)->uchar\n"
char_var:    INDIRI1(arg)                  "%0"
char_var:    INDIRI1(ptr)                  "#		\n"
char_var:    INDIRU1(arg)                  "%0"
char_var:    INDIRU1(ptr)                  "#		\n"

short_var:   CNSTI2                        "@short_%a"
short_var:   CNSTU2                        "@ushort_%a"
short_var:   CVII2(char_var)               "\t\t%%%c = SEXT <@char @short> %0 //char(%a)->short\n"
short_var:   CVII2(int_var)                "\t\t%%%c = TRUNC <@int @short> %0 //int(%a)->short\n"
short_var:   CVII2(long_var)               "\t\t%%%c = TRUNC <@long @short> %0 //long(%a)->short\n"
short_var:   CVIU2(char_var)               "\t\t%%%c = SEXT <@char @ushort> %0 //char(%a)->ushort\n"
short_var:   CVIU2(short_var)              "\t\t%%%c = %%%0 //short(%a)->ushort\n"
short_var:   CVIU2(int_var)                "\t\t%%%c = TRUNC <@int @ushort> %0 //int(%a)->ushort\n"
short_var:   CVIU2(long_var)               "\t\t%%%c = TRUNC <@long @ushort> %0 //long(%a)->ushort\n"
short_var:   CVUI2(char_var)               "\t\t%%%c = ZEXT <@uchar @short> %0 //uchar(%a)->short\n"
short_var:   CVUI2(int_var)                "\t\t%%%c = TRUNC <@uint @short> %0 //uint(%a)->short\n"
short_var:   CVUI2(long_var)               "\t\t%%%c = TRUNC <@ulong @char> %0 //ulong(%a)->short\n"
short_var:   CVUU2(char_var)               "\t\t%%%c = ZEXT <@uchar @ushort> %0 //uchar(%a)->ushort\n"
short_var:   CVUI2(short_var)              "\t\t%%%c = %%%0 //ushort(%a)->short\n"
short_var:   CVUU2(int_var)                "\t\t%%%c = TRUNC <@uint @ushort> %0 //uint(%a)->ushort\n"
short_var:   CVUU2(long_var)               "\t\t%%%c = TRUNC <@ulong @ushort> %0 //ulong(%a)->ushort\n"
short_var:   INDIRI2(arg)                  "%0"
short_var:   INDIRI2(ptr)                  "#		\n"
short_var:   INDIRU2(arg)                  "%0"
short_var:   INDIRU2(ptr)                  "#		\n"

int_var:     ADDI4(int_var, int_var)       "\t\t%%%c = ADD <@int> %0 %1\n"
int_var:     ADDU4(int_var, int_var)       "#		\n"
int_var:     BANDI4(int_var, int_var)      "#		"
int_var:     BANDU4(int_var, int_var)      "#		"
int_var:     BCOMI4(int_var)               "#		"
int_var:     BCOMU4(int_var)               "#		"
int_var:     BORI4(int_var)                "#		"
int_var:     BORU4(int_var)                "#		"
int_var:     BXORI4(int_var)               "#		"
int_var:     CALLI4(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
int_var:     CALLU4(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
int_var:     CNSTI4                        "@int_%a"
int_var:     CNSTU4                        "@uint_%a"
int_var:     CVII4(char_var)               "\t\t%%%c = SEXT <@char @int> %0 //char(%a)->int\n"
int_var:     CVII4(short_var)              "\t\t%%%c = SEXT <@short @int> %0 //short(%a)->int\n"
int_var:     CVII4(long_var)               "\t\t%%%c = TRUNC <@long @int> %0 //long(%a)->int\n"
int_var:     CVIU4(char_var)               "\t\t%%%c = SEXT <@char @uint> %0 //char(%a)->uint\n"
int_var:     CVIU4(short_var)              "\t\t%%%c = SEXT <@short @uint> %0 //short(%a)->uint\n"
int_var:     CVIU4(int_var)                "\t\t%%%c = %%%0 //int(%a)->uint\n"
int_var:     CVIU4(long_var)               "\t\t%%%c = TRUNC <@long @uint> %0 //long(%a)->uint\n"
int_var:     CVUI4(char_var)               "\t\t%%%c = ZEXT <@uchar @int> %0 //uchar(%a)->int\n"
int_var:     CVUI4(short_var)              "\t\t%%%c = ZEXT <@ushort @int> %0 //ushort(%a)->int\n"
int_var:     CVUI4(int_var)                "\t\t%%%c = %%%0 //uint(%a)->int\n"
int_var:     CVUI4(long_var)               "\t\t%%%c = TRUNC <@ulong @int> %0 //ulong(%a)->int\n"
int_var:     CVUU4(char_var)               "\t\t%%%c = ZEXT <@uchar @uint> %0 //uchar(%a)->uint\n"
int_var:     CVUU4(short_var)              "\t\t%%%c = ZEXT <@ushort @uint> %0 //ushort(%a)->uint\n"
int_var:     CVUU4(long_var)               "\t\t%%%c = TRUNC <@ulong @uint> %0 //ulong(%a)->uint\n"
int_var:     CVFI4(float_var)              "#		"
int_var:     CVFI4(double_var)             "#		"
int_var:     DIVI4(int_var, int_var)       "#		"
int_var:     DIVU4(int_var, int_var)       "#		"
int_var:     INDIRI4(arg)                  "%0"
int_var:     INDIRI4(ptr)                  "#		"
int_var:     INDIRU4(arg)                  "%0"
int_var:     INDIRU4(ptr)                  "#		"
int_var:     LSHI4(int_var, int_var)       "#		"
int_var:     LSHU4(int_var, int_var)       "#		"
int_var:     MODI4(int_var, int_var)       "#		"
int_var:     MODU4(int_var, int_var)       "#		"
int_var:     MULI4(int_var, int_var)       "#		"
int_var:     MULU4(int_var, int_var)       "#		"
int_var:     NEGI4(int_var)                "#		"
int_var:     RSHI4(int_var, int_var)       "#		"
int_var:     RSHU4(int_var, int_var)       "#		"
int_var:     SUBI4(int_var, int_var)       "#		"
int_var:     SUBU4(int_var, int_var)       "#		"

long_var:    ADDI8(long_var, long_var)     "#		"
long_var:    ADDU8(long_var, long_var)     "#		"
long_var:    BANDI8(long_var, long_var)    "#		"
long_var:    BANDU8(long_var, long_var)    "#		"
long_var:    BCOMI8(long_var)              "#		"
long_var:    BCOMU8(long_var)              "#		"
long_var:    BORI8(long_var)               "#		"
long_var:    BORU8(long_var)               "#		"
long_var:    BXORI8(long_var)              "#		"
long_var:    CALLI8(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
long_var:    CALLU8(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
long_var:    CNSTI8                        "@long_%a"
long_var:    CNSTU8                        "@ulong_%a"
long_var:    CVII8(char_var)               "\t\t%%%c = SEXT <@char @long> %0 //char(%a)->long\n"
long_var:    CVII8(short_var)              "\t\t%%%c = SEXT <@short @long> %0 //short(%a)->long\n"
long_var:    CVII8(int_var)                "\t\t%%%c = SEXT <@int @long> %0 //int(%a)->long\n"
long_var:    CVIU8(char_var)               "\t\t%%%c = SEXT <@char @ulong> %0 //char(%a)->ulong\n"
long_var:    CVIU8(short_var)              "\t\t%%%c = SEXT <@short @ulong> %0 //short(%a)->ulong\n"
long_var:    CVIU8(int_var)                "\t\t%%%c = SEXT <@int @ulong> %0 //int(%a)->ulong\n"
long_var:    CVIU8(long_var)               "\t\t%%%c = %%%0 //long(%a)->ulong\n"
long_var:    CVUI8(char_var)               "\t\t%%%c = ZEXT <@uchar @long> %0 //uchar(%a)->long\n"
long_var:    CVUI8(short_var)              "\t\t%%%c = ZEXT <@ushort @long> %0 //ushort(%a)->long\n"
long_var:    CVUI8(int_var)                "\t\t%%%c = ZEXT <@uint @long> %0 //uint(%a)->long\n"
long_var:    CVUI8(long_var)               "\t\t%%%c = %%%0 //ulong(%a)->long\n"
long_var:    CVUU8(char_var)               "\t\t%%%c = ZEXT <@uchar @ulong> %0 //uchar(%a)->ulong\n"
long_var:    CVUU8(short_var)              "\t\t%%%c = ZEXT <@ushort @ulong> %0 //ushort(%a)->ulong\n"
long_var:    CVUU8(int_var)                "\t\t%%%c = ZEXT <@uint @ulong> %0 //uchar(%a)->ulong\n"
long_var:    CVFI8(float_var)              "#		"
long_var:    CVFI8(double_var)             "#		"
long_var:    CVPU8(ptr)                    "\t\t// Pointer to integer conversion not implemented"
long_var:    DIVI8(long_var, long_var)     "#		"
long_var:    DIVU8(long_var, long_var)     "#		"
long_var:    INDIRI8(arg)                  "%0"
long_var:    INDIRI8(ptr)                  "#		"
long_var:    INDIRU8(arg)                  "%0"
long_var:    INDIRU8(ptr)                  "#		"
long_var:    LSHI8(long_var, long_var)     "#		"
long_var:    LSHU8(long_var, long_var)     "#		"
long_var:    MODI8(long_var, long_var)     "#		"
long_var:    MODU8(long_var, long_var)     "#		"
long_var:    MULI8(long_var, long_var)     "#		"
long_var:    MULU8(long_var, long_var)     "#		"
long_var:    NEGI8(long_var)               "#		"
long_var:    RSHI8(long_var, long_var)     "#		"
long_var:    RSHU8(long_var, long_var)     "#		"
long_var:    SUBI8(long_var, long_var)     "#		"
long_var:    SUBU8(long_var, long_var)     "#		"

float_var:   CNSTF4                        "\t\t// Float constant %a"
float_var:   NEGF4(float_var)              "\t\t// Negate float value %0"
float_var:   ADDF4(float_var, float_var)   "#		"
float_var:   SUBF4(float_var, float_var)   "#		"
float_var:   MULF4(float_var, float_var)   "#		"
float_var:   DIVF4(float_var, float_var)   "#		"
float_var:   CVIF4(char_var)               "#		"
float_var:   CVIF4(short_var)              "#		"
float_var:   CVIF4(int_var)                "#		"
float_var:   CVIF4(long_var)               "#		"
float_var:   CVFF4(double_var)             "#		"
float_var:   CALLF4(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
float_var:   INDIRF4(arg)                  "%0"
float_var:   INDIRF4(ptr)                  "#		"

double_var:  CNSTF8                        "\t\t// Double constant %a"
double_var:  NEGF4(double_var)             "\t\t// Negate double value %0"
double_var:  ADDF8(double_var, double_var) "#		"
double_var:  SUBF8(double_var, double_var) "#		"
double_var:  MULF8(double_var, double_var) "#		"
double_var:  DIVF8(double_var, double_var) "#		"
double_var:  CVIF8(char_var)               "#		"
double_var:  CVIF8(short_var)              "#		"
double_var:  CVIF8(int_var)                "#		"
double_var:  CVIF8(long_var)               "#		"
double_var:  CVFF8(float_var)              "#		"
double_var:  CALLF8(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
double_var:  INDIRF8(arg)                  "%0"
double_var:  INDIRF8(ptr)                  "#		"

arg:         ADDRFP8                       "%%%a" 1

ptr:         ADDRFP8                       "%%%a" 2
ptr:         ADDRGP8                       "%a"
ptr:         ADDRLP8                       "%a"
ptr:         CALLP8(ptr)                   "#		%%%c = CALL <@sig> @func_ref (%args)\n"
ptr:         CNSTP8                        "#\t\t// constant pointer unimplemented\n"
ptr:         CVUP8(char_var)               "#\t\t// Integer to pointer conversion not implemented\n"
ptr:         CVUP8(short_var)              "#\t\t// Integer to pointer conversion not implemented\n"
ptr:         CVUP8(int_var)                "#\t\t// Integer to pointer conversion not implemented\n"
ptr:         CVUP8(long_var)               "#\t\t// Integer to pointer conversion not implemented\n"
ptr:         INDIRP8(arg)                  "%0"
ptr:         INDIRP8(ptr)                  "%0"
ptr:         ADDP8(ptr, long_var)          "#\n"
ptr:         SUBP8(ptr, long_var)          "#\n"
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
    1,        /* little_endian */
    0,        /* mulops_calls */
    0,        /* wants_callb */
    1,        /* wants_argb */
    0,        /* left_to_right */
    0,        /* wants_dag */
    0,        /* unsigned_char */
    address,
    blockbeg, /* frontend */
    blockend, /* frontend */
    defaddress,
    defconst,
    defstring,
    defsymbol,
    emit, /* frontend */
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
    1,              //unsigned char max_unaligned_load;
    rmap,           //Symbol(*rmap)(int);
    blkfetch,       //void(*blkfetch)(int size, int off, int reg, int tmp);
    blkstore,       //void(*blkstore)(int size, int off, int reg, int tmp);
    blkloop,        //void(*blkloop)(int dreg, int doff, int sreg, int soff, int size, int tmps[]);
    _label,         //void(*_label)(Node);
    _rule,          //int(*_rule)(void*, int);
    _nts,           //short **_nts;
    _kids,          //void(*_kids)(Node, int, Node*);
    _string,        //char **_string;
    _templates,     //char **_templates;
    _isinstruction, //char *_isinstruction;
    _ntname,        //char **_ntname;
    emit2,          //void(*emit2)(Node);
    doarg,          //void(*doarg)(Node);
    target,         //void(*target)(Node);
    clobber         //void(*clobber)(Node);
    }
};

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
 * Generates symbols and a node to bridge between LCC conditional branches (one node) and Mu branches (comparison then cond branch)
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
	Generates a variable name for each instruction's result
 */
static void mugen_var(Node p, int child) {
	if (p == NULL)
		return;
	if (p->x.inst && child) {
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
			while (!k0->x.inst && k0) k0 = k0->kids[0];
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
				if(!(p->kids[1]->x.emitted))
					emitasm(p->kids[1], nts[1]);
				print("\t\t%%%s = %%", s0->name);
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
			while (!k0->x.inst && k0) k0 = k0->kids[0];
			emitasm(k0, k0->x.inst);
			k0->x.emitted = 1;
			arg->arg = k0->syms[2]->x.name;

			while (a != NULL && a->next != NULL)
				a = a->next;
			if (a)
				a->next = arg;
			else
				args_list = arg;
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