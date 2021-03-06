/*	$Id: macdefs.h,v 1.8 2017/02/16 18:55:31 ragge Exp $	*/
/*
 * Copyright (c) 2003 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Machine-dependent defines for both passes.
 */
#define w65_wdc 1
/*
 * Convert (multi-)character constant to integer.
 */
#define makecc(val,i)	lastcon = val;

#define ARGINIT     16      /* # bits above fp where arguments start */
#define AUTOINIT	0	/* # bits below fp where automatics start */

/*
 * Storage space requirements
 */
#define SZCHAR         8
#define SZBOOL         8
#define SZSHORT        16
#define SZINT          16
#define SZLONG         32
#define SZLONGLONG     64
#define SZFLOAT        32
#define SZDOUBLE       64
#define SZLDOUBLE      128
#define SZPOINT(t)     32

/*
 * Alignment constraints
 */
#define ALCHAR         8
#define ALBOOL         8
#define ALSHORT        8
#define ALINT          8
#define ALLONG         8
#define ALPOINT        8
#define ALLONGLONG     8
#define ALFLOAT        8
#define ALDOUBLE       8
#define ALLDOUBLE      8      /* ???? */
/* #undef ALSTRUCT     m68k struct alignment is member defined */
#define ALSTACK        8
#define ALMAX          8 

/*
 * Min/max values.
 */
#define MIN_CHAR       -128
#define MAX_CHAR       127
#define MAX_UCHAR      255
#define MIN_SHORT      -32768
#define MAX_SHORT      32767
#define MAX_USHORT     65535
#define MIN_INT        MIN_SHORT
#define MAX_INT        MAX_SHORT
#define MAX_UNSIGNED   MAX_USHORT
#define MIN_LONG       (-0x7fffffff-1)
#define MAX_LONG       0x7fffffff
#define MAX_ULONG      0xffffffffU
#define MIN_LONGLONG   0x8000000000000000LL
#define MAX_LONGLONG   0x7fffffffffffffffLL
#define MAX_ULONGLONG  0xffffffffffffffffULL

/* Default char is signed */
#undef CHAR_UNSIGNED
#define BOOL_TYPE      UCHAR   /* what used to store _Bool */
/*
 * Use large-enough types.
 */
typedef	long long CONSZ;
typedef	unsigned long long U_CONSZ;
typedef long long OFFSZ;

#define CONFMT	"%llo"		/* format for printing constants */
#define LABFMT	"L%d"		/* format for printing labels */

#define BACKAUTO 		/* stack grows negatively for automatics */
#define BACKTEMP 		/* stack grows negatively for temporaries */

#undef	FIELDOPS		/* no bit-field instructions */
#define TARGET_ENDIAN TARGET_LE

#define FINDMOPS	/* has instructions that modifies memory  */
#define	CC_DIV_0	/* division by zero is safe in the compiler */
#define BYTEOFF(x)	((x)&01)

#define	MYDOTFILE
#define	printdotfile(x)

#define	szty(t)	(((t) == DOUBLE || (t) == FLOAT || \
	(t) == LONGLONG || (t) == ULONGLONG) ? 2 : (t) == LDOUBLE ? 3 : 1)

/*
 * pdp7 has only one register.
 * We emulate 7 memory positions to make the compiler happier.
 */
#define	AC	000	/* Scratch and return register */
#define	POS1	001
#define	POS2	002
#define	POS3	003
#define	POS4	004
#define	POS5	005
#define	POS6	006
#define	POS7	007

#define	FP	010
#define	SP	011

#define	MAXREGS	012

#define	RSTATUS	SAREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG, \
		SBREG|TEMPREG, SBREG|TEMPREG, SBREG|PERMREG, SBREG|PERMREG, \
		0, 0

#define	ROVERLAP { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, \
		{ -1 }, { -1 }, { -1 },


/* Return a register class based on the type of the node */
// #define PCLASS(p) (((p->n_op == REG) || (p->n_op == TEMP)) && regno(p) > 7 ? 0 : \
// 	(((p->n_op == REG) || (p->n_op == TEMP)) && regno(p)) ? SBREG : SAREG)

//#define PCLASS(p) (p->n_op == REG && regno(p) > 7 ? 0 : \
//	(p->n_op == REG && regno(p)) ? SBREG : SAREG)

/* Return a register class based on the type of the node */
#define PCLASS(p) (p->n_type > UNSIGNED  ? SBREG : SAREG)

//#define PCLASS(p) (p->n_op == REG && regno(p) > 7 ? 0 : \
//	(p->n_op == REG && regno(p)) ? SBREG : SAREG)


#define	NUMCLASS 	2	/* highest number of reg classes used */

int COLORMAP(int c, int *r);
#define	GCLASS(x)	((x) == 0 ? CLASSA : CLASSB)
#define DECRA(x,y)	(((x) >> (y*6)) & 63)	/* decode encoded regs */
#define	ENCRD(x)	(x)		/* Encode dest reg in n_reg */
#define ENCRA1(x)	((x) << 6)	/* A1 */
#define ENCRA2(x)	((x) << 12)	/* A2 */
#define ENCRA(x,y)	((x) << (6+y*6))	/* encode regs in int */
#define	RETREG(x)	AC



/* XXX - to die */
#define FPREG	FP	/* frame pointer */
#define STKREG	SP	/* stack pointer */

#define	SLDFPSP		(MAXSPECIAL+1)	/* load fp or sp */


/* floating point definitions */

#define	FDFLOAT
#define	DEFAULT_FPI_DEFS { &fpi_ffloat, &fpi_dfloat, &fpi_dfloat }