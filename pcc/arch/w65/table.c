/*	$Id: table.c,v 1.12 2017/02/16 18:55:31 ragge Exp $	*/
/*
 * Copyright (c) 2017 Anders Magnusson (ragge@ludd.luth.se).
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


# include "pass2.h"

#define TLL TLONGLONG|TULONGLONG
#define TFP	TFLOAT|TDOUBLE|TLDOUBLE

#define ANYSIGNED TINT|TLONG|TSHORT|TCHAR
#define ANYUSIGNED TUNSIGNED|TULONG|TUSHORT|TUCHAR
#define ANYFIXED ANYSIGNED|ANYUSIGNED
#define TUWORD TUNSIGNED
#define TSWORD TINT
#define TWORD TUWORD|TSWORD
#define	 SHINT	SAREG	/* short and int */
#define	 ININT	INAREG
#define	 SHCH	SBREG	/* shape for char */
#define	 INCH	INBREG
#define	 SHLL	SCREG	/* shape for long long */
#define	 INLL	INCREG
#define	 SHFL	SDREG	/* shape for float/double */
#define	 INFL	INDREG	/* shape for float/double */

#define TAREG	TINT|TSHORT|TCHAR|TUNSIGNED|TUSHORT|TUCHAR


#define DF(x) FORREW,SANY,TANY,SANY,TANY,REWRITE,x,"DF"

struct optab table[] = {
/* First entry must be an empty entry */
{ -1, FOREFF, SANY, TANY, SANY, TANY, 0, 0, "tzztr", },

/* PCONVs are usually not necessary */
{ PCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		" pconv", },

/* assignment */
{ ASSIGN,	FOREFF,
	SNAME,	TWORD|TINT|TPOINT,
	SZERO,	TANY,
		0,	0,
		"	stz AL\n", },

{ ASSIGN,	INAREG,
	SAREG,	TCHAR|TUCHAR|TWORD|TPOINT,
	SAREG|SNAME|SOREG|SCON,	TCHAR|TUCHAR|TWORD|TINT|TPOINT,
		0,	RDEST,
		"	lac AR\n	dac AL\n", },

{ ASSIGN,	INAREG,
	SAREG,	TCHAR|TUCHAR|TWORD|TPOINT,
	SAREG|SNAME|SOREG|SCON,	TCHAR|TUCHAR|TWORD|TPOINT,
		0,	RDEST,
		"	lac AR\n	dac AL\n", },


{ ASSIGN,	INCREG,
	SCREG|SNAME|SOREG,	TLL,
	SCREG,			TLL,
		0,	RDEST,
		"	move.l AR,AL\n"
		"	move.l UR,UL\n", },

{ ASSIGN,	FOREFF,
	SAREG,	TAREG,
	SNAME|SOREG,	TAREG,
		0,	RDEST,
		"	sta AR ;0\n", },

{ ASSIGN,	FOREFF,
	SBREG|SNAME|SOREG,	TPOINT,
	SBREG|SNAME|SOREG,	TPOINT,
		0,	RDEST,
		"	move.l AR,AL ;1\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG|SNAME|SOREG,	TAREG,
	SAREG,			TAREG,
		0,	RDEST,
		"	sta AL ;2\n", },

{ ASSIGN,	SANY,
	SAREG|SNAME|SOREG|SCON,	TCHAR|TUCHAR|TWORD|TINT,
	SAREG,			TCHAR|TUCHAR|TWORD|TINT,
		0,	RDEST,
		"	move.l AR,AL ;3\n", },


/** leaf movements */

{ OPLTYPE,	INCREG,
	SANY,			TANY,
	SCREG|SCON|SOREG|SNAME,	TLL,
		NCREG|NCSL,	RESC1,
		"	move.l AL,A1\n	move.l UL,U1\n", },

{ OPLTYPE,	INAREG,
	SAREG,	TINT,
	SAREG|SCON|SOREG|SNAME, TAREG,
		NAREG|NASL,	RESC1,
		"	lda AL   ;51\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG|SCON|SOREG|SNAME, TAREG,
		NAREG|NASL,	RESC1,
		"	move.ZA AL,A1; 52\n", },

{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SBREG|SCON|SOREG|SNAME, TPOINT,
		NBREG|NBSL,	RESC1,
		"	move.l AL,A1 ;6\n", },


{ MUL,	INAREG,
	SAREG,				TWORD,
	SAREG|SNAME|SOREG|SCON,		TWORD,
		0,	RLEFT,
		"	imull AR,AL\n", },

/*
 * Indirection operators.
 */
{ UMUL,	INBREG,
	SANY,	TANY,
	SAREG,	TPOINT,
		NAREG|NBREG,	RESC2,
		"	lac AL i ;0\n	dac A2\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SNAME,	TPTRTO|TCHAR|TUCHAR|TINT|TUNSIGNED,
		NAREG|NASL,	RESC1,
		"	lac AL i;1\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"	lac AL i;2\n", },

/* fetch byte based on byte pointer */
{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG|STARREG,	TANY,
		NAREG,	RESC1,
		"	lda AR ;3\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TUCHAR,
		0,	RLEFT,
		"	jms lbyt\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SNAME,	TANY,
		NAREG|NASL,	RESC1,
		"	lac AL\n	jms lbyt\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TINT|TUNSIGNED|TPOINT,
		NAREG|NASL,	RESC1,
		"	lac AL i\n", },

{ UMUL,	INAREG,
	SANY,		TANY,
	SAREG,	TINT|TUNSIGNED|TPOINT,
		NAREG|NASL|NBREG,	RESC1,
		"	dac A2\n	lac A2 i\n", },

/******/


{ PLUS,		INAREG|FOREFF,
	SAREG,	TCHAR|TUCHAR|TWORD|TPOINT,
	SAREG|SNAME|SOREG|SCON,	TCHAR|TUCHAR|TWORD|TPOINT,
		0,	RLEFT,
		"	ZE\n", },


{ MINUS,	INAREG|FOREFF,
	SAREG,  TWORD|TPOINT,
	SAREG|SNAME|SOREG|SCON,	TCHAR|TUCHAR|TWORD|TPOINT,
	      0,      RLEFT,
		"	ZE\n", },

/*
 * Logical/branching operators
 */

{ OPLOG,	INCREG|FORCC,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RESCC|RLEFT, /* trash left nodes */
		"	sub.l UR,UL\n	subx.l AR,AL\n", },

{ OPLOG,	FORCC,
	SAREG,			TWORD,
	SCON|SAREG|SOREG|SNAME, TWORD,
		0,	RESCC,
		"	cmp AR\n", },

{ OPLOG,	FORCC,
	SBREG,			TPOINT,
	SCON|SBREG|SOREG|SNAME, TPOINT,
		0,	RESCC,
		"	cmp.l AR,AL\n", },

/* jumps below emitted in zzzcode */
{ OPLOG,	FORCC,
	SDREG,			TFP,
	SCON|SOREG|SNAME,	TFP,
		0,	0,
		"	fcmp.ZL AR,AL\n	ZF", },

{ OPLOG,	FORCC,
	SDREG,	TFP,
	SDREG,	TFP,
		0,	0,
		"	fcmp.x AR,AL\n	ZF", },


/*
 * Jumps.
 */
{ GOTO, 	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	bra LL\n", },

/*
 * Arguments to functions.
 */


{ FUNARG,	FOREFF,
	SCON,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"	pea CL\n", },
		

{ FUNARG,	FOREFF,
	SAREG,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"	pha\n", },

{ FUNARG,	FOREFF,
	SAREG,	TUCHAR,
	SANY,	TANY,
		0,	RNULL,
		"	dac ZA\n", },

{ STARG,	FOREFF,
	SAREG,	TPTRTO|TSTRUCT,
	SANY,	TSTRUCT,
		NSPECIAL,	0,
		"ZF", },

/*
 * Subroutine calls.
 */

{ CALL,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr CL;0\nZC\n", },

{ CALL,	INAREG,
	SCON,	TANY,
	SAREG,	TSHORT|TUSHORT|TWORD|TPOINT,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	jsr CL;1\nZC\n", },

{ CALL,		FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr CL;2\nZC\n", },


{ STASG, DF(STASG), },

{ FLD, DF(FLD), },

{ OPLEAF, DF(NAME), },

/* { INIT, DF(INIT), }, */

{ OPUNARY, DF(UMINUS), },

{ OPANY, DF(BITYPE), },

{ FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	"help; I'm in trouble\n" },
};


/** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! OLD SHIT */

struct optab table2[] = {
/* First entry must be an empty entry */
{ -1, FOREFF, SANY, TANY, SANY, TANY, 0, 0, "", },

/* PCONVs are usually not necessary */
{ PCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"", },

/*
 * A bunch conversions of integral<->integral types
 * There are lots of them, first in table conversions to itself
 * and then conversions from each type to the others.
 */

/* itself to itself, including pointers */

/* convert pointers to int. */
{ SCONV,	ININT,
	SHINT,	TPOINT|TWORD,
	SANY,	TWORD,
		0,	RLEFT,
		"", },

/* convert pointers to pointers. */
{ SCONV,	ININT,
	SHINT,	TPOINT,
	SANY,	TPOINT,
		0,	RLEFT,
		"", },

/* char to something */

/* convert signed char to int (or pointer). */
{ SCONV,	INAREG,
	SAREG,	TCHAR,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"	sign extend AL\n", },

/* convert unsigned char to (u)int. */
{ SCONV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TWORD,
		0,	RLEFT,
		"", },

/* short to something */

/* convert (u)short to (u)short. */
{ SCONV,	INAREG,
	SAREG,	TSHORT|TUSHORT,
	SAREG,	TSHORT|TUSHORT,
		0,	RLEFT,
		"", },

/* convert short (in memory) to char */
{ SCONV,	INCH,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SHCH,		TCHAR|TUCHAR,
		NBREG|NBSL,	RESC1,
		"	movb AL,A1\n", },

/* convert short (in reg) to char. */
{ SCONV,	INCH,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL|NBREG|NBSL,	RESC1,
		"ZM", },

/* convert short to (u)int. */
{ SCONV,	ININT,
	SAREG|SOREG|SNAME,	TSHORT,
	SAREG,	TWORD,
		NASL|NAREG,	RESC1,
		"	movswl AL,A1\n", },

/* convert unsigned short to (u)int. */
{ SCONV,	ININT,
	SAREG|SOREG|SNAME,	TUSHORT,
	SAREG,	TWORD,
		NASL|NAREG,	RESC1,
		"	movzwl AL,A1\n", },

/* convert short to (u)long long */
{ SCONV,	INLL,
	SAREG|SOREG|SNAME,	TSHORT,
	SHLL,			TLL,
		NSPECIAL|NCREG|NCSL,	RESC1,
		"	movswl AL,%eax\n	cltd\n", },

/* convert unsigned short to (u)long long */
{ SCONV,	INLL,
	SAREG|SOREG|SNAME,	TUSHORT,
	SHLL,			TLL,
		NCREG|NCSL,	RESC1,
		"	movzwl AL,A1\n	xorl U1,U1\n", },

/* int to something */

/* convert int to char. This is done when register is loaded */
{ SCONV,	INCH,
	SAREG,	TWORD|TPOINT,
	SANY,	TCHAR|TUCHAR,
		NSPECIAL|NBREG|NBSL,	RESC1,
		"ZM", },

/* convert int to uchar. */
{ SCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SANY,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"	and ZF\n", },

/* convert int to short. Nothing to do */
{ SCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SANY,	TSHORT|TUSHORT,
		0,	RLEFT,
		"", },

/* convert signed int to (u)long long */
{ SCONV,	INLL,
	SHINT,	TSWORD,
	SHLL,	TLL,
		NSPECIAL|NCREG|NCSL,	RESC1,
		"	cltd\n", },

/* convert unsigned int to (u)long long */
{ SCONV,	INLL,
	SHINT|SOREG|SNAME,	TUWORD|TPOINT,
	SHLL,	TLL,
		NCSL|NCREG,	RESC1,
		"	movl AL,A1\n	xorl U1,U1\n", },

/* long long to something */

/* convert (u)long long to (u)char (mem->reg) */
{ SCONV,	INCH,
	SOREG|SNAME,	TLL,
	SANY,	TCHAR|TUCHAR,
		NBREG|NBSL,	RESC1,
		"	movb AL,A1\n", },

/* convert (u)long long to (u)char (reg->reg, hopefully nothing) */
{ SCONV,	INCH,
	SHLL,	TLL,
	SANY,	TCHAR|TUCHAR,
		NBREG|NBSL|NTEMP,	RESC1,
		"ZS", },

/* convert (u)long long to (u)short (mem->reg) */
{ SCONV,	INAREG,
	SOREG|SNAME,	TLL,
	SAREG,	TSHORT|TUSHORT,
		NAREG|NASL,	RESC1,
		"	movw AL,A1\n", },

/* convert (u)long long to (u)short (reg->reg, hopefully nothing) */
{ SCONV,	INAREG,
	SHLL|SOREG|SNAME,	TLL,
	SAREG,	TSHORT|TUSHORT,
		NAREG|NASL|NTEMP,	RESC1,
		"ZS", },

/* convert long long to int (mem->reg) */
{ SCONV,	INAREG,
	SOREG|SNAME,	TLL,
	SAREG,	TWORD|TPOINT,
		NAREG|NASL,	RESC1,
		"	movl AL,A1\n", },

/* slut sconv */

/*
 * Subroutine calls.
 */

{ UCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jms CL\nZC", },

{ CALL,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jms CL\nZC", },

{ UCALL,	FOREFF,
	SCON,	TANY,
	SAREG,	TWORD|TPOINT,
		0,	0,
		"	jms CL\nZC", },

{ CALL,	INAREG,
	SCON,	TANY,
	SAREG,	TSHORT|TUSHORT|TWORD|TPOINT,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	jms CL\nZC", },

{ UCALL,	INAREG,
	SCON,	TANY,
	SAREG,	TSHORT|TUSHORT|TWORD|TPOINT,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	jms CL\nZC", },

{ CALL,		FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	dac 20\n	cal i\nZC", },

{ UCALL,	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	dac 20\n	cal i\nZC", },

{ CALL,		INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	dac 20\n	cal i\nZC", },

{ UCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	dac 20\n	cal i\nZC", },

{ STCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		NAREG|NASL|NASR,	0,
		"	jms CL\nZC", },

{ STCALL,	INAREG,
	SCON,	TANY,
	SANY,	TANY,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	jms CL\nZC", },

{ STCALL,	INAREG,
	SNAME|SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL|NASR,	RESC1,	/* should be 0 */
		"	dac 20\n	cal i\nZC", },

/*
 * The next rules handle all binop-style operators.
 */
{ PLUS,		INBREG,
	SCON,	TCHAR|TUCHAR|TWORD|TPOINT,
	SAREG,	TCHAR|TUCHAR|TWORD|TPOINT,
		NBREG,	RESC1,
		"	tad ZL;0 \n	dac A1\n", },

{ PLUS,		INAREG,
	SAREG,		TWORD|TPOINT,
	SNAME|SBREG,	TWORD|TPOINT,
		0,	RLEFT,
		"	tad AR;1 \n", },

{ PLUS,		INBREG,
	SAREG,	TPOINT,
	SCON,	TANY,
		NBREG,	RESC1,
		"	tad ZJ;2\n	dac A1\n", },

/* add constant to memory position referenced by AL */
{ PLUS,		INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SCON,		TANY,
		NBREG,	RLEFT,
		"ZI", },

/* Add name to AC */
{ PLUS,		INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SNAME,	TWORD|TPOINT,
		0,	RLEFT,
		"	tad AR;3\n", },

/* Add constant to AC */
{ PLUS,		INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SCON,	TWORD|TPOINT,
		0,	RLEFT,
		"	tad ZJ;4\n", },

{ MINUS,	INBREG|FOREFF,
	SLDFPSP,	TANY,
	SAREG,		TANY,
	      NBREG,	RESC1,
	      "		tad AL;5\n	dad A1\n", },

{ MINUS,	INAREG|FOREFF,
	SAREG,  TWORD|TPOINT,
	SNAME,  TWORD|TPOINT,
	      0,      RLEFT,
	      "		cma\n	tad AR\n	cma\n", },

{ MINUS,	INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SCON,	TWORD|TPOINT,
	      0,	RLEFT,
	      "		tad ZE;6\n", },

/* Tricky here. Left reg is pointer, must store and use indirect address. */
{ MINUS,	INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SCON,		TANY,
		NBREG,	RLEFT,
		"	dac A1\n"
		"	lac A1 i\n"
		"	tad ZE\n"
		"	dac A1 i\n", },

/*
 * The next rules handle all shift operators.
 */
{ LS,	INAREG|FOREFF,
	SAREG,	TWORD,
	SHCH,	TCHAR|TUCHAR,
		0,	RLEFT,
		"	sall AR,AL\n", },

/* r/m <<= const */
{ LS,	INAREG|FOREFF,
	SAREG,	TWORD,
	SCON,	TANY,
		0,	RLEFT,
		"	clq lls CR\n", },

/* r/m <<= r */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	shlw AR,AL\n", },

/* r/m <<= const */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SCON,	TANY,
		0,	RLEFT,
		"	shlw AR,AL\n", },

{ LS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	salb AR,AL\n", },

{ LS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SCON,			TANY,
		0,	RLEFT,
		"	salb AR,AL\n", },

/* (u)longlong right shift is emulated */
{ RS,	INCREG,
	SCREG,	TLL,
	SHCH,	TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"ZO", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSWORD,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	sarl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSWORD,
	SCON,			TANY,
		0,		RLEFT,
		"	sarl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUWORD,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	shrl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUWORD,
	SCON,			TANY,
		0,		RLEFT,
		"	shrl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	sarw AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT,
	SCON,			TANY,
		0,		RLEFT,
		"	sarw AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUSHORT,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	shrw AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUSHORT,
	SCON,			TANY,
		0,		RLEFT,
		"	shrw AR,AL\n", },

{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	sarb AR,AL\n", },

{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR,
	SCON,			TANY,
		0,		RLEFT,
		"	sarb AR,AL\n", },

{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TUCHAR,
	SHCH,			TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"	shrb AR,AL\n", },

{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TUCHAR,
	SCON,			TANY,
		0,		RLEFT,
		"	shrb AR,AL\n", },

/*
 * The next rules takes care of assignments. "=".
 */
{ ASSIGN,	FOREFF,
	SNAME,	TWORD|TINT|TPOINT,
	SZERO,	TANY,
		0,	0,
		"	dzm AL\n", },

{ ASSIGN,	FOREFF,
	SNAME,	TCHAR|TUCHAR,
	SAREG,	TANY,
		0,	0,
		" zg", },

{ ASSIGN,	FOREFF,
	SNAME,	TCHAR|TUCHAR,
	SZERO,	TANY,
		0,	0,
		"	dzm AL\n", },

{ ASSIGN,	FOREFF,
	SNAME,	TCHAR|TUCHAR,
	SAREG,	TANY,
		0,	0,
		"	dac AL;0\n", },

{ ASSIGN,	FOREFF,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TANY,
		0,	0,
		"	dac AL i;1\n", },

{ ASSIGN,	FOREFF,
	SAREG,	TCHAR|TUCHAR,
	SCON,	TANY,
		0,	0,
		"	jms sbyt\n	CR\n", },

{ ASSIGN,	FOREFF,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TANY,
		0,	0,
		"	sac AL i\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TWORD|TPOINT,
	SAREG,		TWORD|TPOINT,
		0,	RDEST,
		"	dac AL;2\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TWORD|TPOINT,
	SNAME|SBREG,	TWORD|TPOINT,
		NAREG|NASL|NASR,	RDEST,
		"	lac AR\n	dac AL;3\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TWORD|TPOINT,
	SNAME,	TWORD|TPOINT,
		0,	RDEST,
		"	lac AR\n", },

{ ASSIGN,	FOREFF|INAREG,
	SNAME,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RDEST,
		"	dac AL;4\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TWORD|TPOINT,
	SBREG,	TWORD|TPOINT,
		0,	RDEST,
		" zk", },

{ ASSIGN,	FOREFF|INAREG,
	SBREG,	TWORD|TPOINT,
	SNAME,	TWORD|TPOINT,
		NAREG,	RDEST,
		"	lac AR\n	dac AL\n", },

{ STASG,	INAREG|FOREFF,
	SOREG|SNAME,	TANY,
	SAREG,		TPTRTO|TANY,
		NSPECIAL|NAREG,	RDEST,
		"F	movl %esi,A1\nZQF	movl A1,%esi\n", },

/*
 * DIV/MOD/MUL 
 */
/* long long div is emulated */
{ DIV,	INCREG,
	SCREG|SNAME|SOREG|SCON, TLL,
	SCREG|SNAME|SOREG|SCON, TLL,
		NSPECIAL|NCREG|NCSL|NCSR,	RESC1,
		" z0", },

{ DIV,	INAREG,
	SAREG,	TSWORD,
	SCON,	TWORD,
		0,	RLEFT,
		"	cll; idiv; CR; lacq\n", },

{ DIV,		INAREG,
	SAREG,		TWORD,
	SNAME,		TWORD,
		0,      RLEFT,		// XXX, how to rewrite to do the
					// operands in reverse order?
					// I tried RRIGHT and lac AL, no good
		"	lmq\n"
		"	lac AR\n"
		"	dac .+4\n"
		"	lacq\n"
		"	cll; idiv; ..; lacq\n", },

{ MOD,	INAREG,
	SAREG,	TSWORD,
	SCON,	TWORD,
		0,	RLEFT,
		"	cll; idiv; CR\n", },

{ MOD,	INAREG,
	SAREG,			TSWORD,
	SAREG|SNAME|SOREG,	TSWORD,
		NAREG|NSPECIAL,	RESC1,
		"	cltd\n	idivl AR\n", },

{ MOD,	INAREG,
	SAREG,			TWORD|TPOINT,
	SAREG|SNAME|SOREG,	TUWORD|TPOINT,
		NAREG|NSPECIAL,	RESC1,
		"	xorl %edx,%edx\n	divl AR\n", },

{ MOD,	INAREG,
	SAREG,			TUSHORT,
	SAREG|SNAME|SOREG,	TUSHORT,
		NAREG|NSPECIAL,	RESC1,
		"	xorl %edx,%edx\n	divw AR\n", },

{ MOD,	INCH,
	SHCH,			TUCHAR,
	SHCH|SNAME|SOREG,	TUCHAR,
		NBREG|NSPECIAL,	RESC1,
		"	xorb %ah,%ah\n	divb AR\n", },

{ MUL,	INAREG,
	SAREG,		TWORD,
	SNAME,		TWORD,
		0,	RLEFT,
		"	dac .+4\n"
		"	lac AR\n"
		"	cll; mul; ..; lacq\n", },

/*
 * Indirection operators.
 */
{ UMUL,	INBREG,
	SANY,	TANY,
	SAREG,	TPOINT,
		NAREG|NBREG,	RESC2,
		"	lac AL i\n	dac A2\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SNAME,	TPTRTO|TCHAR|TUCHAR|TINT|TUNSIGNED,
		NAREG|NASL,	RESC1,
		"	lac AL i\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"	lac AL i\n", },

/* fetch byte based on byte pointer */
{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TUCHAR,
		 REWRITE, RESC1,
		"	lac AR\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TUCHAR,
		REWRITE,	RLEFT,
		"	jms lbyt\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SNAME,	TANY,
		NAREG|NASL,	RESC1,
		"	lac AL\n	jms lbyt\n", },

{ UMUL,	INAREG,
	SANY,	TANY,
	SAREG,	TINT|TUNSIGNED|TPOINT,
		NAREG|NASL,	RESC1,
		"	lac AL i\n", },

{ UMUL,	INAREG,
	SANY,		TANY,
	SAREG,	TINT|TUNSIGNED|TPOINT,
		NAREG|NASL|NBREG,	RESC1,
		"	dac A2\n	lac A2 i\n", },

/*
 * Logical/branching operators
 */

/* Comparisions, take care of everything */
{ EQ,	FORCC,
	SAREG,	TWORD|TPOINT,
	SZERO,	TWORD|TPOINT,
		0, 	0,
		"	sna\n	jmp LC\n", },

{ NE,	FORCC,
	SAREG,	TWORD|TPOINT,
	SZERO,	TWORD|TPOINT,
		0, 	0,
		"	sza\n	jmp LC\n", },

{ OPLOG,	FORCC,
	SAREG,	TWORD|TPOINT,
	SNAME,	TWORD|TPOINT,
		0, 	RESCC,
		"	cma ; tad AR ; cma\n", },

{ OPLOG,	FORCC,
	SAREG,	TWORD|TPOINT,
	SCON,	TWORD|TPOINT,
		0, 	RESCC,
		"	tad ZE; 11\n", },

{ OPLOG,	FORCC,
	SAREG,	TWORD|TPOINT,
	SZERO,	TWORD|TPOINT,
		0, 	RESCC,
		"", },

{ OPLOG,	FORCC,
	SANY,	TANY,
	SANY,	TANY,
		REWRITE,	0,
		"diediedie!", },

/* AND/OR/ER/NOT */
{ AND,	INAREG|FOREFF,
	SAREG|SOREG|SNAME,	TWORD,
	SCON|SAREG,		TWORD,
		0,	RLEFT,
		"	andl AR,AL\n", },

{ AND,	INCREG|FOREFF,
	SCREG,			TLL,
	SCREG|SOREG|SNAME,	TLL,
		0,	RLEFT,
		"	andl AR,AL\n	andl UR,UL\n", },

{ AND,	INAREG|FOREFF,
	SAREG,			TWORD,
	SAREG|SOREG|SNAME,	TWORD,
		0,	RLEFT,
		"	andl AR,AL\n", },

{ AND,	INAREG|FOREFF,  
	SAREG|SOREG|SNAME,	TSHORT|TUSHORT,
	SCON|SAREG,		TSHORT|TUSHORT,
		0,	RLEFT,
		"	andw AR,AL\n", },

{ AND,	INAREG|FOREFF,  
	SAREG,			TSHORT|TUSHORT,
	SAREG|SOREG|SNAME,	TSHORT|TUSHORT,
		0,	RLEFT,
		"	andw AR,AL\n", },

{ AND,	INBREG|FOREFF,
	SBREG|SOREG|SNAME,	TCHAR|TUCHAR,
	SCON|SBREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"	andb AR,AL\n", },

{ AND,	INBREG|FOREFF,
	SBREG,			TCHAR|TUCHAR,
	SBREG|SOREG|SNAME,	TCHAR|TUCHAR,
		0,	RLEFT,
		"	andb AR,AL\n", },
/* AND/OR/ER/NOT */

/*
 * Jumps.
 */
{ GOTO, 	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp LL\n", },


/*
 * Convert LTYPE to reg.
 */
/* XXX as will store references to byte pointers as word pointers. */
{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON,	TPTRTO|TCHAR|TUCHAR,
		NAREG,	RESC1,
		"	lac ZB\n	rcl\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON,	TCHAR|TUCHAR|TWORD|TPOINT,
		NAREG,	RESC1,
		"	lac ZB\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SNAME,	TWORD|TPOINT,
		NAREG,	RESC1,
		"	lac AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SNAME|SWADD,	TUCHAR,
		NAREG,	RESC1,
		"	lac AL\n	clq lrs 011\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SNAME|SWADD,	TUCHAR,
		NAREG,	RESC1,
		"	lac AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SNAME,	TPTRTO|TCHAR|TUCHAR,
		NAREG,	RESC1,
		"	lac AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SNAME,	TUCHAR,
		NAREG,	RESC1,
		"	lac AL\n	and ZF\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SLDFPSP,	TWORD|TPOINT,
		NAREG,	RESC1,
		"	lac AL\n", },

/*
 * Negate a word.
 */

{ UMINUS,	INCREG|FOREFF,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RLEFT,
		"	negl AL\n	adcl $0,UL\n	negl UL\n", },

{ UMINUS,	INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"	negl AL\n", },

{ UMINUS,	INAREG|FOREFF,
	SAREG,	TSHORT|TUSHORT,
	SAREG,	TSHORT|TUSHORT,
		0,	RLEFT,
		"	negw AL\n", },

{ UMINUS,	INBREG|FOREFF,
	SBREG,	TCHAR|TUCHAR,
	SBREG,	TCHAR|TUCHAR,
		0,	RLEFT,
		"	negb AL\n", },

{ UMINUS,	INFL|FOREFF,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"	fchs\n", },

{ COMPL,	INCREG,
	SCREG,	TLL,
	SANY,	TANY,
		0,	RLEFT,
		"	notl AL\n	notl UL\n", },

{ COMPL,	INAREG,
	SAREG,	TWORD,
	SANY,	TANY,
		0,	RLEFT,
		"	notl AL\n", },

{ COMPL,	INAREG,
	SAREG,	TSHORT|TUSHORT,
	SANY,	TANY,
		0,	RLEFT,
		"	notw AL\n", },

{ COMPL,	INBREG,
	SBREG,	TCHAR|TUCHAR,
	SANY,	TANY,
		0,	RLEFT,
		"	notb AL\n", },

/*
 * Arguments to functions.
 */
{ FUNARG,	FOREFF,
	SAREG,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"	dac ZA\n", },

{ FUNARG,	FOREFF,
	SAREG,	TUCHAR,
	SANY,	TANY,
		0,	RNULL,
		"	dac ZA\n", },

{ STARG,	FOREFF,
	SAREG,	TPTRTO|TSTRUCT,
	SANY,	TSTRUCT,
		NSPECIAL,	0,
		"ZF", },


{ UMUL, DF( UMUL ), },

{ ASSIGN, DF(ASSIGN), },

{ STASG, DF(STASG), },

{ FLD, DF(FLD), },

{ OPLEAF, DF(NAME), },

/* { INIT, DF(INIT), }, */

{ OPUNARY, DF(UMINUS), },

{ OPANY, DF(BITYPE), },

{ FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	"help; I'm in trouble\n" },
};

int tablesize = sizeof(table)/sizeof(table[0]);
