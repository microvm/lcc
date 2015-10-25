enum { EAX=0, ECX=1, EDX=2, EBX=3, ESI=6, EDI=7 };
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
static void function(Symbol, Symbol [], Symbol [], int);
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
/*
generated at Sat Oct 24 22:28:24 2015
by $Id$
*/
static void _kids(NODEPTR_TYPE, int, NODEPTR_TYPE[]);
static void _label(NODEPTR_TYPE);
static int _rule(void*, int);

#define _stmt_NT 1
#define _float_var_NT 2
#define _double_var_NT 3
#define _char_var_NT 4
#define _short_var_NT 5
#define _int_var_NT 6
#define _long_var_NT 7
#define _ptr_NT 8

static char *_ntname[] = {
	0,
	"stmt",
	"float_var",
	"double_var",
	"char_var",
	"short_var",
	"int_var",
	"long_var",
	"ptr",
	0
};

struct _state {
	short cost[9];
	struct {
		unsigned int _stmt:7;
		unsigned int _float_var:4;
		unsigned int _double_var:4;
		unsigned int _char_var:5;
		unsigned int _short_var:5;
		unsigned int _int_var:6;
		unsigned int _long_var:6;
		unsigned int _ptr:4;
	} rule;
};

static short _nts_0[] = { 0 };
static short _nts_1[] = { _ptr_NT, 0 };
static short _nts_2[] = { _float_var_NT, 0 };
static short _nts_3[] = { _double_var_NT, 0 };
static short _nts_4[] = { _int_var_NT, 0 };
static short _nts_5[] = { _long_var_NT, 0 };
static short _nts_6[] = { _ptr_NT, _ptr_NT, 0 };
static short _nts_7[] = { _ptr_NT, _float_var_NT, 0 };
static short _nts_8[] = { _ptr_NT, _double_var_NT, 0 };
static short _nts_9[] = { _ptr_NT, _char_var_NT, 0 };
static short _nts_10[] = { _ptr_NT, _short_var_NT, 0 };
static short _nts_11[] = { _ptr_NT, _int_var_NT, 0 };
static short _nts_12[] = { _ptr_NT, _long_var_NT, 0 };
static short _nts_13[] = { _char_var_NT, 0 };
static short _nts_14[] = { _short_var_NT, 0 };
static short _nts_15[] = { _float_var_NT, _float_var_NT, 0 };
static short _nts_16[] = { _double_var_NT, _double_var_NT, 0 };
static short _nts_17[] = { _int_var_NT, _int_var_NT, 0 };
static short _nts_18[] = { _long_var_NT, _long_var_NT, 0 };

static short *_nts[] = {
	0,	/* 0 */
	_nts_0,	/* 1 */
	_nts_0,	/* 2 */
	_nts_0,	/* 3 */
	_nts_0,	/* 4 */
	_nts_0,	/* 5 */
	_nts_0,	/* 6 */
	_nts_0,	/* 7 */
	_nts_0,	/* 8 */
	_nts_0,	/* 9 */
	_nts_0,	/* 10 */
	_nts_0,	/* 11 */
	_nts_1,	/* 12 */
	_nts_2,	/* 13 */
	_nts_3,	/* 14 */
	_nts_4,	/* 15 */
	_nts_5,	/* 16 */
	_nts_1,	/* 17 */
	_nts_4,	/* 18 */
	_nts_5,	/* 19 */
	_nts_6,	/* 20 */
	_nts_7,	/* 21 */
	_nts_8,	/* 22 */
	_nts_9,	/* 23 */
	_nts_9,	/* 24 */
	_nts_10,	/* 25 */
	_nts_10,	/* 26 */
	_nts_11,	/* 27 */
	_nts_11,	/* 28 */
	_nts_12,	/* 29 */
	_nts_12,	/* 30 */
	_nts_6,	/* 31 */
	_nts_1,	/* 32 */
	_nts_1,	/* 33 */
	_nts_1,	/* 34 */
	_nts_1,	/* 35 */
	_nts_1,	/* 36 */
	_nts_1,	/* 37 */
	_nts_1,	/* 38 */
	_nts_1,	/* 39 */
	_nts_1,	/* 40 */
	_nts_1,	/* 41 */
	_nts_1,	/* 42 */
	_nts_3,	/* 43 */
	_nts_2,	/* 44 */
	_nts_3,	/* 45 */
	_nts_2,	/* 46 */
	_nts_3,	/* 47 */
	_nts_2,	/* 48 */
	_nts_13,	/* 49 */
	_nts_14,	/* 50 */
	_nts_4,	/* 51 */
	_nts_5,	/* 52 */
	_nts_13,	/* 53 */
	_nts_14,	/* 54 */
	_nts_4,	/* 55 */
	_nts_5,	/* 56 */
	_nts_14,	/* 57 */
	_nts_4,	/* 58 */
	_nts_5,	/* 59 */
	_nts_13,	/* 60 */
	_nts_4,	/* 61 */
	_nts_5,	/* 62 */
	_nts_13,	/* 63 */
	_nts_14,	/* 64 */
	_nts_5,	/* 65 */
	_nts_13,	/* 66 */
	_nts_14,	/* 67 */
	_nts_4,	/* 68 */
	_nts_13,	/* 69 */
	_nts_14,	/* 70 */
	_nts_4,	/* 71 */
	_nts_5,	/* 72 */
	_nts_13,	/* 73 */
	_nts_14,	/* 74 */
	_nts_4,	/* 75 */
	_nts_5,	/* 76 */
	_nts_13,	/* 77 */
	_nts_14,	/* 78 */
	_nts_4,	/* 79 */
	_nts_5,	/* 80 */
	_nts_13,	/* 81 */
	_nts_14,	/* 82 */
	_nts_4,	/* 83 */
	_nts_5,	/* 84 */
	_nts_1,	/* 85 */
	_nts_13,	/* 86 */
	_nts_14,	/* 87 */
	_nts_4,	/* 88 */
	_nts_5,	/* 89 */
	_nts_13,	/* 90 */
	_nts_14,	/* 91 */
	_nts_4,	/* 92 */
	_nts_5,	/* 93 */
	_nts_13,	/* 94 */
	_nts_14,	/* 95 */
	_nts_4,	/* 96 */
	_nts_5,	/* 97 */
	_nts_13,	/* 98 */
	_nts_14,	/* 99 */
	_nts_4,	/* 100 */
	_nts_4,	/* 101 */
	_nts_13,	/* 102 */
	_nts_14,	/* 103 */
	_nts_4,	/* 104 */
	_nts_5,	/* 105 */
	_nts_14,	/* 106 */
	_nts_4,	/* 107 */
	_nts_5,	/* 108 */
	_nts_13,	/* 109 */
	_nts_4,	/* 110 */
	_nts_5,	/* 111 */
	_nts_13,	/* 112 */
	_nts_14,	/* 113 */
	_nts_5,	/* 114 */
	_nts_13,	/* 115 */
	_nts_14,	/* 116 */
	_nts_4,	/* 117 */
	_nts_2,	/* 118 */
	_nts_3,	/* 119 */
	_nts_4,	/* 120 */
	_nts_5,	/* 121 */
	_nts_1,	/* 122 */
	_nts_1,	/* 123 */
	_nts_1,	/* 124 */
	_nts_1,	/* 125 */
	_nts_1,	/* 126 */
	_nts_1,	/* 127 */
	_nts_1,	/* 128 */
	_nts_1,	/* 129 */
	_nts_2,	/* 130 */
	_nts_3,	/* 131 */
	_nts_4,	/* 132 */
	_nts_5,	/* 133 */
	_nts_1,	/* 134 */
	_nts_4,	/* 135 */
	_nts_5,	/* 136 */
	_nts_0,	/* 137 */
	_nts_0,	/* 138 */
	_nts_0,	/* 139 */
	_nts_0,	/* 140 */
	_nts_15,	/* 141 */
	_nts_16,	/* 142 */
	_nts_17,	/* 143 */
	_nts_18,	/* 144 */
	_nts_12,	/* 145 */
	_nts_17,	/* 146 */
	_nts_18,	/* 147 */
	_nts_15,	/* 148 */
	_nts_16,	/* 149 */
	_nts_17,	/* 150 */
	_nts_18,	/* 151 */
	_nts_12,	/* 152 */
	_nts_17,	/* 153 */
	_nts_18,	/* 154 */
	_nts_17,	/* 155 */
	_nts_18,	/* 156 */
	_nts_17,	/* 157 */
	_nts_18,	/* 158 */
	_nts_17,	/* 159 */
	_nts_18,	/* 160 */
	_nts_17,	/* 161 */
	_nts_18,	/* 162 */
	_nts_17,	/* 163 */
	_nts_18,	/* 164 */
	_nts_17,	/* 165 */
	_nts_18,	/* 166 */
	_nts_17,	/* 167 */
	_nts_18,	/* 168 */
	_nts_17,	/* 169 */
	_nts_18,	/* 170 */
	_nts_4,	/* 171 */
	_nts_5,	/* 172 */
	_nts_4,	/* 173 */
	_nts_5,	/* 174 */
	_nts_4,	/* 175 */
	_nts_5,	/* 176 */
	_nts_4,	/* 177 */
	_nts_5,	/* 178 */
	_nts_4,	/* 179 */
	_nts_5,	/* 180 */
	_nts_15,	/* 181 */
	_nts_16,	/* 182 */
	_nts_17,	/* 183 */
	_nts_18,	/* 184 */
	_nts_17,	/* 185 */
	_nts_18,	/* 186 */
	_nts_15,	/* 187 */
	_nts_16,	/* 188 */
	_nts_17,	/* 189 */
	_nts_18,	/* 190 */
	_nts_17,	/* 191 */
	_nts_18,	/* 192 */
	_nts_15,	/* 193 */
	_nts_16,	/* 194 */
	_nts_17,	/* 195 */
	_nts_18,	/* 196 */
	_nts_17,	/* 197 */
	_nts_18,	/* 198 */
	_nts_15,	/* 199 */
	_nts_16,	/* 200 */
	_nts_17,	/* 201 */
	_nts_18,	/* 202 */
	_nts_17,	/* 203 */
	_nts_18,	/* 204 */
	_nts_15,	/* 205 */
	_nts_16,	/* 206 */
	_nts_17,	/* 207 */
	_nts_18,	/* 208 */
	_nts_17,	/* 209 */
	_nts_18,	/* 210 */
	_nts_15,	/* 211 */
	_nts_16,	/* 212 */
	_nts_17,	/* 213 */
	_nts_18,	/* 214 */
	_nts_17,	/* 215 */
	_nts_18,	/* 216 */
	_nts_15,	/* 217 */
	_nts_16,	/* 218 */
	_nts_17,	/* 219 */
	_nts_18,	/* 220 */
	_nts_17,	/* 221 */
	_nts_18,	/* 222 */
	_nts_15,	/* 223 */
	_nts_16,	/* 224 */
	_nts_17,	/* 225 */
	_nts_18,	/* 226 */
	_nts_17,	/* 227 */
	_nts_18,	/* 228 */
	_nts_1,	/* 229 */
	_nts_0,	/* 230 */
};

static char *_templates[] = {
/* 0 */	0,
/* 1 */	"// Float constant %a",	/* float_var: CNSTF4 */
/* 2 */	"// Double constant %a",	/* double_var: CNSTF8 */
/* 3 */	"@%a_char",	/* char_var: CNSTI1 */
/* 4 */	"@%a_uchar",	/* char_var: CNSTU1 */
/* 5 */	"@%a_short",	/* short_var: CNSTI2 */
/* 6 */	"@%a_ushort",	/* short_var: CNSTU2 */
/* 7 */	"@%a_int",	/* int_var: CNSTI4 */
/* 8 */	"@%a_uint",	/* int_var: CNSTU4 */
/* 9 */	"@%a_long",	/* long_var: CNSTI8 */
/* 10 */	"@%a_ulong",	/* long_var: CNSTU8 */
/* 11 */	"// constant pointer unimplemented\n",	/* ptr: CNSTP8 */
/* 12 */	"// structure arguments unimplemented\n",	/* stmt: ARGB(INDIRB(ptr)) */
/* 13 */	"// float arguments unimplemented\n",	/* stmt: ARGF4(float_var) */
/* 14 */	"// double arguments unimplemented\n",	/* stmt: ARGF8(double_var) */
/* 15 */	"// int arguments unimplemented\n",	/* stmt: ARGI4(int_var) */
/* 16 */	"// long arguments unimplemented\n",	/* stmt: ARGI8(long_var) */
/* 17 */	"// pointer arguments unimplemented\n",	/* stmt: ARGP8(ptr) */
/* 18 */	"// int arguments unimplemented\n",	/* stmt: ARGU4(int_var) */
/* 19 */	"// long arguments unimplemented\n",	/* stmt: ARGU8(long_var) */
/* 20 */	"// assignment to structure unimplemented\n",	/* stmt: ASGNB(ptr,INDIRB(ptr)) */
/* 21 */	"%%%c = REFCAST <@_ptr_void @_ptr_float> %0\nSTORE <@float> %%%c %1\n",	/* stmt: ASGNF4(ptr,float_var) */
/* 22 */	"%%%c = REFCAST <@_ptr_void @_ptr_double> %0\nSTORE <@double> %%%c %1\n",	/* stmt: ASGNF8(ptr,double_var) */
/* 23 */	"%%%c = REFCAST <@_ptr_void @_ptr_char> %0\nSTORE <@char> %%%c %1\n",	/* stmt: ASGNI1(ptr,char_var) */
/* 24 */	"%%%c = REFCAST <@_ptr_void @_ptr_char> %0\nSTORE <@char> %%%c %1\n",	/* stmt: ASGNU1(ptr,char_var) */
/* 25 */	"%%%c = REFCAST <@_ptr_void @_ptr_short> %0\nSTORE <@short> %%%c %1\n",	/* stmt: ASGNI2(ptr,short_var) */
/* 26 */	"%%%c = REFCAST <@_ptr_void @_ptr_short> %0\nSTORE <@short> %%%c %1\n",	/* stmt: ASGNU2(ptr,short_var) */
/* 27 */	"%%%c = REFCAST <@_ptr_void @_ptr_int> %0\nSTORE <@int> %%%c %1\n",	/* stmt: ASGNI4(ptr,int_var) */
/* 28 */	"%%%c = REFCAST <@_ptr_void @_ptr_int> %0\nSTORE <@int> %%%c %1\n",	/* stmt: ASGNU4(ptr,int_var) */
/* 29 */	"%%%c = REFCAST <@_ptr_void @_ptr_long> %0\nSTORE <@long> %%%c %1\n",	/* stmt: ASGNI8(ptr,long_var) */
/* 30 */	"%%%c = REFCAST <@_ptr_void @_ptr_long> %0\nSTORE <@long> %%%c %1\n",	/* stmt: ASGNU8(ptr,long_var) */
/* 31 */	"%1 = PTRCAST <@_ptr_void @_ptr_ptr_void> %0\n",	/* stmt: ASGNP8(ptr,ptr) */
/* 32 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_float> %0\n%%%c = LOAD <@float> %%%c_0\n",	/* float_var: INDIRF4(ptr) */
/* 33 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_double> %0\n%%%c = LOAD <@double> %%%c_0\n",	/* double_var: INDIRF8(ptr) */
/* 34 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c = LOAD <@char> %%%c_0\n",	/* char_var: INDIRI1(ptr) */
/* 35 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c = LOAD <@char> %%%c_0\n",	/* char_var: INDIRU1(ptr) */
/* 36 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_short> %0\n%%%c = LOAD <@short> %%%c_0\n",	/* short_var: INDIRI2(ptr) */
/* 37 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_short> %0\n%%%c = LOAD <@short> %%%c_0\n",	/* short_var: INDIRU2(ptr) */
/* 38 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_int> %0\n%%%c = LOAD <@int> %%%c_0\n",	/* int_var: INDIRI4(ptr) */
/* 39 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_int> %0\n%%%c = LOAD <@int> %%%c_0\n",	/* int_var: INDIRU4(ptr) */
/* 40 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_long> %0\n%%%c = LOAD <@long> %%%c_0\n",	/* long_var: INDIRI8(ptr) */
/* 41 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_long> %0\n%%%c = LOAD <@long> %%%c_0\n",	/* long_var: INDIRU8(ptr) */
/* 42 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_ptr_void> %0\n%%%c = LOAD <@ptr_void> %%%c_0\n",	/* ptr: INDIRP8(ptr) */
/* 43 */	"%%%c = FPTRUNC <@double @float> %0\n",	/* float_var: CVFF4(double_var) */
/* 44 */	"%%%c = FPEXT <@float @double> %0\n",	/* double_var: CVFF8(float_var) */
/* 45 */	"%%%c = FPTOSI <@double @int> %0\n",	/* int_var: CVFI4(double_var) */
/* 46 */	"%%%c = FPTOSI <@float @int> %0\n",	/* int_var: CVFI4(float_var) */
/* 47 */	"%%%c = FPTOSI <@double @long> %0\n",	/* long_var: CVFI8(double_var) */
/* 48 */	"%%%c = FPTOSI <@float @long> %0\n",	/* long_var: CVFI8(float_var) */
/* 49 */	"%%%c = SITOFP <@char @float> %0\n",	/* float_var: CVIF4(char_var) */
/* 50 */	"%%%c = SITOFP <@short @float> %0\n",	/* float_var: CVIF4(short_var) */
/* 51 */	"%%%c = SITOFP <@int @float> %0\n",	/* float_var: CVIF4(int_var) */
/* 52 */	"%%%c = SITOFP <@long @float> %0\n",	/* float_var: CVIF4(long_var) */
/* 53 */	"%%%c = SITOFP <@char @double> %0\n",	/* double_var: CVIF8(char_var) */
/* 54 */	"%%%c = SITOFP <@short @double> %0\n",	/* double_var: CVIF8(short_var) */
/* 55 */	"%%%c = SITOFP <@int @double> %0\n",	/* double_var: CVIF8(int_var) */
/* 56 */	"%%%c = SITOFP <@long @double> %0\n",	/* double_var: CVIF8(long_var) */
/* 57 */	"%%%c = TRUNC <@short @char> %0\n",	/* char_var: CVII1(short_var) */
/* 58 */	"%%%c = TRUNC <@int @char> %0\n",	/* char_var: CVII1(int_var) */
/* 59 */	"%%%c = TRUNC <@long @char> %0\n",	/* char_var: CVII1(long_var) */
/* 60 */	"%%%c = SEXT <@char @short> %0\n",	/* short_var: CVII2(char_var) */
/* 61 */	"%%%c = TRUNC <@int @short> %0\n",	/* short_var: CVII2(int_var) */
/* 62 */	"%%%c = TRUNC <@long @short> %0\n",	/* short_var: CVII2(long_var) */
/* 63 */	"%%%c = SEXT <@char @int> %0\n",	/* int_var: CVII4(char_var) */
/* 64 */	"%%%c = SEXT <@short @int> %0\n",	/* int_var: CVII4(short_var) */
/* 65 */	"%%%c = TRUNC <@long @int> %0\n",	/* int_var: CVII4(long_var) */
/* 66 */	"%%%c = SEXT <@char @long> %0\n",	/* long_var: CVII8(char_var) */
/* 67 */	"%%%c = SEXT <@short @long> %0\n",	/* long_var: CVII8(short_var) */
/* 68 */	"%%%c = SEXT <@int @long> %0\n",	/* long_var: CVII8(int_var) */
/* 69 */	"%%%c = %0\n",	/* char_var: CVIU1(char_var) */
/* 70 */	"%%%c = TRUNC <@short @char> %0\n",	/* char_var: CVIU1(short_var) */
/* 71 */	"%%%c = TRUNC <@int @char> %0\n",	/* char_var: CVIU1(int_var) */
/* 72 */	"%%%c = TRUNC <@long @char> %0\n",	/* char_var: CVIU1(long_var) */
/* 73 */	"%%%c = ZEXT <@char @short> %0\n",	/* short_var: CVIU2(char_var) */
/* 74 */	"%%%c = %0\n",	/* short_var: CVIU2(short_var) */
/* 75 */	"%%%c = TRUNC <@int @short> %0\n",	/* short_var: CVIU2(int_var) */
/* 76 */	"%%%c = TRUNC <@long @short> %0\n",	/* short_var: CVIU2(long_var) */
/* 77 */	"%%%c = ZEXT <@char @int> %0\n",	/* int_var: CVIU4(char_var) */
/* 78 */	"%%%c = ZEXT <@short @int> %0\n",	/* int_var: CVIU4(short_var) */
/* 79 */	"%%%c = %0\n",	/* int_var: CVIU4(int_var) */
/* 80 */	"%%%c = TRUNC <@long @int> %0\n",	/* int_var: CVIU4(long_var) */
/* 81 */	"%%%c = ZEXT <@char @long> %0\n",	/* long_var: CVIU8(char_var) */
/* 82 */	"%%%c = ZEXT <@short @long> %0\n",	/* long_var: CVIU8(short_var) */
/* 83 */	"%%%c = ZEXT <@int @long> %0\n",	/* long_var: CVIU8(int_var) */
/* 84 */	"%%%c = %0\n",	/* long_var: CVIU8(long_var) */
/* 85 */	"// Pointer to integer conversion not supported\n",	/* long_var: CVPU8(ptr) */
/* 86 */	"%%%c = %0\n",	/* char_var: CVUI1(char_var) */
/* 87 */	"%%%c = TRUNC <@short @char> %0\n",	/* char_var: CVUI1(short_var) */
/* 88 */	"%%%c = TRUNC <@int @char> %0\n",	/* char_var: CVUI1(int_var) */
/* 89 */	"%%%c = TRUNC <@long @char> %0\n",	/* char_var: CVUI1(long_var) */
/* 90 */	"%%%c = SEXT <@char @short> %0\n",	/* short_var: CVUI2(char_var) */
/* 91 */	"%%%c = %0\n",	/* short_var: CVUI2(short_var) */
/* 92 */	"%%%c = TRUNC <@int @short> %0\n",	/* short_var: CVUI2(int_var) */
/* 93 */	"%%%c = TRUNC <@long @short> %0\n",	/* short_var: CVUI2(long_var) */
/* 94 */	"%%%c = SEXT <@char @int> %0\n",	/* int_var: CVUI4(char_var) */
/* 95 */	"%%%c = SEXT <@short @int> %0\n",	/* int_var: CVUI4(short_var) */
/* 96 */	"%%%c = %0\n",	/* int_var: CVUI4(int_var) */
/* 97 */	"%%%c = TRUNC <@long @int> %0\n",	/* int_var: CVUI4(long_var) */
/* 98 */	"%%%c = SEXT <@char @long> %0\n",	/* long_var: CVUI8(char_var) */
/* 99 */	"%%%c = SEXT <@short @long> %0\n",	/* long_var: CVUI8(short_var) */
/* 100 */	"%%%c = SEXT <@int @long> %0\n",	/* long_var: CVUI8(int_var) */
/* 101 */	"%%%c = %0\n",	/* long_var: CVUI8(int_var) */
/* 102 */	"// Integer to pointer conversion not supported\n",	/* ptr: CVUP8(char_var) */
/* 103 */	"// Integer to pointer conversion not supported\n",	/* ptr: CVUP8(short_var) */
/* 104 */	"// Integer to pointer conversion not supported\n",	/* ptr: CVUP8(int_var) */
/* 105 */	"// Integer to pointer conversion not supported\n",	/* ptr: CVUP8(long_var) */
/* 106 */	"%%%c = TRUNC <@short @char> %0\n",	/* char_var: CVUU1(short_var) */
/* 107 */	"%%%c = TRUNC <@int @char> %0\n",	/* char_var: CVUU1(int_var) */
/* 108 */	"%%%c = TRUNC <@long @char> %0\n",	/* char_var: CVUU1(long_var) */
/* 109 */	"%%%c = ZEXT <@char @short> %0\n",	/* short_var: CVUU2(char_var) */
/* 110 */	"%%%c = TRUNC <@int @short> %0\n",	/* short_var: CVUU2(int_var) */
/* 111 */	"%%%c = TRUNC <@long @short> %0\n",	/* short_var: CVUU2(long_var) */
/* 112 */	"%%%c = ZEXT <@char @int> %0\n",	/* int_var: CVUU4(char_var) */
/* 113 */	"%%%c = ZEXT <@short @int> %0\n",	/* int_var: CVUU4(short_var) */
/* 114 */	"%%%c = TRUNC <@long @int> %0\n",	/* int_var: CVUU4(long_var) */
/* 115 */	"%%%c = ZEXT <@char @long> %0\n",	/* long_var: CVUU8(char_var) */
/* 116 */	"%%%c = ZEXT <@short @long> %0\n",	/* long_var: CVUU8(short_var) */
/* 117 */	"%%%c = ZEXT <@int @long> %0\n",	/* long_var: CVUU8(int_var) */
/* 118 */	"// Negate float value %0",	/* float_var: NEGF4(float_var) */
/* 119 */	"// Negate double value %0",	/* double_var: NEGF4(double_var) */
/* 120 */	"%%%c = SUB <@int> @0_int %0\n",	/* int_var: NEGI4(int_var) */
/* 121 */	"%%%c = SUB <@long> @0_long %0\n",	/* long_var: NEGI8(long_var) */
/* 122 */	"// Call (float) unimplemented\n",	/* float_var: CALLF4(ptr) */
/* 123 */	"// Call (double) unimplemented\n",	/* double_var: CALLF8(ptr) */
/* 124 */	"// Call (int) unimplemented\n",	/* int_var: CALLI4(ptr) */
/* 125 */	"// Call (long) unimplemented\n",	/* long_var: CALLI8(ptr) */
/* 126 */	"// Call (pointer) unimplemented\n",	/* ptr: CALLP8(ptr) */
/* 127 */	"// Call (uint) unimplemented\n",	/* int_var: CALLU4(ptr) */
/* 128 */	"// Call (ulong) unimplemented\n",	/* long_var: CALLU8(ptr) */
/* 129 */	"// Call (void) unimplemented\n",	/* stmt: CALLV(ptr) */
/* 130 */	"RET <@float> %0\n",	/* stmt: RETF4(float_var) */
/* 131 */	"RET <@double> %0\n",	/* stmt: RETF8(double_var) */
/* 132 */	"RET <@int> %0\n",	/* stmt: RETI4(int_var) */
/* 133 */	"RET <@long> %0\n",	/* stmt: RETI8(long_var) */
/* 134 */	"RET <@_ptr_void> %0\n",	/* stmt: RETP8(ptr) */
/* 135 */	"RET <@int> %0\n",	/* stmt: RETU4(int_var) */
/* 136 */	"RET <@long> %0\n",	/* stmt: RETU8(long_var) */
/* 137 */	"RETVOID\n",	/* stmt: RETV */
/* 138 */	"@%%%a",	/* ptr: ADDRGP8 */
/* 139 */	"%%%a",	/* ptr: ADDRLP8 */
/* 140 */	"%%%a",	/* ptr: ADDRFP8 */
/* 141 */	"%%%c = FADD <@float> %0 %1\n",	/* float_var: ADDF4(float_var,float_var) */
/* 142 */	"%%%c = FADD <@double> %0 %1\n",	/* double_var: ADDF8(double_var,double_var) */
/* 143 */	"%%%c = ADD <@int> %0 %1\n",	/* int_var: ADDI4(int_var,int_var) */
/* 144 */	"%%%c = ADD <@long> %0 %1\n",	/* long_var: ADDI8(long_var,long_var) */
/* 145 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c_1 = SHIFTIREF <@char @long> %%%c_0 %1\n%%%c = REFCAST <@_ptr_char @_ptr_void> %%%c_1\n",	/* ptr: ADDP8(ptr,long_var) */
/* 146 */	"%%%c = ADD <@int> %0 %1\n",	/* int_var: ADDU4(int_var,int_var) */
/* 147 */	"%%%c = ADD <@long> %0 %1\n",	/* long_var: ADDU8(long_var,long_var) */
/* 148 */	"%%%c = FSUB <@float> %0 %1\n",	/* float_var: SUBF4(float_var,float_var) */
/* 149 */	"%%%c = FSUB <@double> %0 %1\n",	/* double_var: SUBF8(double_var,double_var) */
/* 150 */	"%%%c = SUB <@int> %0 %1\n",	/* int_var: SUBI4(int_var,int_var) */
/* 151 */	"%%%c = SUB <@long> %0 %1\n",	/* long_var: SUBI8(long_var,long_var) */
/* 152 */	"%%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c_1 = SUB <@long> @0_long %1\n%%%c_2 = SHIFTIREF <@char @long> %%%c_0 %%%c_1\n%%%c = REFCAST <@_ptr_char @_ptr_void> %%%c_2\n",	/* ptr: SUBP8(ptr,long_var) */
/* 153 */	"%%%c = SUB <@int> %0 %1\n",	/* int_var: SUBU4(int_var,int_var) */
/* 154 */	"%%%c = SUB <@long> %0 %1\n",	/* long_var: SUBU8(long_var,long_var) */
/* 155 */	"%%%c = SHL <@int> %0 %1\n",	/* int_var: LSHI4(int_var,int_var) */
/* 156 */	"%%%c = SHL <@long> %0 %1\n",	/* long_var: LSHI8(long_var,long_var) */
/* 157 */	"%%%c = SHL <@int> %0 %1\n",	/* int_var: LSHU4(int_var,int_var) */
/* 158 */	"%%%c = SHL <@long> %0 %1\n",	/* long_var: LSHU8(long_var,long_var) */
/* 159 */	"%%%c = SREM <@int> %0 %1\n",	/* int_var: MODI4(int_var,int_var) */
/* 160 */	"%%%c = SREM <@long> %0 %1\n",	/* long_var: MODI8(long_var,long_var) */
/* 161 */	"%%%c = UREM <@int> %0 %1\n",	/* int_var: MODU4(int_var,int_var) */
/* 162 */	"%%%c = UREM <@int> %0 %1\n",	/* long_var: MODU8(long_var,long_var) */
/* 163 */	"%%%c = ASHR <@int> %0 %1\n",	/* int_var: RSHI4(int_var,int_var) */
/* 164 */	"%%%c = ASHR <@long> %0 %1\n",	/* long_var: RSHI8(long_var,long_var) */
/* 165 */	"%%%c = LSHR <@int> %0 %1\n",	/* int_var: RSHU4(int_var,int_var) */
/* 166 */	"%%%c = LSHR <@long> %0 %1\n",	/* long_var: RSHU8(long_var,long_var) */
/* 167 */	"%%%c = AND <@int> %0 %1\n",	/* int_var: BANDI4(int_var,int_var) */
/* 168 */	"%%%c = AND <@long> %0 %1\n",	/* long_var: BANDI8(long_var,long_var) */
/* 169 */	"%%%c = AND <@int> %0 %1\n",	/* int_var: BANDU4(int_var,int_var) */
/* 170 */	"%%%c = AND <@long> %0 %1\n",	/* long_var: BANDU8(long_var,long_var) */
/* 171 */	"%%%c = XOR <@int> %0 @1_neg_int\n",	/* int_var: BCOMI4(int_var) */
/* 172 */	"%%%c = XOR <@long> %0 @1_neg_long\n",	/* long_var: BCOMI8(long_var) */
/* 173 */	"%%%c = XOR <@int> %0 @1_neg_int\n",	/* int_var: BCOMU4(int_var) */
/* 174 */	"%%%c = XOR <@long> %0 @1_neg_long\n",	/* long_var: BCOMU8(long_var) */
/* 175 */	"%%%c = OR <@int> %0 %1\n",	/* int_var: BORI4(int_var) */
/* 176 */	"%%%c = OR <@long> %0 %1\n",	/* long_var: BORI8(long_var) */
/* 177 */	"%%%c = OR <@int> %0 %1\n",	/* int_var: BORU4(int_var) */
/* 178 */	"%%%c = OR <@long> %0 %1\n",	/* long_var: BORU8(long_var) */
/* 179 */	"%%%c = XOR <@int> %0 %1\n",	/* int_var: BXORI4(int_var) */
/* 180 */	"%%%c = XOR <@int> %0 %1\n",	/* long_var: BXORI8(long_var) */
/* 181 */	"%%%c = FDIV <@float> %0 %1\n",	/* float_var: DIVF4(float_var,float_var) */
/* 182 */	"%%%c = FDIV <@double> %0 %1\n",	/* double_var: DIVF8(double_var,double_var) */
/* 183 */	"%%%c = SDIV <@int> %0 %1\n",	/* int_var: DIVI4(int_var,int_var) */
/* 184 */	"%%%c = SDIV <@long> %0 %1\n",	/* long_var: DIVI8(long_var,long_var) */
/* 185 */	"%%%c = UDIV <@int> %0 %1\n",	/* int_var: DIVU4(int_var,int_var) */
/* 186 */	"%%%c = UDIV <@long> %0 %1\n",	/* long_var: DIVU8(long_var,long_var) */
/* 187 */	"%%%c = FMUL <@float> %0 %1\n",	/* float_var: MULF4(float_var,float_var) */
/* 188 */	"%%%c = FMUL <@double> %0 %1\n",	/* double_var: MULF8(double_var,double_var) */
/* 189 */	"%%%c = MUL <@int> %0 %1\n",	/* int_var: MULI4(int_var,int_var) */
/* 190 */	"%%%c = MUL <@long> %0 %1\n",	/* long_var: MULI8(long_var,long_var) */
/* 191 */	"%%%c = MUL <@int> %0 %1\n",	/* int_var: MULU4(int_var,int_var) */
/* 192 */	"%%%c = MUL <@long> %0 %1\n",	/* long_var: MULU8(long_var,long_var) */
/* 193 */	"%%%c = FOEQ <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: EQF4(float_var,float_var) */
/* 194 */	"%%%c = FOEQ <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: EQF8(double_var,double_var) */
/* 195 */	"%%%c = EQ <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: EQI4(int_var,int_var) */
/* 196 */	"%%%c = EQ <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: EQI8(long_var,long_var) */
/* 197 */	"%%%c = EQ <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: EQU4(int_var,int_var) */
/* 198 */	"%%%c = EQ <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: EQU8(long_var,long_var) */
/* 199 */	"%%%c = FOGE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GEF4(float_var,float_var) */
/* 200 */	"%%%c = FOGE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GEF8(double_var,double_var) */
/* 201 */	"#%%%c = SGE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GEI4(int_var,int_var) */
/* 202 */	"#%%%c = SGE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GEI8(long_var,long_var) */
/* 203 */	"%%%c = UGE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GEU4(int_var,int_var) */
/* 204 */	"%%%c = UGE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GEU8(long_var,long_var) */
/* 205 */	"%%%c = FOGT <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GTF4(float_var,float_var) */
/* 206 */	"%%%c = FOGT <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GTF8(double_var,double_var) */
/* 207 */	"%%%c = SGT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GTI4(int_var,int_var) */
/* 208 */	"%%%c = SGT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GTI8(long_var,long_var) */
/* 209 */	"%%%c = UGT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GTU4(int_var,int_var) */
/* 210 */	"%%%c = UGT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: GTU8(long_var,long_var) */
/* 211 */	"%%%c = FOLE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LEF4(float_var,float_var) */
/* 212 */	"%%%c = FOLE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LEF8(double_var,double_var) */
/* 213 */	"%%%c = SLE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LEI4(int_var,int_var) */
/* 214 */	"%%%c = SLE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LEI8(long_var,long_var) */
/* 215 */	"%%%c = ULE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LEU4(int_var,int_var) */
/* 216 */	"%%%c = ULE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LEU8(long_var,long_var) */
/* 217 */	"%%%c = FOLT <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LTF4(float_var,float_var) */
/* 218 */	"%%%c = FOLT <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LTF8(double_var,double_var) */
/* 219 */	"%%%c = SLT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LTI4(int_var,int_var) */
/* 220 */	"%%%c = SLT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LTI8(long_var,long_var) */
/* 221 */	"%%%c = ULT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LTU4(int_var,int_var) */
/* 222 */	"%%%c = ULT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: LTU8(long_var,long_var) */
/* 223 */	"%%%c = FONE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: NEF4(float_var,float_var) */
/* 224 */	"%%%c = FONE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: NEF8(double_var,double_var) */
/* 225 */	"%%%c = NE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: NEI4(int_var,int_var) */
/* 226 */	"%%%c = NE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: NEI8(long_var,long_var) */
/* 227 */	"%%%c = NE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: NEU4(int_var,int_var) */
/* 228 */	"%%%c = NE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n",	/* stmt: NEU8(long_var,long_var) */
/* 229 */	"BRANCH %0\n",	/* stmt: JUMPV(ptr) */
/* 230 */	"%%%a:\n",	/* stmt: LABELV */
};

static char _isinstruction[] = {
/* 0 */	0,
/* 1 */	0,	/* // Float constant %a */
/* 2 */	0,	/* // Double constant %a */
/* 3 */	0,	/* @%a_char */
/* 4 */	0,	/* @%a_uchar */
/* 5 */	0,	/* @%a_short */
/* 6 */	0,	/* @%a_ushort */
/* 7 */	0,	/* @%a_int */
/* 8 */	0,	/* @%a_uint */
/* 9 */	0,	/* @%a_long */
/* 10 */	0,	/* @%a_ulong */
/* 11 */	1,	/* // constant pointer unimplemented\n */
/* 12 */	1,	/* // structure arguments unimplemented\n */
/* 13 */	1,	/* // float arguments unimplemented\n */
/* 14 */	1,	/* // double arguments unimplemented\n */
/* 15 */	1,	/* // int arguments unimplemented\n */
/* 16 */	1,	/* // long arguments unimplemented\n */
/* 17 */	1,	/* // pointer arguments unimplemented\n */
/* 18 */	1,	/* // int arguments unimplemented\n */
/* 19 */	1,	/* // long arguments unimplemented\n */
/* 20 */	1,	/* // assignment to structure unimplemented\n */
/* 21 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_float> %0\nSTORE <@float> %%%c %1\n */
/* 22 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_double> %0\nSTORE <@double> %%%c %1\n */
/* 23 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_char> %0\nSTORE <@char> %%%c %1\n */
/* 24 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_char> %0\nSTORE <@char> %%%c %1\n */
/* 25 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_short> %0\nSTORE <@short> %%%c %1\n */
/* 26 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_short> %0\nSTORE <@short> %%%c %1\n */
/* 27 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_int> %0\nSTORE <@int> %%%c %1\n */
/* 28 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_int> %0\nSTORE <@int> %%%c %1\n */
/* 29 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_long> %0\nSTORE <@long> %%%c %1\n */
/* 30 */	1,	/* %%%c = REFCAST <@_ptr_void @_ptr_long> %0\nSTORE <@long> %%%c %1\n */
/* 31 */	1,	/* %1 = PTRCAST <@_ptr_void @_ptr_ptr_void> %0\n */
/* 32 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_float> %0\n%%%c = LOAD <@float> %%%c_0\n */
/* 33 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_double> %0\n%%%c = LOAD <@double> %%%c_0\n */
/* 34 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c = LOAD <@char> %%%c_0\n */
/* 35 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c = LOAD <@char> %%%c_0\n */
/* 36 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_short> %0\n%%%c = LOAD <@short> %%%c_0\n */
/* 37 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_short> %0\n%%%c = LOAD <@short> %%%c_0\n */
/* 38 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_int> %0\n%%%c = LOAD <@int> %%%c_0\n */
/* 39 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_int> %0\n%%%c = LOAD <@int> %%%c_0\n */
/* 40 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_long> %0\n%%%c = LOAD <@long> %%%c_0\n */
/* 41 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_long> %0\n%%%c = LOAD <@long> %%%c_0\n */
/* 42 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_ptr_void> %0\n%%%c = LOAD <@ptr_void> %%%c_0\n */
/* 43 */	1,	/* %%%c = FPTRUNC <@double @float> %0\n */
/* 44 */	1,	/* %%%c = FPEXT <@float @double> %0\n */
/* 45 */	1,	/* %%%c = FPTOSI <@double @int> %0\n */
/* 46 */	1,	/* %%%c = FPTOSI <@float @int> %0\n */
/* 47 */	1,	/* %%%c = FPTOSI <@double @long> %0\n */
/* 48 */	1,	/* %%%c = FPTOSI <@float @long> %0\n */
/* 49 */	1,	/* %%%c = SITOFP <@char @float> %0\n */
/* 50 */	1,	/* %%%c = SITOFP <@short @float> %0\n */
/* 51 */	1,	/* %%%c = SITOFP <@int @float> %0\n */
/* 52 */	1,	/* %%%c = SITOFP <@long @float> %0\n */
/* 53 */	1,	/* %%%c = SITOFP <@char @double> %0\n */
/* 54 */	1,	/* %%%c = SITOFP <@short @double> %0\n */
/* 55 */	1,	/* %%%c = SITOFP <@int @double> %0\n */
/* 56 */	1,	/* %%%c = SITOFP <@long @double> %0\n */
/* 57 */	1,	/* %%%c = TRUNC <@short @char> %0\n */
/* 58 */	1,	/* %%%c = TRUNC <@int @char> %0\n */
/* 59 */	1,	/* %%%c = TRUNC <@long @char> %0\n */
/* 60 */	1,	/* %%%c = SEXT <@char @short> %0\n */
/* 61 */	1,	/* %%%c = TRUNC <@int @short> %0\n */
/* 62 */	1,	/* %%%c = TRUNC <@long @short> %0\n */
/* 63 */	1,	/* %%%c = SEXT <@char @int> %0\n */
/* 64 */	1,	/* %%%c = SEXT <@short @int> %0\n */
/* 65 */	1,	/* %%%c = TRUNC <@long @int> %0\n */
/* 66 */	1,	/* %%%c = SEXT <@char @long> %0\n */
/* 67 */	1,	/* %%%c = SEXT <@short @long> %0\n */
/* 68 */	1,	/* %%%c = SEXT <@int @long> %0\n */
/* 69 */	1,	/* %%%c = %0\n */
/* 70 */	1,	/* %%%c = TRUNC <@short @char> %0\n */
/* 71 */	1,	/* %%%c = TRUNC <@int @char> %0\n */
/* 72 */	1,	/* %%%c = TRUNC <@long @char> %0\n */
/* 73 */	1,	/* %%%c = ZEXT <@char @short> %0\n */
/* 74 */	1,	/* %%%c = %0\n */
/* 75 */	1,	/* %%%c = TRUNC <@int @short> %0\n */
/* 76 */	1,	/* %%%c = TRUNC <@long @short> %0\n */
/* 77 */	1,	/* %%%c = ZEXT <@char @int> %0\n */
/* 78 */	1,	/* %%%c = ZEXT <@short @int> %0\n */
/* 79 */	1,	/* %%%c = %0\n */
/* 80 */	1,	/* %%%c = TRUNC <@long @int> %0\n */
/* 81 */	1,	/* %%%c = ZEXT <@char @long> %0\n */
/* 82 */	1,	/* %%%c = ZEXT <@short @long> %0\n */
/* 83 */	1,	/* %%%c = ZEXT <@int @long> %0\n */
/* 84 */	1,	/* %%%c = %0\n */
/* 85 */	1,	/* // Pointer to integer conversion not supported\n */
/* 86 */	1,	/* %%%c = %0\n */
/* 87 */	1,	/* %%%c = TRUNC <@short @char> %0\n */
/* 88 */	1,	/* %%%c = TRUNC <@int @char> %0\n */
/* 89 */	1,	/* %%%c = TRUNC <@long @char> %0\n */
/* 90 */	1,	/* %%%c = SEXT <@char @short> %0\n */
/* 91 */	1,	/* %%%c = %0\n */
/* 92 */	1,	/* %%%c = TRUNC <@int @short> %0\n */
/* 93 */	1,	/* %%%c = TRUNC <@long @short> %0\n */
/* 94 */	1,	/* %%%c = SEXT <@char @int> %0\n */
/* 95 */	1,	/* %%%c = SEXT <@short @int> %0\n */
/* 96 */	1,	/* %%%c = %0\n */
/* 97 */	1,	/* %%%c = TRUNC <@long @int> %0\n */
/* 98 */	1,	/* %%%c = SEXT <@char @long> %0\n */
/* 99 */	1,	/* %%%c = SEXT <@short @long> %0\n */
/* 100 */	1,	/* %%%c = SEXT <@int @long> %0\n */
/* 101 */	1,	/* %%%c = %0\n */
/* 102 */	1,	/* // Integer to pointer conversion not supported\n */
/* 103 */	1,	/* // Integer to pointer conversion not supported\n */
/* 104 */	1,	/* // Integer to pointer conversion not supported\n */
/* 105 */	1,	/* // Integer to pointer conversion not supported\n */
/* 106 */	1,	/* %%%c = TRUNC <@short @char> %0\n */
/* 107 */	1,	/* %%%c = TRUNC <@int @char> %0\n */
/* 108 */	1,	/* %%%c = TRUNC <@long @char> %0\n */
/* 109 */	1,	/* %%%c = ZEXT <@char @short> %0\n */
/* 110 */	1,	/* %%%c = TRUNC <@int @short> %0\n */
/* 111 */	1,	/* %%%c = TRUNC <@long @short> %0\n */
/* 112 */	1,	/* %%%c = ZEXT <@char @int> %0\n */
/* 113 */	1,	/* %%%c = ZEXT <@short @int> %0\n */
/* 114 */	1,	/* %%%c = TRUNC <@long @int> %0\n */
/* 115 */	1,	/* %%%c = ZEXT <@char @long> %0\n */
/* 116 */	1,	/* %%%c = ZEXT <@short @long> %0\n */
/* 117 */	1,	/* %%%c = ZEXT <@int @long> %0\n */
/* 118 */	0,	/* // Negate float value %0 */
/* 119 */	0,	/* // Negate double value %0 */
/* 120 */	1,	/* %%%c = SUB <@int> @0_int %0\n */
/* 121 */	1,	/* %%%c = SUB <@long> @0_long %0\n */
/* 122 */	1,	/* // Call (float) unimplemented\n */
/* 123 */	1,	/* // Call (double) unimplemented\n */
/* 124 */	1,	/* // Call (int) unimplemented\n */
/* 125 */	1,	/* // Call (long) unimplemented\n */
/* 126 */	1,	/* // Call (pointer) unimplemented\n */
/* 127 */	1,	/* // Call (uint) unimplemented\n */
/* 128 */	1,	/* // Call (ulong) unimplemented\n */
/* 129 */	1,	/* // Call (void) unimplemented\n */
/* 130 */	1,	/* RET <@float> %0\n */
/* 131 */	1,	/* RET <@double> %0\n */
/* 132 */	1,	/* RET <@int> %0\n */
/* 133 */	1,	/* RET <@long> %0\n */
/* 134 */	1,	/* RET <@_ptr_void> %0\n */
/* 135 */	1,	/* RET <@int> %0\n */
/* 136 */	1,	/* RET <@long> %0\n */
/* 137 */	1,	/* RETVOID\n */
/* 138 */	0,	/* @%%%a */
/* 139 */	0,	/* %%%a */
/* 140 */	0,	/* %%%a */
/* 141 */	1,	/* %%%c = FADD <@float> %0 %1\n */
/* 142 */	1,	/* %%%c = FADD <@double> %0 %1\n */
/* 143 */	1,	/* %%%c = ADD <@int> %0 %1\n */
/* 144 */	1,	/* %%%c = ADD <@long> %0 %1\n */
/* 145 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c_1 = SHIFTIREF <@char @long> %%%c_0 %1\n%%%c = REFCAST <@_ptr_char @_ptr_void> %%%c_1\n */
/* 146 */	1,	/* %%%c = ADD <@int> %0 %1\n */
/* 147 */	1,	/* %%%c = ADD <@long> %0 %1\n */
/* 148 */	1,	/* %%%c = FSUB <@float> %0 %1\n */
/* 149 */	1,	/* %%%c = FSUB <@double> %0 %1\n */
/* 150 */	1,	/* %%%c = SUB <@int> %0 %1\n */
/* 151 */	1,	/* %%%c = SUB <@long> %0 %1\n */
/* 152 */	1,	/* %%%c_0 = REFCAST <@_ptr_void @_ptr_char> %0\n%%%c_1 = SUB <@long> @0_long %1\n%%%c_2 = SHIFTIREF <@char @long> %%%c_0 %%%c_1\n%%%c = REFCAST <@_ptr_char @_ptr_void> %%%c_2\n */
/* 153 */	1,	/* %%%c = SUB <@int> %0 %1\n */
/* 154 */	1,	/* %%%c = SUB <@long> %0 %1\n */
/* 155 */	1,	/* %%%c = SHL <@int> %0 %1\n */
/* 156 */	1,	/* %%%c = SHL <@long> %0 %1\n */
/* 157 */	1,	/* %%%c = SHL <@int> %0 %1\n */
/* 158 */	1,	/* %%%c = SHL <@long> %0 %1\n */
/* 159 */	1,	/* %%%c = SREM <@int> %0 %1\n */
/* 160 */	1,	/* %%%c = SREM <@long> %0 %1\n */
/* 161 */	1,	/* %%%c = UREM <@int> %0 %1\n */
/* 162 */	1,	/* %%%c = UREM <@int> %0 %1\n */
/* 163 */	1,	/* %%%c = ASHR <@int> %0 %1\n */
/* 164 */	1,	/* %%%c = ASHR <@long> %0 %1\n */
/* 165 */	1,	/* %%%c = LSHR <@int> %0 %1\n */
/* 166 */	1,	/* %%%c = LSHR <@long> %0 %1\n */
/* 167 */	1,	/* %%%c = AND <@int> %0 %1\n */
/* 168 */	1,	/* %%%c = AND <@long> %0 %1\n */
/* 169 */	1,	/* %%%c = AND <@int> %0 %1\n */
/* 170 */	1,	/* %%%c = AND <@long> %0 %1\n */
/* 171 */	1,	/* %%%c = XOR <@int> %0 @1_neg_int\n */
/* 172 */	1,	/* %%%c = XOR <@long> %0 @1_neg_long\n */
/* 173 */	1,	/* %%%c = XOR <@int> %0 @1_neg_int\n */
/* 174 */	1,	/* %%%c = XOR <@long> %0 @1_neg_long\n */
/* 175 */	1,	/* %%%c = OR <@int> %0 %1\n */
/* 176 */	1,	/* %%%c = OR <@long> %0 %1\n */
/* 177 */	1,	/* %%%c = OR <@int> %0 %1\n */
/* 178 */	1,	/* %%%c = OR <@long> %0 %1\n */
/* 179 */	1,	/* %%%c = XOR <@int> %0 %1\n */
/* 180 */	1,	/* %%%c = XOR <@int> %0 %1\n */
/* 181 */	1,	/* %%%c = FDIV <@float> %0 %1\n */
/* 182 */	1,	/* %%%c = FDIV <@double> %0 %1\n */
/* 183 */	1,	/* %%%c = SDIV <@int> %0 %1\n */
/* 184 */	1,	/* %%%c = SDIV <@long> %0 %1\n */
/* 185 */	1,	/* %%%c = UDIV <@int> %0 %1\n */
/* 186 */	1,	/* %%%c = UDIV <@long> %0 %1\n */
/* 187 */	1,	/* %%%c = FMUL <@float> %0 %1\n */
/* 188 */	1,	/* %%%c = FMUL <@double> %0 %1\n */
/* 189 */	1,	/* %%%c = MUL <@int> %0 %1\n */
/* 190 */	1,	/* %%%c = MUL <@long> %0 %1\n */
/* 191 */	1,	/* %%%c = MUL <@int> %0 %1\n */
/* 192 */	1,	/* %%%c = MUL <@long> %0 %1\n */
/* 193 */	1,	/* %%%c = FOEQ <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 194 */	1,	/* %%%c = FOEQ <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 195 */	1,	/* %%%c = EQ <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 196 */	1,	/* %%%c = EQ <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 197 */	1,	/* %%%c = EQ <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 198 */	1,	/* %%%c = EQ <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 199 */	1,	/* %%%c = FOGE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 200 */	1,	/* %%%c = FOGE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 201 */	1,	/* #%%%c = SGE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 202 */	1,	/* #%%%c = SGE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 203 */	1,	/* %%%c = UGE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 204 */	1,	/* %%%c = UGE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 205 */	1,	/* %%%c = FOGT <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 206 */	1,	/* %%%c = FOGT <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 207 */	1,	/* %%%c = SGT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 208 */	1,	/* %%%c = SGT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 209 */	1,	/* %%%c = UGT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 210 */	1,	/* %%%c = UGT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 211 */	1,	/* %%%c = FOLE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 212 */	1,	/* %%%c = FOLE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 213 */	1,	/* %%%c = SLE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 214 */	1,	/* %%%c = SLE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 215 */	1,	/* %%%c = ULE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 216 */	1,	/* %%%c = ULE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 217 */	1,	/* %%%c = FOLT <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 218 */	1,	/* %%%c = FOLT <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 219 */	1,	/* %%%c = SLT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 220 */	1,	/* %%%c = SLT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 221 */	1,	/* %%%c = ULT <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 222 */	1,	/* %%%c = ULT <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 223 */	1,	/* %%%c = FONE <@float> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 224 */	1,	/* %%%c = FONE <@double> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 225 */	1,	/* %%%c = NE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 226 */	1,	/* %%%c = NE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 227 */	1,	/* %%%c = NE <@int> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 228 */	1,	/* %%%c = NE <@long> %0 %1\nBRANCH2 %%%c %%%a %%%b\n */
/* 229 */	1,	/* BRANCH %0\n */
/* 230 */	1,	/* %%%a:\n */
};

static char *_string[] = {
/* 0 */	0,
/* 1 */	"float_var: CNSTF4",
/* 2 */	"double_var: CNSTF8",
/* 3 */	"char_var: CNSTI1",
/* 4 */	"char_var: CNSTU1",
/* 5 */	"short_var: CNSTI2",
/* 6 */	"short_var: CNSTU2",
/* 7 */	"int_var: CNSTI4",
/* 8 */	"int_var: CNSTU4",
/* 9 */	"long_var: CNSTI8",
/* 10 */	"long_var: CNSTU8",
/* 11 */	"ptr: CNSTP8",
/* 12 */	"stmt: ARGB(INDIRB(ptr))",
/* 13 */	"stmt: ARGF4(float_var)",
/* 14 */	"stmt: ARGF8(double_var)",
/* 15 */	"stmt: ARGI4(int_var)",
/* 16 */	"stmt: ARGI8(long_var)",
/* 17 */	"stmt: ARGP8(ptr)",
/* 18 */	"stmt: ARGU4(int_var)",
/* 19 */	"stmt: ARGU8(long_var)",
/* 20 */	"stmt: ASGNB(ptr,INDIRB(ptr))",
/* 21 */	"stmt: ASGNF4(ptr,float_var)",
/* 22 */	"stmt: ASGNF8(ptr,double_var)",
/* 23 */	"stmt: ASGNI1(ptr,char_var)",
/* 24 */	"stmt: ASGNU1(ptr,char_var)",
/* 25 */	"stmt: ASGNI2(ptr,short_var)",
/* 26 */	"stmt: ASGNU2(ptr,short_var)",
/* 27 */	"stmt: ASGNI4(ptr,int_var)",
/* 28 */	"stmt: ASGNU4(ptr,int_var)",
/* 29 */	"stmt: ASGNI8(ptr,long_var)",
/* 30 */	"stmt: ASGNU8(ptr,long_var)",
/* 31 */	"stmt: ASGNP8(ptr,ptr)",
/* 32 */	"float_var: INDIRF4(ptr)",
/* 33 */	"double_var: INDIRF8(ptr)",
/* 34 */	"char_var: INDIRI1(ptr)",
/* 35 */	"char_var: INDIRU1(ptr)",
/* 36 */	"short_var: INDIRI2(ptr)",
/* 37 */	"short_var: INDIRU2(ptr)",
/* 38 */	"int_var: INDIRI4(ptr)",
/* 39 */	"int_var: INDIRU4(ptr)",
/* 40 */	"long_var: INDIRI8(ptr)",
/* 41 */	"long_var: INDIRU8(ptr)",
/* 42 */	"ptr: INDIRP8(ptr)",
/* 43 */	"float_var: CVFF4(double_var)",
/* 44 */	"double_var: CVFF8(float_var)",
/* 45 */	"int_var: CVFI4(double_var)",
/* 46 */	"int_var: CVFI4(float_var)",
/* 47 */	"long_var: CVFI8(double_var)",
/* 48 */	"long_var: CVFI8(float_var)",
/* 49 */	"float_var: CVIF4(char_var)",
/* 50 */	"float_var: CVIF4(short_var)",
/* 51 */	"float_var: CVIF4(int_var)",
/* 52 */	"float_var: CVIF4(long_var)",
/* 53 */	"double_var: CVIF8(char_var)",
/* 54 */	"double_var: CVIF8(short_var)",
/* 55 */	"double_var: CVIF8(int_var)",
/* 56 */	"double_var: CVIF8(long_var)",
/* 57 */	"char_var: CVII1(short_var)",
/* 58 */	"char_var: CVII1(int_var)",
/* 59 */	"char_var: CVII1(long_var)",
/* 60 */	"short_var: CVII2(char_var)",
/* 61 */	"short_var: CVII2(int_var)",
/* 62 */	"short_var: CVII2(long_var)",
/* 63 */	"int_var: CVII4(char_var)",
/* 64 */	"int_var: CVII4(short_var)",
/* 65 */	"int_var: CVII4(long_var)",
/* 66 */	"long_var: CVII8(char_var)",
/* 67 */	"long_var: CVII8(short_var)",
/* 68 */	"long_var: CVII8(int_var)",
/* 69 */	"char_var: CVIU1(char_var)",
/* 70 */	"char_var: CVIU1(short_var)",
/* 71 */	"char_var: CVIU1(int_var)",
/* 72 */	"char_var: CVIU1(long_var)",
/* 73 */	"short_var: CVIU2(char_var)",
/* 74 */	"short_var: CVIU2(short_var)",
/* 75 */	"short_var: CVIU2(int_var)",
/* 76 */	"short_var: CVIU2(long_var)",
/* 77 */	"int_var: CVIU4(char_var)",
/* 78 */	"int_var: CVIU4(short_var)",
/* 79 */	"int_var: CVIU4(int_var)",
/* 80 */	"int_var: CVIU4(long_var)",
/* 81 */	"long_var: CVIU8(char_var)",
/* 82 */	"long_var: CVIU8(short_var)",
/* 83 */	"long_var: CVIU8(int_var)",
/* 84 */	"long_var: CVIU8(long_var)",
/* 85 */	"long_var: CVPU8(ptr)",
/* 86 */	"char_var: CVUI1(char_var)",
/* 87 */	"char_var: CVUI1(short_var)",
/* 88 */	"char_var: CVUI1(int_var)",
/* 89 */	"char_var: CVUI1(long_var)",
/* 90 */	"short_var: CVUI2(char_var)",
/* 91 */	"short_var: CVUI2(short_var)",
/* 92 */	"short_var: CVUI2(int_var)",
/* 93 */	"short_var: CVUI2(long_var)",
/* 94 */	"int_var: CVUI4(char_var)",
/* 95 */	"int_var: CVUI4(short_var)",
/* 96 */	"int_var: CVUI4(int_var)",
/* 97 */	"int_var: CVUI4(long_var)",
/* 98 */	"long_var: CVUI8(char_var)",
/* 99 */	"long_var: CVUI8(short_var)",
/* 100 */	"long_var: CVUI8(int_var)",
/* 101 */	"long_var: CVUI8(int_var)",
/* 102 */	"ptr: CVUP8(char_var)",
/* 103 */	"ptr: CVUP8(short_var)",
/* 104 */	"ptr: CVUP8(int_var)",
/* 105 */	"ptr: CVUP8(long_var)",
/* 106 */	"char_var: CVUU1(short_var)",
/* 107 */	"char_var: CVUU1(int_var)",
/* 108 */	"char_var: CVUU1(long_var)",
/* 109 */	"short_var: CVUU2(char_var)",
/* 110 */	"short_var: CVUU2(int_var)",
/* 111 */	"short_var: CVUU2(long_var)",
/* 112 */	"int_var: CVUU4(char_var)",
/* 113 */	"int_var: CVUU4(short_var)",
/* 114 */	"int_var: CVUU4(long_var)",
/* 115 */	"long_var: CVUU8(char_var)",
/* 116 */	"long_var: CVUU8(short_var)",
/* 117 */	"long_var: CVUU8(int_var)",
/* 118 */	"float_var: NEGF4(float_var)",
/* 119 */	"double_var: NEGF4(double_var)",
/* 120 */	"int_var: NEGI4(int_var)",
/* 121 */	"long_var: NEGI8(long_var)",
/* 122 */	"float_var: CALLF4(ptr)",
/* 123 */	"double_var: CALLF8(ptr)",
/* 124 */	"int_var: CALLI4(ptr)",
/* 125 */	"long_var: CALLI8(ptr)",
/* 126 */	"ptr: CALLP8(ptr)",
/* 127 */	"int_var: CALLU4(ptr)",
/* 128 */	"long_var: CALLU8(ptr)",
/* 129 */	"stmt: CALLV(ptr)",
/* 130 */	"stmt: RETF4(float_var)",
/* 131 */	"stmt: RETF8(double_var)",
/* 132 */	"stmt: RETI4(int_var)",
/* 133 */	"stmt: RETI8(long_var)",
/* 134 */	"stmt: RETP8(ptr)",
/* 135 */	"stmt: RETU4(int_var)",
/* 136 */	"stmt: RETU8(long_var)",
/* 137 */	"stmt: RETV",
/* 138 */	"ptr: ADDRGP8",
/* 139 */	"ptr: ADDRLP8",
/* 140 */	"ptr: ADDRFP8",
/* 141 */	"float_var: ADDF4(float_var,float_var)",
/* 142 */	"double_var: ADDF8(double_var,double_var)",
/* 143 */	"int_var: ADDI4(int_var,int_var)",
/* 144 */	"long_var: ADDI8(long_var,long_var)",
/* 145 */	"ptr: ADDP8(ptr,long_var)",
/* 146 */	"int_var: ADDU4(int_var,int_var)",
/* 147 */	"long_var: ADDU8(long_var,long_var)",
/* 148 */	"float_var: SUBF4(float_var,float_var)",
/* 149 */	"double_var: SUBF8(double_var,double_var)",
/* 150 */	"int_var: SUBI4(int_var,int_var)",
/* 151 */	"long_var: SUBI8(long_var,long_var)",
/* 152 */	"ptr: SUBP8(ptr,long_var)",
/* 153 */	"int_var: SUBU4(int_var,int_var)",
/* 154 */	"long_var: SUBU8(long_var,long_var)",
/* 155 */	"int_var: LSHI4(int_var,int_var)",
/* 156 */	"long_var: LSHI8(long_var,long_var)",
/* 157 */	"int_var: LSHU4(int_var,int_var)",
/* 158 */	"long_var: LSHU8(long_var,long_var)",
/* 159 */	"int_var: MODI4(int_var,int_var)",
/* 160 */	"long_var: MODI8(long_var,long_var)",
/* 161 */	"int_var: MODU4(int_var,int_var)",
/* 162 */	"long_var: MODU8(long_var,long_var)",
/* 163 */	"int_var: RSHI4(int_var,int_var)",
/* 164 */	"long_var: RSHI8(long_var,long_var)",
/* 165 */	"int_var: RSHU4(int_var,int_var)",
/* 166 */	"long_var: RSHU8(long_var,long_var)",
/* 167 */	"int_var: BANDI4(int_var,int_var)",
/* 168 */	"long_var: BANDI8(long_var,long_var)",
/* 169 */	"int_var: BANDU4(int_var,int_var)",
/* 170 */	"long_var: BANDU8(long_var,long_var)",
/* 171 */	"int_var: BCOMI4(int_var)",
/* 172 */	"long_var: BCOMI8(long_var)",
/* 173 */	"int_var: BCOMU4(int_var)",
/* 174 */	"long_var: BCOMU8(long_var)",
/* 175 */	"int_var: BORI4(int_var)",
/* 176 */	"long_var: BORI8(long_var)",
/* 177 */	"int_var: BORU4(int_var)",
/* 178 */	"long_var: BORU8(long_var)",
/* 179 */	"int_var: BXORI4(int_var)",
/* 180 */	"long_var: BXORI8(long_var)",
/* 181 */	"float_var: DIVF4(float_var,float_var)",
/* 182 */	"double_var: DIVF8(double_var,double_var)",
/* 183 */	"int_var: DIVI4(int_var,int_var)",
/* 184 */	"long_var: DIVI8(long_var,long_var)",
/* 185 */	"int_var: DIVU4(int_var,int_var)",
/* 186 */	"long_var: DIVU8(long_var,long_var)",
/* 187 */	"float_var: MULF4(float_var,float_var)",
/* 188 */	"double_var: MULF8(double_var,double_var)",
/* 189 */	"int_var: MULI4(int_var,int_var)",
/* 190 */	"long_var: MULI8(long_var,long_var)",
/* 191 */	"int_var: MULU4(int_var,int_var)",
/* 192 */	"long_var: MULU8(long_var,long_var)",
/* 193 */	"stmt: EQF4(float_var,float_var)",
/* 194 */	"stmt: EQF8(double_var,double_var)",
/* 195 */	"stmt: EQI4(int_var,int_var)",
/* 196 */	"stmt: EQI8(long_var,long_var)",
/* 197 */	"stmt: EQU4(int_var,int_var)",
/* 198 */	"stmt: EQU8(long_var,long_var)",
/* 199 */	"stmt: GEF4(float_var,float_var)",
/* 200 */	"stmt: GEF8(double_var,double_var)",
/* 201 */	"stmt: GEI4(int_var,int_var)",
/* 202 */	"stmt: GEI8(long_var,long_var)",
/* 203 */	"stmt: GEU4(int_var,int_var)",
/* 204 */	"stmt: GEU8(long_var,long_var)",
/* 205 */	"stmt: GTF4(float_var,float_var)",
/* 206 */	"stmt: GTF8(double_var,double_var)",
/* 207 */	"stmt: GTI4(int_var,int_var)",
/* 208 */	"stmt: GTI8(long_var,long_var)",
/* 209 */	"stmt: GTU4(int_var,int_var)",
/* 210 */	"stmt: GTU8(long_var,long_var)",
/* 211 */	"stmt: LEF4(float_var,float_var)",
/* 212 */	"stmt: LEF8(double_var,double_var)",
/* 213 */	"stmt: LEI4(int_var,int_var)",
/* 214 */	"stmt: LEI8(long_var,long_var)",
/* 215 */	"stmt: LEU4(int_var,int_var)",
/* 216 */	"stmt: LEU8(long_var,long_var)",
/* 217 */	"stmt: LTF4(float_var,float_var)",
/* 218 */	"stmt: LTF8(double_var,double_var)",
/* 219 */	"stmt: LTI4(int_var,int_var)",
/* 220 */	"stmt: LTI8(long_var,long_var)",
/* 221 */	"stmt: LTU4(int_var,int_var)",
/* 222 */	"stmt: LTU8(long_var,long_var)",
/* 223 */	"stmt: NEF4(float_var,float_var)",
/* 224 */	"stmt: NEF8(double_var,double_var)",
/* 225 */	"stmt: NEI4(int_var,int_var)",
/* 226 */	"stmt: NEI8(long_var,long_var)",
/* 227 */	"stmt: NEU4(int_var,int_var)",
/* 228 */	"stmt: NEU8(long_var,long_var)",
/* 229 */	"stmt: JUMPV(ptr)",
/* 230 */	"stmt: LABELV",
};

static short _decode_stmt[] = {
	0,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	129,
	130,
	131,
	132,
	133,
	134,
	135,
	136,
	137,
	193,
	194,
	195,
	196,
	197,
	198,
	199,
	200,
	201,
	202,
	203,
	204,
	205,
	206,
	207,
	208,
	209,
	210,
	211,
	212,
	213,
	214,
	215,
	216,
	217,
	218,
	219,
	220,
	221,
	222,
	223,
	224,
	225,
	226,
	227,
	228,
	229,
	230,
};

static short _decode_float_var[] = {
	0,
	1,
	32,
	43,
	49,
	50,
	51,
	52,
	118,
	122,
	141,
	148,
	181,
	187,
};

static short _decode_double_var[] = {
	0,
	2,
	33,
	44,
	53,
	54,
	55,
	56,
	119,
	123,
	142,
	149,
	182,
	188,
};

static short _decode_char_var[] = {
	0,
	3,
	4,
	34,
	35,
	57,
	58,
	59,
	69,
	70,
	71,
	72,
	86,
	87,
	88,
	89,
	106,
	107,
	108,
};

static short _decode_short_var[] = {
	0,
	5,
	6,
	36,
	37,
	60,
	61,
	62,
	73,
	74,
	75,
	76,
	90,
	91,
	92,
	93,
	109,
	110,
	111,
};

static short _decode_int_var[] = {
	0,
	7,
	8,
	38,
	39,
	45,
	46,
	63,
	64,
	65,
	77,
	78,
	79,
	80,
	94,
	95,
	96,
	97,
	112,
	113,
	114,
	120,
	124,
	127,
	143,
	146,
	150,
	153,
	155,
	157,
	159,
	161,
	163,
	165,
	167,
	169,
	171,
	173,
	175,
	177,
	179,
	183,
	185,
	189,
	191,
};

static short _decode_long_var[] = {
	0,
	9,
	10,
	40,
	41,
	47,
	48,
	66,
	67,
	68,
	81,
	82,
	83,
	84,
	85,
	98,
	99,
	100,
	101,
	115,
	116,
	117,
	121,
	125,
	128,
	144,
	147,
	151,
	154,
	156,
	158,
	160,
	162,
	164,
	166,
	168,
	170,
	172,
	174,
	176,
	178,
	180,
	184,
	186,
	190,
	192,
};

static short _decode_ptr[] = {
	0,
	11,
	42,
	102,
	103,
	104,
	105,
	126,
	138,
	139,
	140,
	145,
	152,
};

static int _rule(void *state, int goalnt) {
	if (goalnt < 1 || goalnt > 8)
		fatal("_rule", "Bad goal nonterminal %d\n", goalnt);
	if (!state)
		return 0;
	switch (goalnt) {
	case _stmt_NT:	return _decode_stmt[((struct _state *)state)->rule._stmt];
	case _float_var_NT:	return _decode_float_var[((struct _state *)state)->rule._float_var];
	case _double_var_NT:	return _decode_double_var[((struct _state *)state)->rule._double_var];
	case _char_var_NT:	return _decode_char_var[((struct _state *)state)->rule._char_var];
	case _short_var_NT:	return _decode_short_var[((struct _state *)state)->rule._short_var];
	case _int_var_NT:	return _decode_int_var[((struct _state *)state)->rule._int_var];
	case _long_var_NT:	return _decode_long_var[((struct _state *)state)->rule._long_var];
	case _ptr_NT:	return _decode_ptr[((struct _state *)state)->rule._ptr];
	default:
		fatal("_rule", "Bad goal nonterminal %d\n", goalnt);
		return 0;
	}
}


static void _label(NODEPTR_TYPE a) {
	int c;
	struct _state *p;

	if (!a)
		fatal("_label", "Null tree\n", 0);
	STATE_LABEL(a) = p = allocate(sizeof *p, FUNC);
	p->rule._stmt = 0;
	p->cost[1] =
	p->cost[2] =
	p->cost[3] =
	p->cost[4] =
	p->cost[5] =
	p->cost[6] =
	p->cost[7] =
	p->cost[8] =
		0x7fff;
	switch (OP_LABEL(a)) {
	case 41: /* ARGB */
		_label(LEFT_CHILD(a));
		if (	/* stmt: ARGB(INDIRB(ptr)) */
			LEFT_CHILD(a)->op == 73 /* INDIRB */
		) {
			c = ((struct _state *)(LEFT_CHILD(LEFT_CHILD(a))->x.state))->cost[_ptr_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 1;
			}
		}
		break;
	case 57: /* ASGNB */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		if (	/* stmt: ASGNB(ptr,INDIRB(ptr)) */
			RIGHT_CHILD(a)->op == 73 /* INDIRB */
		) {
			c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(LEFT_CHILD(RIGHT_CHILD(a))->x.state))->cost[_ptr_NT] + 0;
			if (c + 0 < p->cost[_stmt_NT]) {
				p->cost[_stmt_NT] = c + 0;
				p->rule._stmt = 9;
			}
		}
		break;
	case 73: /* INDIRB */
		_label(LEFT_CHILD(a));
		break;
	case 216: /* CALLV */
		_label(LEFT_CHILD(a));
		/* stmt: CALLV(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 21;
		}
		break;
	case 217: /* CALLB */
		break;
	case 248: /* RETV */
		/* stmt: RETV */
		if (0 + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = 0 + 0;
			p->rule._stmt = 29;
		}
		break;
	case 584: /* JUMPV */
		_label(LEFT_CHILD(a));
		/* stmt: JUMPV(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 66;
		}
		break;
	case 600: /* LABELV */
		/* stmt: LABELV */
		if (0 + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = 0 + 0;
			p->rule._stmt = 67;
		}
		break;
	case 1045: /* CNSTI1 */
		/* char_var: CNSTI1 */
		if (0 + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = 0 + 0;
			p->rule._char_var = 1;
		}
		break;
	case 1046: /* CNSTU1 */
		/* char_var: CNSTU1 */
		if (0 + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = 0 + 0;
			p->rule._char_var = 2;
		}
		break;
	case 1077: /* ASGNI1 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNI1(ptr,char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 12;
		}
		break;
	case 1078: /* ASGNU1 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNU1(ptr,char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 13;
		}
		break;
	case 1093: /* INDIRI1 */
		_label(LEFT_CHILD(a));
		/* char_var: INDIRI1(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 3;
		}
		break;
	case 1094: /* INDIRU1 */
		_label(LEFT_CHILD(a));
		/* char_var: INDIRU1(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 4;
		}
		break;
	case 1157: /* CVII1 */
		_label(LEFT_CHILD(a));
		/* char_var: CVII1(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 5;
		}
		/* char_var: CVII1(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 6;
		}
		/* char_var: CVII1(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 7;
		}
		break;
	case 1158: /* CVIU1 */
		_label(LEFT_CHILD(a));
		/* char_var: CVIU1(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 8;
		}
		/* char_var: CVIU1(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 9;
		}
		/* char_var: CVIU1(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 10;
		}
		/* char_var: CVIU1(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 11;
		}
		break;
	case 1205: /* CVUI1 */
		_label(LEFT_CHILD(a));
		/* char_var: CVUI1(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 12;
		}
		/* char_var: CVUI1(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 13;
		}
		/* char_var: CVUI1(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 14;
		}
		/* char_var: CVUI1(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 15;
		}
		break;
	case 1206: /* CVUU1 */
		_label(LEFT_CHILD(a));
		/* char_var: CVUU1(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 16;
		}
		/* char_var: CVUU1(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 17;
		}
		/* char_var: CVUU1(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_char_var_NT]) {
			p->cost[_char_var_NT] = c + 0;
			p->rule._char_var = 18;
		}
		break;
	case 2069: /* CNSTI2 */
		/* short_var: CNSTI2 */
		if (0 + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = 0 + 0;
			p->rule._short_var = 1;
		}
		break;
	case 2070: /* CNSTU2 */
		/* short_var: CNSTU2 */
		if (0 + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = 0 + 0;
			p->rule._short_var = 2;
		}
		break;
	case 2101: /* ASGNI2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNI2(ptr,short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 14;
		}
		break;
	case 2102: /* ASGNU2 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNU2(ptr,short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 15;
		}
		break;
	case 2117: /* INDIRI2 */
		_label(LEFT_CHILD(a));
		/* short_var: INDIRI2(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 3;
		}
		break;
	case 2118: /* INDIRU2 */
		_label(LEFT_CHILD(a));
		/* short_var: INDIRU2(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 4;
		}
		break;
	case 2181: /* CVII2 */
		_label(LEFT_CHILD(a));
		/* short_var: CVII2(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 5;
		}
		/* short_var: CVII2(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 6;
		}
		/* short_var: CVII2(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 7;
		}
		break;
	case 2182: /* CVIU2 */
		_label(LEFT_CHILD(a));
		/* short_var: CVIU2(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 8;
		}
		/* short_var: CVIU2(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 9;
		}
		/* short_var: CVIU2(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 10;
		}
		/* short_var: CVIU2(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 11;
		}
		break;
	case 2229: /* CVUI2 */
		_label(LEFT_CHILD(a));
		/* short_var: CVUI2(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 12;
		}
		/* short_var: CVUI2(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 13;
		}
		/* short_var: CVUI2(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 14;
		}
		/* short_var: CVUI2(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 15;
		}
		break;
	case 2230: /* CVUU2 */
		_label(LEFT_CHILD(a));
		/* short_var: CVUU2(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 16;
		}
		/* short_var: CVUU2(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 17;
		}
		/* short_var: CVUU2(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_short_var_NT]) {
			p->cost[_short_var_NT] = c + 0;
			p->rule._short_var = 18;
		}
		break;
	case 4113: /* CNSTF4 */
		/* float_var: CNSTF4 */
		if (0 + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = 0 + 0;
			p->rule._float_var = 1;
		}
		break;
	case 4117: /* CNSTI4 */
		/* int_var: CNSTI4 */
		if (0 + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = 0 + 0;
			p->rule._int_var = 1;
		}
		break;
	case 4118: /* CNSTU4 */
		/* int_var: CNSTU4 */
		if (0 + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = 0 + 0;
			p->rule._int_var = 2;
		}
		break;
	case 4129: /* ARGF4 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGF4(float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 2;
		}
		break;
	case 4133: /* ARGI4 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 4;
		}
		break;
	case 4134: /* ARGU4 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGU4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 7;
		}
		break;
	case 4145: /* ASGNF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNF4(ptr,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 10;
		}
		break;
	case 4149: /* ASGNI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNI4(ptr,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 16;
		}
		break;
	case 4150: /* ASGNU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNU4(ptr,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 17;
		}
		break;
	case 4161: /* INDIRF4 */
		_label(LEFT_CHILD(a));
		/* float_var: INDIRF4(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 2;
		}
		break;
	case 4165: /* INDIRI4 */
		_label(LEFT_CHILD(a));
		/* int_var: INDIRI4(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 3;
		}
		break;
	case 4166: /* INDIRU4 */
		_label(LEFT_CHILD(a));
		/* int_var: INDIRU4(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 4;
		}
		break;
	case 4209: /* CVFF4 */
		_label(LEFT_CHILD(a));
		/* float_var: CVFF4(double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 3;
		}
		break;
	case 4213: /* CVFI4 */
		_label(LEFT_CHILD(a));
		/* int_var: CVFI4(double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 5;
		}
		/* int_var: CVFI4(float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 6;
		}
		break;
	case 4225: /* CVIF4 */
		_label(LEFT_CHILD(a));
		/* float_var: CVIF4(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 4;
		}
		/* float_var: CVIF4(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 5;
		}
		/* float_var: CVIF4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 6;
		}
		/* float_var: CVIF4(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 7;
		}
		break;
	case 4229: /* CVII4 */
		_label(LEFT_CHILD(a));
		/* int_var: CVII4(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 7;
		}
		/* int_var: CVII4(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 8;
		}
		/* int_var: CVII4(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 9;
		}
		break;
	case 4230: /* CVIU4 */
		_label(LEFT_CHILD(a));
		/* int_var: CVIU4(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 10;
		}
		/* int_var: CVIU4(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 11;
		}
		/* int_var: CVIU4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 12;
		}
		/* int_var: CVIU4(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 13;
		}
		break;
	case 4277: /* CVUI4 */
		_label(LEFT_CHILD(a));
		/* int_var: CVUI4(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 14;
		}
		/* int_var: CVUI4(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 15;
		}
		/* int_var: CVUI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 16;
		}
		/* int_var: CVUI4(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 17;
		}
		break;
	case 4278: /* CVUU4 */
		_label(LEFT_CHILD(a));
		/* int_var: CVUU4(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 18;
		}
		/* int_var: CVUU4(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 19;
		}
		/* int_var: CVUU4(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 20;
		}
		break;
	case 4289: /* NEGF4 */
		_label(LEFT_CHILD(a));
		/* float_var: NEGF4(float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 8;
		}
		/* double_var: NEGF4(double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 8;
		}
		break;
	case 4293: /* NEGI4 */
		_label(LEFT_CHILD(a));
		/* int_var: NEGI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 21;
		}
		break;
	case 4305: /* CALLF4 */
		_label(LEFT_CHILD(a));
		/* float_var: CALLF4(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 9;
		}
		break;
	case 4309: /* CALLI4 */
		_label(LEFT_CHILD(a));
		/* int_var: CALLI4(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 22;
		}
		break;
	case 4310: /* CALLU4 */
		_label(LEFT_CHILD(a));
		/* int_var: CALLU4(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 23;
		}
		break;
	case 4337: /* RETF4 */
		_label(LEFT_CHILD(a));
		/* stmt: RETF4(float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 22;
		}
		break;
	case 4341: /* RETI4 */
		_label(LEFT_CHILD(a));
		/* stmt: RETI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 24;
		}
		break;
	case 4342: /* RETU4 */
		_label(LEFT_CHILD(a));
		/* stmt: RETU4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 27;
		}
		break;
	case 4401: /* ADDF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* float_var: ADDF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 10;
		}
		break;
	case 4405: /* ADDI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: ADDI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 24;
		}
		break;
	case 4406: /* ADDU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: ADDU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 25;
		}
		break;
	case 4417: /* SUBF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* float_var: SUBF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 11;
		}
		break;
	case 4421: /* SUBI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: SUBI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 26;
		}
		break;
	case 4422: /* SUBU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: SUBU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 27;
		}
		break;
	case 4437: /* LSHI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: LSHI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 28;
		}
		break;
	case 4438: /* LSHU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: LSHU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 29;
		}
		break;
	case 4453: /* MODI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: MODI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 30;
		}
		break;
	case 4454: /* MODU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: MODU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 31;
		}
		break;
	case 4469: /* RSHI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: RSHI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 32;
		}
		break;
	case 4470: /* RSHU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: RSHU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 33;
		}
		break;
	case 4485: /* BANDI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: BANDI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 34;
		}
		break;
	case 4486: /* BANDU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: BANDU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 35;
		}
		break;
	case 4501: /* BCOMI4 */
		_label(LEFT_CHILD(a));
		/* int_var: BCOMI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 36;
		}
		break;
	case 4502: /* BCOMU4 */
		_label(LEFT_CHILD(a));
		/* int_var: BCOMU4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 37;
		}
		break;
	case 4517: /* BORI4 */
		_label(LEFT_CHILD(a));
		/* int_var: BORI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 38;
		}
		break;
	case 4518: /* BORU4 */
		_label(LEFT_CHILD(a));
		/* int_var: BORU4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 39;
		}
		break;
	case 4533: /* BXORI4 */
		_label(LEFT_CHILD(a));
		/* int_var: BXORI4(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 40;
		}
		break;
	case 4534: /* BXORU4 */
		break;
	case 4545: /* DIVF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* float_var: DIVF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 12;
		}
		break;
	case 4549: /* DIVI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: DIVI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 41;
		}
		break;
	case 4550: /* DIVU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: DIVU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 42;
		}
		break;
	case 4561: /* MULF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* float_var: MULF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_float_var_NT]) {
			p->cost[_float_var_NT] = c + 0;
			p->rule._float_var = 13;
		}
		break;
	case 4565: /* MULI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: MULI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 43;
		}
		break;
	case 4566: /* MULU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* int_var: MULU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_int_var_NT]) {
			p->cost[_int_var_NT] = c + 0;
			p->rule._int_var = 44;
		}
		break;
	case 4577: /* EQF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 30;
		}
		break;
	case 4581: /* EQI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 32;
		}
		break;
	case 4582: /* EQU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 34;
		}
		break;
	case 4593: /* GEF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 36;
		}
		break;
	case 4597: /* GEI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 38;
		}
		break;
	case 4598: /* GEU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 40;
		}
		break;
	case 4609: /* GTF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 42;
		}
		break;
	case 4613: /* GTI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 44;
		}
		break;
	case 4614: /* GTU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 46;
		}
		break;
	case 4625: /* LEF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 48;
		}
		break;
	case 4629: /* LEI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 50;
		}
		break;
	case 4630: /* LEU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 52;
		}
		break;
	case 4641: /* LTF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 54;
		}
		break;
	case 4645: /* LTI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 56;
		}
		break;
	case 4646: /* LTU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 58;
		}
		break;
	case 4657: /* NEF4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEF4(float_var,float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 60;
		}
		break;
	case 4661: /* NEI4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEI4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 62;
		}
		break;
	case 4662: /* NEU4 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEU4(int_var,int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 64;
		}
		break;
	case 8209: /* CNSTF8 */
		/* double_var: CNSTF8 */
		if (0 + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = 0 + 0;
			p->rule._double_var = 1;
		}
		break;
	case 8213: /* CNSTI8 */
		/* long_var: CNSTI8 */
		if (0 + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = 0 + 0;
			p->rule._long_var = 1;
		}
		break;
	case 8214: /* CNSTU8 */
		/* long_var: CNSTU8 */
		if (0 + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = 0 + 0;
			p->rule._long_var = 2;
		}
		break;
	case 8215: /* CNSTP8 */
		/* ptr: CNSTP8 */
		if (0 + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = 0 + 0;
			p->rule._ptr = 1;
		}
		break;
	case 8225: /* ARGF8 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGF8(double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 3;
		}
		break;
	case 8229: /* ARGI8 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGI8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 5;
		}
		break;
	case 8230: /* ARGU8 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGU8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 8;
		}
		break;
	case 8231: /* ARGP8 */
		_label(LEFT_CHILD(a));
		/* stmt: ARGP8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 6;
		}
		break;
	case 8241: /* ASGNF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNF8(ptr,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 11;
		}
		break;
	case 8245: /* ASGNI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNI8(ptr,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 18;
		}
		break;
	case 8246: /* ASGNU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNU8(ptr,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 19;
		}
		break;
	case 8247: /* ASGNP8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: ASGNP8(ptr,ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 20;
		}
		break;
	case 8257: /* INDIRF8 */
		_label(LEFT_CHILD(a));
		/* double_var: INDIRF8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 2;
		}
		break;
	case 8261: /* INDIRI8 */
		_label(LEFT_CHILD(a));
		/* long_var: INDIRI8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 3;
		}
		break;
	case 8262: /* INDIRU8 */
		_label(LEFT_CHILD(a));
		/* long_var: INDIRU8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 4;
		}
		break;
	case 8263: /* INDIRP8 */
		_label(LEFT_CHILD(a));
		/* ptr: INDIRP8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 2;
		}
		break;
	case 8305: /* CVFF8 */
		_label(LEFT_CHILD(a));
		/* double_var: CVFF8(float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 3;
		}
		break;
	case 8309: /* CVFI8 */
		_label(LEFT_CHILD(a));
		/* long_var: CVFI8(double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 5;
		}
		/* long_var: CVFI8(float_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_float_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 6;
		}
		break;
	case 8321: /* CVIF8 */
		_label(LEFT_CHILD(a));
		/* double_var: CVIF8(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 4;
		}
		/* double_var: CVIF8(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 5;
		}
		/* double_var: CVIF8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 6;
		}
		/* double_var: CVIF8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 7;
		}
		break;
	case 8325: /* CVII8 */
		_label(LEFT_CHILD(a));
		/* long_var: CVII8(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 7;
		}
		/* long_var: CVII8(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 8;
		}
		/* long_var: CVII8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 9;
		}
		break;
	case 8326: /* CVIU8 */
		_label(LEFT_CHILD(a));
		/* long_var: CVIU8(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 10;
		}
		/* long_var: CVIU8(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 11;
		}
		/* long_var: CVIU8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 12;
		}
		/* long_var: CVIU8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 13;
		}
		break;
	case 8342: /* CVPU8 */
		_label(LEFT_CHILD(a));
		/* long_var: CVPU8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 14;
		}
		break;
	case 8373: /* CVUI8 */
		_label(LEFT_CHILD(a));
		/* long_var: CVUI8(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 15;
		}
		/* long_var: CVUI8(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 16;
		}
		/* long_var: CVUI8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 17;
		}
		/* long_var: CVUI8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 18;
		}
		break;
	case 8374: /* CVUU8 */
		_label(LEFT_CHILD(a));
		/* long_var: CVUU8(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 19;
		}
		/* long_var: CVUU8(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 20;
		}
		/* long_var: CVUU8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 21;
		}
		break;
	case 8375: /* CVUP8 */
		_label(LEFT_CHILD(a));
		/* ptr: CVUP8(char_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_char_var_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 3;
		}
		/* ptr: CVUP8(short_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_short_var_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 4;
		}
		/* ptr: CVUP8(int_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_int_var_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 5;
		}
		/* ptr: CVUP8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 6;
		}
		break;
	case 8385: /* NEGF8 */
		break;
	case 8389: /* NEGI8 */
		_label(LEFT_CHILD(a));
		/* long_var: NEGI8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 22;
		}
		break;
	case 8401: /* CALLF8 */
		_label(LEFT_CHILD(a));
		/* double_var: CALLF8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 9;
		}
		break;
	case 8405: /* CALLI8 */
		_label(LEFT_CHILD(a));
		/* long_var: CALLI8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 23;
		}
		break;
	case 8406: /* CALLU8 */
		_label(LEFT_CHILD(a));
		/* long_var: CALLU8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 24;
		}
		break;
	case 8407: /* CALLP8 */
		_label(LEFT_CHILD(a));
		/* ptr: CALLP8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 7;
		}
		break;
	case 8433: /* RETF8 */
		_label(LEFT_CHILD(a));
		/* stmt: RETF8(double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 23;
		}
		break;
	case 8437: /* RETI8 */
		_label(LEFT_CHILD(a));
		/* stmt: RETI8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 25;
		}
		break;
	case 8438: /* RETU8 */
		_label(LEFT_CHILD(a));
		/* stmt: RETU8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 28;
		}
		break;
	case 8439: /* RETP8 */
		_label(LEFT_CHILD(a));
		/* stmt: RETP8(ptr) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 26;
		}
		break;
	case 8455: /* ADDRGP8 */
		/* ptr: ADDRGP8 */
		if (0 + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = 0 + 0;
			p->rule._ptr = 8;
		}
		break;
	case 8471: /* ADDRFP8 */
		/* ptr: ADDRFP8 */
		if (0 + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = 0 + 0;
			p->rule._ptr = 10;
		}
		break;
	case 8487: /* ADDRLP8 */
		/* ptr: ADDRLP8 */
		if (0 + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = 0 + 0;
			p->rule._ptr = 9;
		}
		break;
	case 8497: /* ADDF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* double_var: ADDF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 10;
		}
		break;
	case 8501: /* ADDI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: ADDI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 25;
		}
		break;
	case 8502: /* ADDU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: ADDU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 26;
		}
		break;
	case 8503: /* ADDP8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ptr: ADDP8(ptr,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 11;
		}
		break;
	case 8513: /* SUBF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* double_var: SUBF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 11;
		}
		break;
	case 8517: /* SUBI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: SUBI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 27;
		}
		break;
	case 8518: /* SUBU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: SUBU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 28;
		}
		break;
	case 8519: /* SUBP8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* ptr: SUBP8(ptr,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_ptr_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_ptr_NT]) {
			p->cost[_ptr_NT] = c + 0;
			p->rule._ptr = 12;
		}
		break;
	case 8533: /* LSHI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: LSHI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 29;
		}
		break;
	case 8534: /* LSHU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: LSHU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 30;
		}
		break;
	case 8549: /* MODI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: MODI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 31;
		}
		break;
	case 8550: /* MODU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: MODU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 32;
		}
		break;
	case 8565: /* RSHI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: RSHI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 33;
		}
		break;
	case 8566: /* RSHU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: RSHU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 34;
		}
		break;
	case 8581: /* BANDI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: BANDI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 35;
		}
		break;
	case 8582: /* BANDU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: BANDU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 36;
		}
		break;
	case 8597: /* BCOMI8 */
		_label(LEFT_CHILD(a));
		/* long_var: BCOMI8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 37;
		}
		break;
	case 8598: /* BCOMU8 */
		_label(LEFT_CHILD(a));
		/* long_var: BCOMU8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 38;
		}
		break;
	case 8613: /* BORI8 */
		_label(LEFT_CHILD(a));
		/* long_var: BORI8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 39;
		}
		break;
	case 8614: /* BORU8 */
		_label(LEFT_CHILD(a));
		/* long_var: BORU8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 40;
		}
		break;
	case 8629: /* BXORI8 */
		_label(LEFT_CHILD(a));
		/* long_var: BXORI8(long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 41;
		}
		break;
	case 8630: /* BXORU8 */
		break;
	case 8641: /* DIVF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* double_var: DIVF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 12;
		}
		break;
	case 8645: /* DIVI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: DIVI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 42;
		}
		break;
	case 8646: /* DIVU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: DIVU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 43;
		}
		break;
	case 8657: /* MULF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* double_var: MULF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_double_var_NT]) {
			p->cost[_double_var_NT] = c + 0;
			p->rule._double_var = 13;
		}
		break;
	case 8661: /* MULI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: MULI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 44;
		}
		break;
	case 8662: /* MULU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* long_var: MULU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_long_var_NT]) {
			p->cost[_long_var_NT] = c + 0;
			p->rule._long_var = 45;
		}
		break;
	case 8673: /* EQF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 31;
		}
		break;
	case 8677: /* EQI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 33;
		}
		break;
	case 8678: /* EQU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: EQU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 35;
		}
		break;
	case 8689: /* GEF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 37;
		}
		break;
	case 8693: /* GEI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 39;
		}
		break;
	case 8694: /* GEU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GEU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 41;
		}
		break;
	case 8705: /* GTF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 43;
		}
		break;
	case 8709: /* GTI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 45;
		}
		break;
	case 8710: /* GTU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: GTU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 47;
		}
		break;
	case 8721: /* LEF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 49;
		}
		break;
	case 8725: /* LEI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 51;
		}
		break;
	case 8726: /* LEU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LEU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 53;
		}
		break;
	case 8737: /* LTF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 55;
		}
		break;
	case 8741: /* LTI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 57;
		}
		break;
	case 8742: /* LTU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: LTU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 59;
		}
		break;
	case 8753: /* NEF8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEF8(double_var,double_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_double_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_double_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 61;
		}
		break;
	case 8757: /* NEI8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEI8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 63;
		}
		break;
	case 8758: /* NEU8 */
		_label(LEFT_CHILD(a));
		_label(RIGHT_CHILD(a));
		/* stmt: NEU8(long_var,long_var) */
		c = ((struct _state *)(LEFT_CHILD(a)->x.state))->cost[_long_var_NT] + ((struct _state *)(RIGHT_CHILD(a)->x.state))->cost[_long_var_NT] + 0;
		if (c + 0 < p->cost[_stmt_NT]) {
			p->cost[_stmt_NT] = c + 0;
			p->rule._stmt = 65;
		}
		break;
	default:
		fatal("_label", "Bad terminal %d\n", OP_LABEL(a));
	}
}

static void _kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]) {
	if (!p)
		fatal("_kids", "Null tree\n", 0);
	if (!kids)
		fatal("_kids", "Null kids\n", 0);
	switch (eruleno) {
	case 230: /* stmt: LABELV */
	case 140: /* ptr: ADDRFP8 */
	case 139: /* ptr: ADDRLP8 */
	case 138: /* ptr: ADDRGP8 */
	case 137: /* stmt: RETV */
	case 11: /* ptr: CNSTP8 */
	case 10: /* long_var: CNSTU8 */
	case 9: /* long_var: CNSTI8 */
	case 8: /* int_var: CNSTU4 */
	case 7: /* int_var: CNSTI4 */
	case 6: /* short_var: CNSTU2 */
	case 5: /* short_var: CNSTI2 */
	case 4: /* char_var: CNSTU1 */
	case 3: /* char_var: CNSTI1 */
	case 2: /* double_var: CNSTF8 */
	case 1: /* float_var: CNSTF4 */
		break;
	case 12: /* stmt: ARGB(INDIRB(ptr)) */
		kids[0] = LEFT_CHILD(LEFT_CHILD(p));
		break;
	case 229: /* stmt: JUMPV(ptr) */
	case 180: /* long_var: BXORI8(long_var) */
	case 179: /* int_var: BXORI4(int_var) */
	case 178: /* long_var: BORU8(long_var) */
	case 177: /* int_var: BORU4(int_var) */
	case 176: /* long_var: BORI8(long_var) */
	case 175: /* int_var: BORI4(int_var) */
	case 174: /* long_var: BCOMU8(long_var) */
	case 173: /* int_var: BCOMU4(int_var) */
	case 172: /* long_var: BCOMI8(long_var) */
	case 171: /* int_var: BCOMI4(int_var) */
	case 136: /* stmt: RETU8(long_var) */
	case 135: /* stmt: RETU4(int_var) */
	case 134: /* stmt: RETP8(ptr) */
	case 133: /* stmt: RETI8(long_var) */
	case 132: /* stmt: RETI4(int_var) */
	case 131: /* stmt: RETF8(double_var) */
	case 130: /* stmt: RETF4(float_var) */
	case 129: /* stmt: CALLV(ptr) */
	case 128: /* long_var: CALLU8(ptr) */
	case 127: /* int_var: CALLU4(ptr) */
	case 126: /* ptr: CALLP8(ptr) */
	case 125: /* long_var: CALLI8(ptr) */
	case 124: /* int_var: CALLI4(ptr) */
	case 123: /* double_var: CALLF8(ptr) */
	case 122: /* float_var: CALLF4(ptr) */
	case 121: /* long_var: NEGI8(long_var) */
	case 120: /* int_var: NEGI4(int_var) */
	case 119: /* double_var: NEGF4(double_var) */
	case 118: /* float_var: NEGF4(float_var) */
	case 117: /* long_var: CVUU8(int_var) */
	case 116: /* long_var: CVUU8(short_var) */
	case 115: /* long_var: CVUU8(char_var) */
	case 114: /* int_var: CVUU4(long_var) */
	case 113: /* int_var: CVUU4(short_var) */
	case 112: /* int_var: CVUU4(char_var) */
	case 111: /* short_var: CVUU2(long_var) */
	case 110: /* short_var: CVUU2(int_var) */
	case 109: /* short_var: CVUU2(char_var) */
	case 108: /* char_var: CVUU1(long_var) */
	case 107: /* char_var: CVUU1(int_var) */
	case 106: /* char_var: CVUU1(short_var) */
	case 105: /* ptr: CVUP8(long_var) */
	case 104: /* ptr: CVUP8(int_var) */
	case 103: /* ptr: CVUP8(short_var) */
	case 102: /* ptr: CVUP8(char_var) */
	case 101: /* long_var: CVUI8(int_var) */
	case 100: /* long_var: CVUI8(int_var) */
	case 99: /* long_var: CVUI8(short_var) */
	case 98: /* long_var: CVUI8(char_var) */
	case 97: /* int_var: CVUI4(long_var) */
	case 96: /* int_var: CVUI4(int_var) */
	case 95: /* int_var: CVUI4(short_var) */
	case 94: /* int_var: CVUI4(char_var) */
	case 93: /* short_var: CVUI2(long_var) */
	case 92: /* short_var: CVUI2(int_var) */
	case 91: /* short_var: CVUI2(short_var) */
	case 90: /* short_var: CVUI2(char_var) */
	case 89: /* char_var: CVUI1(long_var) */
	case 88: /* char_var: CVUI1(int_var) */
	case 87: /* char_var: CVUI1(short_var) */
	case 86: /* char_var: CVUI1(char_var) */
	case 85: /* long_var: CVPU8(ptr) */
	case 84: /* long_var: CVIU8(long_var) */
	case 83: /* long_var: CVIU8(int_var) */
	case 82: /* long_var: CVIU8(short_var) */
	case 81: /* long_var: CVIU8(char_var) */
	case 80: /* int_var: CVIU4(long_var) */
	case 79: /* int_var: CVIU4(int_var) */
	case 78: /* int_var: CVIU4(short_var) */
	case 77: /* int_var: CVIU4(char_var) */
	case 76: /* short_var: CVIU2(long_var) */
	case 75: /* short_var: CVIU2(int_var) */
	case 74: /* short_var: CVIU2(short_var) */
	case 73: /* short_var: CVIU2(char_var) */
	case 72: /* char_var: CVIU1(long_var) */
	case 71: /* char_var: CVIU1(int_var) */
	case 70: /* char_var: CVIU1(short_var) */
	case 69: /* char_var: CVIU1(char_var) */
	case 68: /* long_var: CVII8(int_var) */
	case 67: /* long_var: CVII8(short_var) */
	case 66: /* long_var: CVII8(char_var) */
	case 65: /* int_var: CVII4(long_var) */
	case 64: /* int_var: CVII4(short_var) */
	case 63: /* int_var: CVII4(char_var) */
	case 62: /* short_var: CVII2(long_var) */
	case 61: /* short_var: CVII2(int_var) */
	case 60: /* short_var: CVII2(char_var) */
	case 59: /* char_var: CVII1(long_var) */
	case 58: /* char_var: CVII1(int_var) */
	case 57: /* char_var: CVII1(short_var) */
	case 56: /* double_var: CVIF8(long_var) */
	case 55: /* double_var: CVIF8(int_var) */
	case 54: /* double_var: CVIF8(short_var) */
	case 53: /* double_var: CVIF8(char_var) */
	case 52: /* float_var: CVIF4(long_var) */
	case 51: /* float_var: CVIF4(int_var) */
	case 50: /* float_var: CVIF4(short_var) */
	case 49: /* float_var: CVIF4(char_var) */
	case 48: /* long_var: CVFI8(float_var) */
	case 47: /* long_var: CVFI8(double_var) */
	case 46: /* int_var: CVFI4(float_var) */
	case 45: /* int_var: CVFI4(double_var) */
	case 44: /* double_var: CVFF8(float_var) */
	case 43: /* float_var: CVFF4(double_var) */
	case 42: /* ptr: INDIRP8(ptr) */
	case 41: /* long_var: INDIRU8(ptr) */
	case 40: /* long_var: INDIRI8(ptr) */
	case 39: /* int_var: INDIRU4(ptr) */
	case 38: /* int_var: INDIRI4(ptr) */
	case 37: /* short_var: INDIRU2(ptr) */
	case 36: /* short_var: INDIRI2(ptr) */
	case 35: /* char_var: INDIRU1(ptr) */
	case 34: /* char_var: INDIRI1(ptr) */
	case 33: /* double_var: INDIRF8(ptr) */
	case 32: /* float_var: INDIRF4(ptr) */
	case 19: /* stmt: ARGU8(long_var) */
	case 18: /* stmt: ARGU4(int_var) */
	case 17: /* stmt: ARGP8(ptr) */
	case 16: /* stmt: ARGI8(long_var) */
	case 15: /* stmt: ARGI4(int_var) */
	case 14: /* stmt: ARGF8(double_var) */
	case 13: /* stmt: ARGF4(float_var) */
		kids[0] = LEFT_CHILD(p);
		break;
	case 20: /* stmt: ASGNB(ptr,INDIRB(ptr)) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = LEFT_CHILD(RIGHT_CHILD(p));
		break;
	case 228: /* stmt: NEU8(long_var,long_var) */
	case 227: /* stmt: NEU4(int_var,int_var) */
	case 226: /* stmt: NEI8(long_var,long_var) */
	case 225: /* stmt: NEI4(int_var,int_var) */
	case 224: /* stmt: NEF8(double_var,double_var) */
	case 223: /* stmt: NEF4(float_var,float_var) */
	case 222: /* stmt: LTU8(long_var,long_var) */
	case 221: /* stmt: LTU4(int_var,int_var) */
	case 220: /* stmt: LTI8(long_var,long_var) */
	case 219: /* stmt: LTI4(int_var,int_var) */
	case 218: /* stmt: LTF8(double_var,double_var) */
	case 217: /* stmt: LTF4(float_var,float_var) */
	case 216: /* stmt: LEU8(long_var,long_var) */
	case 215: /* stmt: LEU4(int_var,int_var) */
	case 214: /* stmt: LEI8(long_var,long_var) */
	case 213: /* stmt: LEI4(int_var,int_var) */
	case 212: /* stmt: LEF8(double_var,double_var) */
	case 211: /* stmt: LEF4(float_var,float_var) */
	case 210: /* stmt: GTU8(long_var,long_var) */
	case 209: /* stmt: GTU4(int_var,int_var) */
	case 208: /* stmt: GTI8(long_var,long_var) */
	case 207: /* stmt: GTI4(int_var,int_var) */
	case 206: /* stmt: GTF8(double_var,double_var) */
	case 205: /* stmt: GTF4(float_var,float_var) */
	case 204: /* stmt: GEU8(long_var,long_var) */
	case 203: /* stmt: GEU4(int_var,int_var) */
	case 202: /* stmt: GEI8(long_var,long_var) */
	case 201: /* stmt: GEI4(int_var,int_var) */
	case 200: /* stmt: GEF8(double_var,double_var) */
	case 199: /* stmt: GEF4(float_var,float_var) */
	case 198: /* stmt: EQU8(long_var,long_var) */
	case 197: /* stmt: EQU4(int_var,int_var) */
	case 196: /* stmt: EQI8(long_var,long_var) */
	case 195: /* stmt: EQI4(int_var,int_var) */
	case 194: /* stmt: EQF8(double_var,double_var) */
	case 193: /* stmt: EQF4(float_var,float_var) */
	case 192: /* long_var: MULU8(long_var,long_var) */
	case 191: /* int_var: MULU4(int_var,int_var) */
	case 190: /* long_var: MULI8(long_var,long_var) */
	case 189: /* int_var: MULI4(int_var,int_var) */
	case 188: /* double_var: MULF8(double_var,double_var) */
	case 187: /* float_var: MULF4(float_var,float_var) */
	case 186: /* long_var: DIVU8(long_var,long_var) */
	case 185: /* int_var: DIVU4(int_var,int_var) */
	case 184: /* long_var: DIVI8(long_var,long_var) */
	case 183: /* int_var: DIVI4(int_var,int_var) */
	case 182: /* double_var: DIVF8(double_var,double_var) */
	case 181: /* float_var: DIVF4(float_var,float_var) */
	case 170: /* long_var: BANDU8(long_var,long_var) */
	case 169: /* int_var: BANDU4(int_var,int_var) */
	case 168: /* long_var: BANDI8(long_var,long_var) */
	case 167: /* int_var: BANDI4(int_var,int_var) */
	case 166: /* long_var: RSHU8(long_var,long_var) */
	case 165: /* int_var: RSHU4(int_var,int_var) */
	case 164: /* long_var: RSHI8(long_var,long_var) */
	case 163: /* int_var: RSHI4(int_var,int_var) */
	case 162: /* long_var: MODU8(long_var,long_var) */
	case 161: /* int_var: MODU4(int_var,int_var) */
	case 160: /* long_var: MODI8(long_var,long_var) */
	case 159: /* int_var: MODI4(int_var,int_var) */
	case 158: /* long_var: LSHU8(long_var,long_var) */
	case 157: /* int_var: LSHU4(int_var,int_var) */
	case 156: /* long_var: LSHI8(long_var,long_var) */
	case 155: /* int_var: LSHI4(int_var,int_var) */
	case 154: /* long_var: SUBU8(long_var,long_var) */
	case 153: /* int_var: SUBU4(int_var,int_var) */
	case 152: /* ptr: SUBP8(ptr,long_var) */
	case 151: /* long_var: SUBI8(long_var,long_var) */
	case 150: /* int_var: SUBI4(int_var,int_var) */
	case 149: /* double_var: SUBF8(double_var,double_var) */
	case 148: /* float_var: SUBF4(float_var,float_var) */
	case 147: /* long_var: ADDU8(long_var,long_var) */
	case 146: /* int_var: ADDU4(int_var,int_var) */
	case 145: /* ptr: ADDP8(ptr,long_var) */
	case 144: /* long_var: ADDI8(long_var,long_var) */
	case 143: /* int_var: ADDI4(int_var,int_var) */
	case 142: /* double_var: ADDF8(double_var,double_var) */
	case 141: /* float_var: ADDF4(float_var,float_var) */
	case 31: /* stmt: ASGNP8(ptr,ptr) */
	case 30: /* stmt: ASGNU8(ptr,long_var) */
	case 29: /* stmt: ASGNI8(ptr,long_var) */
	case 28: /* stmt: ASGNU4(ptr,int_var) */
	case 27: /* stmt: ASGNI4(ptr,int_var) */
	case 26: /* stmt: ASGNU2(ptr,short_var) */
	case 25: /* stmt: ASGNI2(ptr,short_var) */
	case 24: /* stmt: ASGNU1(ptr,char_var) */
	case 23: /* stmt: ASGNI1(ptr,char_var) */
	case 22: /* stmt: ASGNF8(ptr,double_var) */
	case 21: /* stmt: ASGNF4(ptr,float_var) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = RIGHT_CHILD(p);
		break;
	default:
		fatal("_kids", "Bad rule number %d\n", eruleno);
	}
}

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