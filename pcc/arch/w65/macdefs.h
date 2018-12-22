/*
 * Copyright (c) 2011 Janne Johansson <jj@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Machine-dependent defines for both passes.
 */


/*
 * Convert (multi-)character constant to integer.
 */
#define makecc(val,i)  lastcon = i ? (val<<8)|lastcon : val

#define ARGINIT		64	/* # bits above fp where arguments start */
#define AUTOINIT	0	/* # bits below fp where automatics start */

/*
 * Storage space requirements
 */
#define SZCHAR		8
#define SZBOOL		8
#define SZSHORT		16
#define SZINT		16
#define SZLONG		32
#define SZPOINT(t)	24
#define SZLONGLONG	64
#define SZFLOAT		32
#define SZDOUBLE	64
#define SZLDOUBLE	128

/*
 * Alignment constraints
 */
#define ALCHAR		8
#define ALBOOL		8
#define ALSHORT		16
#define ALINT		16
#define ALLONG		32
#define ALPOINT		32
#define ALLONGLONG	64
#define ALFLOAT		32
#define ALDOUBLE	64
#define ALLDOUBLE	32	/* ???? */
/* #undef ALSTRUCT	m68k struct alignment is member defined */
#define ALSTACK		32
#define ALMAX		64 

/*
 * Min/max values.
 */
#define MIN_CHAR	-128
#define MAX_CHAR	127
#define MAX_UCHAR	255
#define MIN_SHORT	-32768
#define MAX_SHORT	32767
#define MAX_USHORT	65535
#define MIN_INT		MIN_SHORT
#define MAX_INT		MAX_SHORT
#define MAX_UNSIGNED	MAX_USHORT
#define MIN_LONG	(-0x7fffffff-1)
#define MAX_LONG	0x7fffffff
#define MAX_ULONG	0xffffffffU
#define MIN_LONGLONG	0x8000000000000000LL
#define MAX_LONGLONG	0x7fffffffffffffffLL
#define MAX_ULONGLONG	0xffffffffffffffffULL

/* Default char is signed */
#undef	CHAR_UNSIGNED
#define BOOL_TYPE	UCHAR	/* what used to store _Bool */

/*
 * Use large-enough types.
 */
typedef long long CONSZ;
typedef unsigned long long U_CONSZ;
typedef long long OFFSZ;

#define CONFMT	"%lld"		/* format for printing constants */
#define LABFMT	".L%d"		/* format for printing labels */
#define STABLBL ".LL%d"		/* format for stab (debugging) labels */
#ifdef LANG_F77
#define BLANKCOMMON "_BLNK_"
#define MSKIREG	 (M(TYSHORT)|M(TYLONG))
#define TYIREG TYLONG
#define FSZLENG	 FSZLONG
#define ARGREG	AX
#define ARGOFFSET 8
#endif

#define BACKAUTO		/* stack grows negatively for automatics */
#define BACKTEMP		/* stack grows negatively for temporaries */

#undef	FIELDOPS		/* no bit-field instructions */
#define TARGET_ENDIAN TARGET_BE /* big-endian */

#define MYDOTFILE

#undef FINDMOPS /* XXX FIXME */

#define CC_DIV_0	/* division by zero is safe in the compiler */

/* Definitions mostly used in pass2 */

#define BYTEOFF(x)	((x)&03)
#define wdal(k)		(BYTEOFF(k)==0)

#define STOARG(p)
#define STOFARG(p)
#define STOSTARG(p)
#define genfcall(a,b)	gencall(a,b)

/* How many integer registers are needed? (used for stack allocation) */
#define szty(t) ((t) == LDOUBLE ? 3 : \
	(t) == DOUBLE || DEUNSIGN(t) == LONGLONG ? 2 : 1)

/*
 * All registers are given a sequential number to
 * identify it which must match rnames[] in local2.c.
 *
 * The classes used on w65 are:
 *  A - 16-bit  registers (A, X, Y)
 *  B - zero page 
 */

#define AX 0
#define XX 1
#define YX 2

#define ZP1L 3
#define ZP2L 4
#define ZP3L 5
#define ZP4L 6

#define DP 7
#define SP 8


#define MAXREGS 7	/* 7 registers */

#define RSTATUS \
	SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG, \
	SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG, \
	 0, 0, /* dp and sp are ignored here */ 


/* no overlapping registers at all */
#define ROVERLAP \
	{ -1}, \
	{ -1}, \
	{ -1}, \
	{ -1}, \
	{ -1}, \
	{ -1}, \
	{ -1}, \
	{ -1}, 


/* Return a register class based on the type of the node */
#define PCLASS(p) (p->n_type > SHORT ? SBREG : SAREG)

#define NUMCLASS	2	/* highest number of reg classes used */

int COLORMAP(int c, int *r);
#define GCLASS(x) (x < 8 ? CLASSA : x < 16 ? CLASSB : x < 23 ? CLASSC : CLASSD)
#define DECRA(x,y)	(((x) >> (y*6)) & 63)	/* decode encoded regs */
#define ENCRD(x)	(x)		/* Encode dest reg in n_reg */
#define ENCRA1(x)	((x) << 12)	/* A1 */
#define ENCRA2(x)	((x) << 18)	/* A2 */
#define ENCRA(x,y)	((x) << (6+y*6))	/* encode regs in int */

#define RETREG(x)	((x) == FLOAT || (x) == DOUBLE || (x) == LDOUBLE ? ZP2L : \
	(x) == LONGLONG || (x) == ULONGLONG ? ZP2L : (x) > BTMASK ? ZP1L : AX)

#define FPREG	ZP4L	/* frame pointer */
#define STKREG	SP	/* stack pointer */

#define HAVE_WEAKREF
#define TARGET_FLT_EVAL_METHOD	2	/* all as long double */

/*
 * Extended assembler macros.
 */
int targarg(char *w, void *arg);
#define XASM_TARGARG(w, ary) (w[1] == 'b' ? w++, 0 : targarg(w, ary))

/* floating point definitions */
#define USE_IEEEFP_32
#define FLT_PREFIX	IEEEFP_32
#define USE_IEEEFP_64
#define DBL_PREFIX	IEEEFP_64
#define LDBL_PREFIX	IEEEFP_64

