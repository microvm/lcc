%{
#include "../src/c.h"
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
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char **);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);

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

static char* indent;

static Types types_head;
static Consts consts_head;

static Symbol intreg[32], fltreg[32];
static Symbol intregw, fltregw;

static Symbol g;

static int cseg;
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

float_var:   CNSTF4                        "// Float constant %a"
double_var:  CNSTF8                        "// Double constant %a"
char_var:    CNSTI1                        "@char_%a"
char_var:    CNSTU1                        "@uchar_%a"
short_var:   CNSTI2                        "@short_%a"
short_var:   CNSTU2                        "@ushort_%a"
int_var:     CNSTI4                        "@int_%a"
int_var:     CNSTU4                        "@uint_%a"
long_var:    CNSTI8                        "@long_%a"
long_var:    CNSTU8                        "@ulong_%a"
ptr:         CNSTP8                        "// constant pointer unimplemented\n"
stmt:        ARGB(INDIRB(ptr))             "// structure arguments unimplemented\n"
stmt:        ARGF4(float_var)              "// float arguments unimplemented\n"
stmt:        ARGF8(double_var)             "// double arguments unimplemented\n"
stmt:        ARGI4(int_var)                "// int arguments unimplemented\n"
stmt:        ARGI8(long_var)               "// long arguments unimplemented\n"
stmt:        ARGP8(ptr)                    "// pointer arguments unimplemented\n"
stmt:        ARGU4(int_var)                "// int arguments unimplemented\n"
stmt:        ARGU8(long_var)               "// long arguments unimplemented\n"
stmt:        ASGNB(ptr, INDIRB(ptr))       "// assignment to structure unimplemented\n"
stmt:        ASGNF4(ptr, float_var)        "%%%c = REFCAST <@_ptr_void @_ptr_float> %0\nSTORE <@float> %%%c %1\n"
stmt:        ASGNF8(ptr, double_var)       "%%%c = REFCAST <@_ptr_void @_ptr_double> %0\nSTORE <@double> %%%c %1\n"
stmt:        ASGNI1(ptr, char_var)         "%%%c = REFCAST <@_ptr_void @_ptr_char> %0\nSTORE <@char> %%%c %1\n"
stmt:        ASGNU1(ptr, char_var)         "%%%c = REFCAST <@_ptr_void @_ptr_char> %0\nSTORE <@char> %%%c %1\n"
stmt:        ASGNI2(ptr, short_var)        "%%%c = REFCAST <@_ptr_void @_ptr_short> %0\nSTORE <@short> %%%c %1\n"
stmt:        ASGNU2(ptr, short_var)        "%%%c = REFCAST <@_ptr_void @_ptr_short> %0\nSTORE <@short> %%%c %1\n"
stmt:        ASGNI4(ptr, int_var)          "%%%c = REFCAST <@_ptr_void @_ptr_int> %0\nSTORE <@int> %%%c %1\n"
stmt:        ASGNU4(ptr, int_var)          "%%%c = REFCAST <@_ptr_void @_ptr_int> %0\nSTORE <@int> %%%c %1\n"
stmt:        ASGNI8(ptr, long_var)         "%%%c = REFCAST <@_ptr_void @_ptr_long> %0\nSTORE <@long> %%%c %1\n"
stmt:        ASGNU8(ptr, long_var)         "%%%c = REFCAST <@_ptr_void @_ptr_long> %0\nSTORE <@long> %%%c %1\n"
stmt:        ASGNP8(ptr, ptr)              "#%0 = PTRCAST <@_ptr_void @_ptr_ptr_void> %1\n"
float_var:   INDIRF4(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_float> %0\n%%%c = LOAD <@float> %%%c_0\n"
double_var:  INDIRF8(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_double> %0\n%%%c = LOAD <@double> %%%c_0\n"
char_var:    INDIRI1(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c = LOAD <@char> %%%c_0\n"
char_var:    INDIRU1(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c = LOAD <@char> %%%c_0\n"
short_var:   INDIRI2(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_short> %0\n%%%c = LOAD <@short> %%%c_0\n"
short_var:   INDIRU2(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_short> %0\n%%%c = LOAD <@short> %%%c_0\n"
int_var:     INDIRI4(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_int> %0\n%%%c = LOAD <@int> %%%c_0\n"
int_var:     INDIRU4(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_int> %0\n%%%c = LOAD <@int> %%%c_0\n"
long_var:    INDIRI8(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_long> %0\n%%%c = LOAD <@long> %%%c_0\n"
long_var:    INDIRU8(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_long> %0\n%%%c = LOAD <@long> %%%c_0\n"
ptr:         INDIRP8(ptr)                  "%%%c_0 = REFCAST <@_ptr_void @_ptr_ptr_void> %0\n%%%c = LOAD <@ptr_void> %%%c_0\n"

float_var:   CVFF4(double_var)             "%%%c = FPTRUNC <@double @float> %0\n"
double_var:  CVFF8(float_var)              "%%%c = FPEXT <@float @double> %0\n"
int_var:     CVFI4(double_var)             "%%%c = FPTOSI <@double @int> %0\n"
int_var:     CVFI4(float_var)              "%%%c = FPTOSI <@float @int> %0\n"
long_var:    CVFI8(double_var)             "%%%c = FPTOSI <@double @long> %0\n"
long_var:    CVFI8(float_var)              "%%%c = FPTOSI <@float @long> %0\n"
float_var:   CVIF4(char_var)               "%%%c = SITOFP <@char @float> %0\n"
float_var:   CVIF4(short_var)              "%%%c = SITOFP <@short @float> %0\n"
float_var:   CVIF4(int_var)                "%%%c = SITOFP <@int @float> %0\n"
float_var:   CVIF4(long_var)               "%%%c = SITOFP <@long @float> %0\n"
double_var:  CVIF8(char_var)               "%%%c = SITOFP <@char @double> %0\n"
double_var:  CVIF8(short_var)              "%%%c = SITOFP <@short @double> %0\n"
double_var:  CVIF8(int_var)                "%%%c = SITOFP <@int @double> %0\n"
double_var:  CVIF8(long_var)               "%%%c = SITOFP <@long @double> %0\n"
char_var:    CVII1(short_var)              "%%%c = TRUNC <@short @char> %0\n"
char_var:    CVII1(int_var)                "%%%c = TRUNC <@int @char> %0\n"
char_var:    CVII1(long_var)               "%%%c = TRUNC <@long @char> %0\n"
short_var:   CVII2(char_var)               "%%%c = SEXT <@char @short> %0\n"
short_var:   CVII2(int_var)                "%%%c = TRUNC <@int @short> %0\n"
short_var:   CVII2(long_var)               "%%%c = TRUNC <@long @short> %0\n"
int_var:     CVII4(char_var)               "%%%c = SEXT <@char @int> %0\n"
int_var:     CVII4(short_var)              "%%%c = SEXT <@short @int> %0\n"
int_var:     CVII4(long_var)               "%%%c = TRUNC <@long @int> %0\n"
long_var:    CVII8(char_var)               "%%%c = SEXT <@char @long> %0\n"
long_var:    CVII8(short_var)              "%%%c = SEXT <@short @long> %0\n"
long_var:    CVII8(int_var)                "%%%c = SEXT <@int @long> %0\n"
char_var:    CVIU1(char_var)               "%%%c = %0\n"
char_var:    CVIU1(short_var)              "%%%c = TRUNC <@short @char> %0\n"
char_var:    CVIU1(int_var)                "%%%c = TRUNC <@int @char> %0\n"
char_var:    CVIU1(long_var)               "%%%c = TRUNC <@long @char> %0\n"
short_var:   CVIU2(char_var)               "%%%c = ZEXT <@char @short> %0\n"
short_var:   CVIU2(short_var)              "%%%c = %0\n"
short_var:   CVIU2(int_var)                "%%%c = TRUNC <@int @short> %0\n"
short_var:   CVIU2(long_var)               "%%%c = TRUNC <@long @short> %0\n"
int_var:     CVIU4(char_var)               "%%%c = ZEXT <@char @int> %0\n"
int_var:     CVIU4(short_var)              "%%%c = ZEXT <@short @int> %0\n"
int_var:     CVIU4(int_var)                "%%%c = %0\n"
int_var:     CVIU4(long_var)               "%%%c = TRUNC <@long @int> %0\n"
long_var:    CVIU8(char_var)               "%%%c = ZEXT <@char @long> %0\n"
long_var:    CVIU8(short_var)              "%%%c = ZEXT <@short @long> %0\n"
long_var:    CVIU8(int_var)                "%%%c = ZEXT <@int @long> %0\n"
long_var:    CVIU8(long_var)               "%%%c = %0\n"
long_var:    CVPU8(ptr)                    "// Pointer to integer conversion not supported\n"
char_var:    CVUI1(char_var)               "%%%c = %0\n"
char_var:    CVUI1(short_var)              "%%%c = TRUNC <@short @char> %0\n"
char_var:    CVUI1(int_var)                "%%%c = TRUNC <@int @char> %0\n"
char_var:    CVUI1(long_var)               "%%%c = TRUNC <@long @char> %0\n"
short_var:   CVUI2(char_var)               "%%%c = SEXT <@char @short> %0\n"
short_var:   CVUI2(short_var)              "%%%c = %0\n"
short_var:   CVUI2(int_var)                "%%%c = TRUNC <@int @short> %0\n"
short_var:   CVUI2(long_var)               "%%%c = TRUNC <@long @short> %0\n"
int_var:     CVUI4(char_var)               "%%%c = SEXT <@char @int> %0\n"
int_var:     CVUI4(short_var)              "%%%c = SEXT <@short @int> %0\n"
int_var:     CVUI4(int_var)                "%%%c = %0\n"
int_var:     CVUI4(long_var)               "%%%c = TRUNC <@long @int> %0\n"
long_var:    CVUI8(char_var)               "%%%c = SEXT <@char @long> %0\n"
long_var:    CVUI8(short_var)              "%%%c = SEXT <@short @long> %0\n"
long_var:    CVUI8(int_var)                "%%%c = SEXT <@int @long> %0\n"
long_var:    CVUI8(int_var)                "%%%c = %0\n"
ptr:         CVUP8(char_var)               "// Integer to pointer conversion not supported\n"
ptr:         CVUP8(short_var)              "// Integer to pointer conversion not supported\n"
ptr:         CVUP8(int_var)                "// Integer to pointer conversion not supported\n"
ptr:         CVUP8(long_var)               "// Integer to pointer conversion not supported\n"
char_var:    CVUU1(short_var)              "%%%c = TRUNC <@short @char> %0\n"
char_var:    CVUU1(int_var)                "%%%c = TRUNC <@int @char> %0\n"
char_var:    CVUU1(long_var)               "%%%c = TRUNC <@long @char> %0\n"
short_var:   CVUU2(char_var)               "%%%c = ZEXT <@char @short> %0\n"
short_var:   CVUU2(int_var)                "%%%c = TRUNC <@int @short> %0\n"
short_var:   CVUU2(long_var)               "%%%c = TRUNC <@long @short> %0\n"
int_var:     CVUU4(char_var)               "%%%c = ZEXT <@char @int> %0\n"
int_var:     CVUU4(short_var)              "%%%c = ZEXT <@short @int> %0\n"
int_var:     CVUU4(long_var)               "%%%c = TRUNC <@long @int> %0\n"
long_var:    CVUU8(char_var)               "%%%c = ZEXT <@char @long> %0\n"
long_var:    CVUU8(short_var)              "%%%c = ZEXT <@short @long> %0\n"
long_var:    CVUU8(int_var)                "%%%c = ZEXT <@int @long> %0\n"

float_var:   NEGF4(float_var)              "// Negate float value %0"
double_var:  NEGF4(double_var)             "// Negate double value %0"
int_var:     NEGI4(int_var)                "%%%c = SUB <@int> @0_int %0\n"
long_var:    NEGI8(long_var)               "%%%c = SUB <@long> @0_long %0\n"
float_var:   CALLF4(ptr)                   "// Call (float) unimplemented\n"
double_var:  CALLF8(ptr)                   "// Call (double) unimplemented\n"
int_var:     CALLI4(ptr)                   "// Call (int) unimplemented\n"
long_var:    CALLI8(ptr)                   "// Call (long) unimplemented\n"
ptr:         CALLP8(ptr)                   "// Call (pointer) unimplemented\n"
int_var:     CALLU4(ptr)                   "// Call (uint) unimplemented\n"
long_var:    CALLU8(ptr)                   "// Call (ulong) unimplemented\n"
stmt:        CALLV(ptr)                    "// Call (void) unimplemented\n"

stmt:        RETF4(float_var)              "RET %0\n"
stmt:        RETF8(double_var)             "RET %0\n"
stmt:        RETI4(int_var)                "RET %0\n"
stmt:        RETI8(long_var)               "RET %0\n"
stmt:        RETP8(ptr)                    "RET %0\n"
stmt:        RETU4(int_var)                "RET %0\n"
stmt:        RETU8(long_var)               "RET %0\n"
stmt:        RETV                          "RET ()\n"

ptr:         ADDRGP8                       "@%a"
ptr:         ADDRLP8                       "%%%a"
ptr:         ADDRFP8                       "%%%a"

float_var:   ADDF4(float_var, float_var)   "%%%c = FADD <@float> %0 %1\n"
double_var:  ADDF8(double_var, double_var) "%%%c = FADD <@double> %0 %1\n"
int_var:     ADDI4(int_var, int_var)       "%%%c = ADD <@int> %0 %1\n"
long_var:    ADDI8(long_var, long_var)     "%%%c = ADD <@long> %0 %1\n"
ptr:         ADDP8(ptr, long_var)          "%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c_1 = SHIFTIREF <@char @long> %%%c_0 %1\n%%%c = REFCAST <@_ptr_char @_ptr_void> %%%c_1\n"
int_var:     ADDU4(int_var, int_var)       "%%%c = ADD <@int> %0 %1\n"
long_var:    ADDU8(long_var, long_var)     "%%%c = ADD <@long> %0 %1\n"

float_var:   SUBF4(float_var, float_var)   "%%%c = FSUB <@float> %0 %1\n"
double_var:  SUBF8(double_var, double_var) "%%%c = FSUB <@double> %0 %1\n"
int_var:     SUBI4(int_var, int_var)       "%%%c = SUB <@int> %0 %1\n"
long_var:    SUBI8(long_var, long_var)     "%%%c = SUB <@long> %0 %1\n"
ptr:         SUBP8(ptr, long_var)          "%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c_1 = SUB <@long> @0_long %1\n%%%c_2 = SHIFTIREF <@char @long> %%%c_0 %%%c_1\n%%%c = REFCAST <@_ptr_char @_ptr_void> %%%c_2\n"
int_var:     SUBU4(int_var, int_var)       "%%%c = SUB <@int> %0 %1\n"
long_var:    SUBU8(long_var, long_var)     "%%%c = SUB <@long> %0 %1\n"

int_var:     LSHI4(int_var, int_var)       "%%%c = SHL <@int> %0 %1\n"
long_var:    LSHI8(long_var, long_var)     "%%%c = SHL <@long> %0 %1\n"
int_var:     LSHU4(int_var, int_var)       "%%%c = SHL <@int> %0 %1\n"
long_var:    LSHU8(long_var, long_var)     "%%%c = SHL <@long> %0 %1\n"

int_var:     MODI4(int_var, int_var)       "%%%c = SREM <@int> %0 %1\n"
long_var:    MODI8(long_var, long_var)     "%%%c = SREM <@long> %0 %1\n"
int_var:     MODU4(int_var, int_var)       "%%%c = UREM <@int> %0 %1\n"
long_var:    MODU8(long_var, long_var)     "%%%c = UREM <@int> %0 %1\n"

int_var:     RSHI4(int_var, int_var)       "%%%c = ASHR <@int> %0 %1\n"
long_var:    RSHI8(long_var, long_var)     "%%%c = ASHR <@long> %0 %1\n"
int_var:     RSHU4(int_var, int_var)       "%%%c = LSHR <@int> %0 %1\n"
long_var:    RSHU8(long_var, long_var)     "%%%c = LSHR <@long> %0 %1\n"

int_var:     BANDI4(int_var, int_var)      "%%%c = AND <@int> %0 %1\n"
long_var:    BANDI8(long_var, long_var)    "%%%c = AND <@long> %0 %1\n"
int_var:     BANDU4(int_var, int_var)      "%%%c = AND <@int> %0 %1\n"
long_var:    BANDU8(long_var, long_var)    "%%%c = AND <@long> %0 %1\n"

int_var:     BCOMI4(int_var)               "%%%c = XOR <@int> %0 @1_neg_int\n"
long_var:    BCOMI8(long_var)              "%%%c = XOR <@long> %0 @1_neg_long\n"
int_var:     BCOMU4(int_var)               "%%%c = XOR <@int> %0 @1_neg_int\n"
long_var:    BCOMU8(long_var)              "%%%c = XOR <@long> %0 @1_neg_long\n"

int_var:     BORI4(int_var)                "%%%c = OR <@int> %0 %1\n"
long_var:    BORI8(long_var)               "%%%c = OR <@long> %0 %1\n"
int_var:     BORU4(int_var)                "%%%c = OR <@int> %0 %1\n"
long_var:    BORU8(long_var)               "%%%c = OR <@long> %0 %1\n"

int_var:     BXORI4(int_var)               "%%%c = XOR <@int> %0 %1\n"
long_var:    BXORI8(long_var)              "%%%c = XOR <@int> %0 %1\n"

float_var:   DIVF4(float_var, float_var)   "%%%c = FDIV <@float> %0 %1\n"
double_var:  DIVF8(double_var, double_var) "%%%c = FDIV <@double> %0 %1\n"
int_var:     DIVI4(int_var, int_var)       "%%%c = SDIV <@int> %0 %1\n"
long_var:    DIVI8(long_var, long_var)     "%%%c = SDIV <@long> %0 %1\n"
int_var:     DIVU4(int_var, int_var)       "%%%c = UDIV <@int> %0 %1\n"
long_var:    DIVU8(long_var, long_var)     "%%%c = UDIV <@long> %0 %1\n"

float_var:   MULF4(float_var, float_var)   "%%%c = FMUL <@float> %0 %1\n"
double_var:  MULF8(double_var, double_var) "%%%c = FMUL <@double> %0 %1\n"
int_var:     MULI4(int_var, int_var)       "%%%c = MUL <@int> %0 %1\n"
long_var:    MULI8(long_var, long_var)     "%%%c = MUL <@long> %0 %1\n"
int_var:     MULU4(int_var, int_var)       "%%%c = MUL <@int> %0 %1\n"
long_var:    MULU8(long_var, long_var)     "%%%c = MUL <@long> %0 %1\n"

stmt:        EQF4(float_var, float_var)    "%%%c = FOEQ <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        EQF8(double_var, double_var)  "%%%c = FOEQ <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        EQI4(int_var, int_var)        "%%%c = EQ <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        EQI8(long_var, long_var)      "%%%c = EQ <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        EQU4(int_var, int_var)        "%%%c = EQ <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        EQU8(long_var, long_var)      "%%%c = EQ <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GEF4(float_var, float_var)    "%%%c = FOGE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GEF8(double_var, double_var)  "%%%c = FOGE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GEI4(int_var, int_var)        "#%%%c = SGE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GEI8(long_var, long_var)      "#%%%c = SGE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GEU4(int_var, int_var)        "%%%c = UGE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GEU8(long_var, long_var)      "%%%c = UGE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GTF4(float_var, float_var)    "%%%c = FOGT <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GTF8(double_var, double_var)  "%%%c = FOGT <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GTI4(int_var, int_var)        "%%%c = SGT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GTI8(long_var, long_var)      "%%%c = SGT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GTU4(int_var, int_var)        "%%%c = UGT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        GTU8(long_var, long_var)      "%%%c = UGT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LEF4(float_var, float_var)    "%%%c = FOLE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LEF8(double_var, double_var)  "%%%c = FOLE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LEI4(int_var, int_var)        "%%%c = SLE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LEI8(long_var, long_var)      "%%%c = SLE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LEU4(int_var, int_var)        "%%%c = ULE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LEU8(long_var, long_var)      "%%%c = ULE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LTF4(float_var, float_var)    "%%%c = FOLT <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LTF8(double_var, double_var)  "%%%c = FOLT <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LTI4(int_var, int_var)        "%%%c = SLT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LTI8(long_var, long_var)      "%%%c = SLT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LTU4(int_var, int_var)        "%%%c = ULT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        LTU8(long_var, long_var)      "%%%c = ULT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        NEF4(float_var, float_var)    "%%%c = FONE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        NEF8(double_var, double_var)  "%%%c = FONE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        NEI4(int_var, int_var)        "%%%c = NE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        NEI8(long_var, long_var)      "%%%c = NE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        NEU4(int_var, int_var)        "%%%c = NE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        NEU8(long_var, long_var)      "%%%c = NE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n"
stmt:        JUMPV(ptr)                    "BRANCH %0\n"
stmt:        LABELV                        "\t%%%a:\n"
%%
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

	const_name(longtype, "0");
	const_name(longtype, "1");
	const_name(floattype, "0.0f");
	const_name(floattype, "1.0f");
	const_name(doubletype, "0.0d");
	const_name(doubletype, "1.0d");

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
static void progend(void)
{
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
	print(") -> ( @%s )\n", "rettype");
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
	//TODO: add a flag so I know if I need to add a RET ()
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
	switch (specific(p->op))
	{
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
			print("%s = COMMINST @uvm.native.pin <@%s> @%s\n", tmp, type_name(t), s2->x.name);
			print("%%%s = PTRCAST <uptr<@%s> @%s> %s\n", s1->x.name, type_name(t), type_name(s1->type), tmp);
		} else {
			print("%%%s = PTRCAST <@%s @%s> %s\n", s1->x.name, type_name(s2->type), type_name(s1->type), s2->x.name);
		}
		break;
	default:
		print("//OP %d NOT RECOGNIZED\n", p->op);
		break;
	}
}
static void doarg(Node p)
{
	print("//%s called\n", __FUNCTION__);
	mkactual(4, p->syms[0]->u.c.v.i);
}
static void target(Node p) {}
static void clobber(Node p) {}

static int def_type(Type t, char *name, char *mu_t) {
	Types ts, n;
	Type ts_tmp, t_tmp;
	if (isptr(t)) {
		for (ts = types_head; ts; ts = ts->next) {
			n = ts;
			if (!isptr(ts->type)) continue;
			ts_tmp = ts->type;
			t_tmp = t;
			while (isptr(ts_tmp) && isptr(t_tmp))
			{
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
			while (isptr(ts_tmp) && isptr(t_tmp))
			{
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
    gen, /* frontend */
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