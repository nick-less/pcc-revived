/*	$Id: table.c,v 1.13 2015/10/27 14:48:50 ragge Exp $	*/
/*
 * Copyright (c) 2014 Anders Magnusson (ragge@ludd.ltu.se).
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

#define TLL	TLONGLONG|TULONGLONG
#define TAREG	TINT|TSHORT|TCHAR|TUNSIGNED|TUSHORT|TUCHAR
#define SABREG	SAREG|SBREG
#define TFP	TFLOAT|TDOUBLE|TLDOUBLE

# define ANYSIGNED TINT|TSHORT|TCHAR
# define ANYUSIGNED TUNSIGNED|TUSHORT|TUCHAR
# define ANYFIXED ANYSIGNED|ANYUSIGNED
# define TUWORD TUNSIGNED
# define TSWORD TINT
# define TWORD	TUWORD|TSWORD
#define TANYINT TLL|ANYFIXED
#define	 SHINT	SAREG	/* Any integer */
#define	 ININT	INAREG
#define	 SHFL	SCREG	/* shape for long double */
#define	 INFL	INCREG	/* shape for long double */

/* note to myself, upcasecase in code has special meanings, write small! */
struct optab table[] = {
/* First entry must be an empty entry */
{ -1, FOREFF, SANY, TANY, SANY, TANY, 0, 0, "", },

/* begin with all these casts */

/* pointer to pointer (same reg class) */
{ PCONV,	INBREG,
	SBREG,		TPOINT,
	SBREG,		TANY,
		0,	RLEFT,
		"", },

/* (u)int -> pointer */
{ PCONV,	INBREG,
	SAREG,		TWORD,
	SBREG,		TANY,
		NBREG,	RESC1,
		"	move.l AL,A1 ;int ptr\n", },

/* pointer to int/unsigned */
{ SCONV,	INAREG,
	SBREG,		TPOINT,
	SAREG,		TWORD,
		NAREG,	RESC1,
		"	move.l AL,A1\n", },

/* (u)char -> (u)char */
{ SCONV,	INAREG,
	SAREG,		TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"", },

/* char -> short/ushort */
{ SCONV,	INAREG|INBREG,
	SABREG,		TCHAR,
	SABREG,		TSHORT|TUSHORT,
		0,	RLEFT,
		"	ext.w AL\n", },

/* uchar -> short/ushort */
{ SCONV,	INAREG|INBREG,
	SABREG,		TUCHAR,
	SABREG,		TSHORT|TUSHORT,
		0,	RLEFT,
		"	and.l #255,AL\n", },

/* char -> (u)int */
{ SCONV,	INAREG,
	SAREG,		TCHAR,
	SAREG,		TINT|TUNSIGNED,
		0,	RLEFT,
		"	extb.l AL\n", },

/* uchar -> (u)int */
{ SCONV,	INAREG,
	SAREG,		TUCHAR,
	SAREG,		TINT|TUNSIGNED,
		0,	RLEFT,
		"	and.l #255,AL\n", },

/* char -> (u)longlong */
{ SCONV,	INCREG,
	SAREG|SNAME|SOREG,	TCHAR,
	SCREG,			TLL,
		NCREG,	RESC1,
		"	move.b AL,U1\n	extb.l U1\n"
		"	smi A1\n	extb.l A1\n", },

/* uchar -> (u)longlong */
{ SCONV,	INCREG,
	SAREG|SNAME|SOREG,	TUCHAR,
	SCREG,			TLL,
		NCREG,	RESC1,
		"	move.b AL,U1\n	and.l #255,U1\n	clr.l A1\n", },

/* char -> float/(l)double */
{ SCONV,	INDREG,
	SAREG,		TCHAR,
	SDREG,		TFP,
		NDREG,	RESC1,
		"	fmove.ZL AL,A1\n", },

/* (u)char -> float/(l)double */
{ SCONV,	INDREG,
	SAREG,		TUCHAR,
	SDREG,		TFP,
		NAREG|NDREG,	RESC2,
		"	clr.l A1\n	move.b AL,A1\n	fmove.w A1,A2\n", },

/* (u)short -> (u)char */
{ SCONV,	INAREG,
	SAREG,		TSHORT|TUSHORT,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"", },

/* (u)short -> (u)short */
{ SCONV,	INAREG,
	SAREG,		TSHORT|TUSHORT,
	SAREG,		TSHORT|TUSHORT,
		0,	RLEFT,
		"", },

/* short -> (u)int */
{ SCONV,	INAREG|INBREG,
	SABREG,		TSHORT,
	SABREG,		TINT|TUNSIGNED,
		0,	RLEFT,
		"	ext.l AL\n", },

/* ushort -> (u)int */
{ SCONV,	INAREG|INBREG,
	SABREG,		TUSHORT,
	SABREG,		TINT|TUNSIGNED,
		0,	RLEFT,
		"	and.l #65535,AL\n", },

/* short -> (u)longlong */
{ SCONV,	INCREG,
	SAREG,		TSHORT,
	SCREG,		TLL,
		NCREG,	RESC1,
		"	move AL,U1\n	ext.l U1\n"
		"	smi A1\n	extb.l A1\n", },

/* ushort -> (u)longlong */
{ SCONV,	INCREG,
	SAREG|SNAME|SOREG,	TUSHORT,
	SCREG,			TLL,
		NCREG,	RESC1,
		"	move.l AL,U1\n	and.l #65535,U1\n	clr.l A1\n", },

/* ushort -> float/(l)double */
{ SCONV,	INDREG,
	SAREG|SNAME|SOREG,	TUSHORT,
	SAREG|SDREG,		TFP,
		NAREG|NDREG|NDSL,	RESC2,
		"	move.w AL,A1\n	and.l #65535,A1\n"
		"	fmove.l A1,A2\n", },

/* (u)int -> (u)char */
{ SCONV,	INAREG,
	SAREG,		TWORD,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"	and.l #255,AL\n", },

/* (u)int -> (u)short */
{ SCONV,	INAREG,
	SAREG,		TWORD,
	SAREG,		TSHORT|TUSHORT,
		0,	RLEFT,
		"	and.l #65535,AL\n", },

/* (u)int -> (u)int  - nothing */
{ SCONV,	INAREG,
	SAREG,		TWORD,
	SAREG,		TWORD,
		0,	RLEFT,
		"", },

/* int -> (u)longlong */
{ SCONV,	INCREG,
	SAREG|SOREG|SNAME,	TINT,
	SCREG,			TLL,
		NCREG,	RESC1,
		"	move.l AL,U1\n	smi A1\n	extb.l A1\n", },

/* (u)int -> (u)longlong */
{ SCONV,	INCREG,
	SAREG|SOREG|SNAME,	TUNSIGNED,
	SCREG,			TLL,
		NCREG,	RESC1,
		"	move.l AL,U1\n	clr.l A1\n", },



/* (u)longlong -> (u)char/(u)short/(u)int */
{ SCONV,	INAREG,
	SCREG|SOREG|SNAME,	TLL,
	SAREG,			TAREG,
		NAREG,	RESC1,
		"	movl UL,A1\n", },

/* (u)longlong to (u)longlong */
{ SCONV,	INCREG,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RLEFT,
		"", },


/* assignment */

{ ASSIGN,	FOREFF,
	SAREG|SNAME|SOREG,	TAREG,
	SAREG|SNAME|SOREG,	TAREG,
		0,	0,
		"	move.ZA AR,AL; (AR/AL)0\n", },

{ ASSIGN,	FOREFF,
	SBREG|SNAME|SOREG,	TPOINT,
	SBREG|SNAME|SOREG,	TPOINT,
		0,	0,
		"	move.l  AR,AL ; 11\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG|SNAME|SOREG,	TAREG,
	SAREG,			TAREG,
		0,	RDEST,
		"	move.ZA AR,AL; 2\n", },

{ ASSIGN,	FOREFF|INBREG,
	SBREG|SNAME|SOREG,	TPOINT,
	SBREG,			TPOINT,
		0,	RDEST,
		"	move.l  AR,AL ;3\n", },


{ ASSIGN,	FOREFF | INAREG,
	SAREG|SBREG,	TANY,
	SAREG|SBREG,	TANY,
		0, RDEST,
		"	fmove.x AR,AL ;4\n", },

/* structure stuff */
{ STASG,	INBREG|FOREFF,
	SOREG|SNAME,	TANY,
	SBREG,		TPTRTO|TANY,
		NSPECIAL,	RDEST,
		"ZQ", },
/* 
 * Simple ops (add, sub, and, or, xor)
 */
/* Address registers may be added to (or subtracted from) */
{ PLUS, FOREFF,
	SBREG|SNAME|SOREG|SCON,		TWORD|TPOINT,
	SAREG,				TWORD,
		0,	0,
		"	add.l AR,AL\n", },

{ PLUS, FOREFF|INBREG,
	SBREG,			TPOINT,
	SAREG|SNAME|SOREG|SCON, TWORD,
		0,	RLEFT|RESCC,
		"	add.l AR,AL\n", },

{ PLUS, FOREFF|INDREG,
	SDREG,		TFP,
	SNAME|SOREG|SCON, TFP,
		0,	RLEFT|RESCC,
		"	fadd.ZA AR,AL\n", },

{ PLUS, FOREFF|INDREG,
	SDREG,	TFP,
	SDREG,	TFP,
		0,	RLEFT|RESCC,
		"	fadd.x AR,AL\n", },

{ PLUS, FOREFF|INCREG|RESCC,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RLEFT|RESCC,
		"	add.l UR,UL\n	addx.l AR,AL\n", },

{ PLUS,		INAREG|FOREFF,
	SBREG|SNAME|SOREG,	TLL|TPOINT,
	SONE,	TANY,
		0,	RLEFT,
		"	incq AL\n", },

{ PLUS,		INAREG|FOREFF,
	SBREG|SNAME|SOREG,	TWORD,
	SONE,	TANY,
		0,	RLEFT,
		"	incl AL\n", },

{ PLUS,		INAREG,
	SBREG,	TLL|TPOINT,
	SCON,	TWORD,
		NAREG|NASL,	RESC1,
		"	leaq CR(AL),A1\n", },


{ PLUS,		INAREG|FOREFF,
	SBREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SONE,	TANY,
		0,	RLEFT,
		"	incw AL\n", },

{ PLUS,		INAREG|FOREFF,
	SBREG|SNAME|SOREG,	TCHAR|TUCHAR,
	SONE,	TANY,
		0,	RLEFT,
		"	incb AL\n", },

// { PLUS, FOREFF | INBREG, SBREG, TANY, SBREG, TANY, RLEFT|RESCC, 0, "aha",},
		

{ MINUS, FOREFF,
	SBREG|SNAME|SOREG|SCON,		TWORD|TPOINT,
	SBREG,				TWORD,
		0,	0,
		"	sub.l AR,AL\n", },

{ MINUS, INAREG,
	SBREG,		TPOINT,
	SBREG,		TPOINT,
		NAREG,	RESC1,
		"	move.l AL,A1\n	sub.l AR,A1\n", },

{ MINUS, FOREFF|INBREG,
	SBREG,				TWORD|TPOINT,
	SABREG|SNAME|SOREG|SCON,	TWORD,
		0,	RLEFT|RESCC,
		"	sub.l AR,AL\n", },

{ MINUS, FOREFF|INDREG,
	SDREG,			TFP,
	SNAME|SOREG|SCON, TFP,
		0,	RLEFT|RESCC,
		"	fsub.ZA AR,AL\n", },

{ MINUS, FOREFF|INDREG,
	SDREG,	TFP,
	SDREG,	TFP,
		0,	RLEFT|RESCC,
		"	fsub.x AR,AL\n", },

{ MINUS, FOREFF|INCREG|RESCC,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RLEFT|RESCC,
		"	sub.l UR,UL\n	subx.l AR,AL\n", },

/* two pointers give a scalar */
{ MINUS,	INAREG|FORCC,
	SBREG|SNAME|SOREG|SCON, TPOINT,
	SBREG|SNAME|SOREG|SCON, TPOINT,
		NAREG,	RESC1|RESCC,
		"	move.l AL,A1\n	sub.l AR,A1\n", },

/* Hack to allow for opsimp later down */
/* Fortunately xor is not that common */
{ ER,	FOREFF|INAREG,
	SAREG,				TAREG,
	SNAME|SOREG|SCON,		TAREG,
		NAREG,	RLEFT|RESCC,
		"	move.ZA AR,A1\n	eor.ZA A1,AL\n", },

{ ER,	FOREFF|INCREG|FORCC,
	SCREG|SNAME|SOREG|SCON,		TLL,
	SCREG,				TLL,
		0,	RLEFT|RESCC,
		"	eor.l AR,AL\n	eor.l UR,UL\n", },

{ AND,	FOREFF|INCREG|FORCC,
	SCREG,				TLL,
	SCREG|SNAME|SOREG|SCON,		TLL,
		0,	RLEFT|RESCC,
		"	and.l AR,AL\n	and.l UR,UL\n", },

{ OR,	FOREFF|INCREG|FORCC,
	SCREG,				TLL,
	SCREG|SNAME|SOREG|SCON,		TLL,
		0,	RLEFT|RESCC,
		"	or.l AR,AL\n	or.l UR,UL\n", },

{ OPSIMP,	FOREFF|INAREG,
	SAREG,				TAREG,
	SAREG|SNAME|SOREG|SCON,		TAREG,
		0,	RLEFT|RESCC,
		"	Oz.ZA AR,AL\n", },

{ OPSIMP,	FOREFF,
	SAREG|SNAME|SOREG,	TAREG,
	SAREG,			TAREG,
		0,	RLEFT|RESCC,
		"	Oz.ZA AR,AL\n", },

/*
 * Negate a word.
 */

{ UMINUS,	FOREFF|INCREG|FORCC,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RLEFT|RESCC,
		"	neg.l UL\n	negx.l AL\n", },

{ UMINUS,	FOREFF|INAREG|FORCC,
	SAREG,	TAREG,
	SAREG,	TAREG,
		0,	RLEFT|RESCC,
		"	neg.ZA AL\n", },


{ COMPL,	FOREFF|INAREG|FORCC,
	SAREG,	TAREG,
	SAREG,	TAREG,
		0,	RLEFT|RESCC,
		"	not.ZA AL\n", },

{ COMPL,	FOREFF|INCREG,
	SCREG,	TLL,
	SANY,	TANY,
		0,	RLEFT,
		"	not.l AL\n	not.l UL\n", },

/*
 * Shift operators.
 */
{ LS,	INAREG|FOREFF,
	SAREG,	TAREG,
	SAREG,	TAREG,
		0,	RLEFT,
		"	lsl.ZA AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG,	TUNSIGNED|TUSHORT|TUCHAR,
	SAREG,	TAREG,
		0,	RLEFT,
		"	lsr.ZA AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG,	TINT|TSHORT|TCHAR,
	SAREG,	TAREG,
		0,	RLEFT,
		"	asr.ZA AR,AL\n", },

/*
 * Leaf movements
 */


{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG|SCON|SOREG|SNAME, TAREG,
		NAREG|NASL,	RESC1,
		"	stA1 AL ;leafm 1\n", },

{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SBREG|SCON|SOREG|SNAME, TPOINT,
		NBREG|NBSL,	RESC1,
		"	move.l AL,A1 ;leafm 2\n", },



/*
 * Indirection operators.
 */

{ UMUL, INAREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TPOINT|TWORD,
		NAREG|NASL,	RESC1,
		"	move.l AL,A1\n", },

{ UMUL, INBREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TPOINT|TWORD,
		NBREG|NBSL,	RESC1,
		"	move.l AL,A1\n", },


{ UMUL, INAREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TSHORT|TUSHORT,
		NAREG|NASL,	RESC1,
		"	move.w AL,A1\n", },

{ UMUL, INAREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"	move.b AL,A1\n", },


/*
 * DIV/MOD/MUL 
 */
{ DIV,	INAREG,
	SAREG|SNAME|SOREG,	TINT,
	SAREG,			TINT,
		0,	RLEFT,
		"	divs.l AR,AL\n", },

{ DIV,	INAREG,
	SAREG|SNAME|SOREG,	TUNSIGNED,
	SAREG,			TUNSIGNED,
		0,	RLEFT,
		"	divu.l AR,AL\n", },



{ MOD,	INAREG,
	SAREG,			TINT,
	SAREG|SNAME|SOREG,	TINT,
		NAREG*2,	RESC1,
		"mov.l AL,A2\n	divsl.l AR,A1:A2\n", },

{ MOD,	INAREG,
	SAREG,			TUNSIGNED,
	SAREG|SNAME|SOREG,	TUNSIGNED,
		NAREG*2,	RESC1,
		"mov.l AL,A2\n	divul.l AR,A1:A2\n", },

{ MUL,	INAREG,
	SAREG|SNAME|SOREG,	TWORD,
	SAREG,			TWORD,
		0,	RLEFT,
		"	muls.l AR,AL\n"
		 "	phy\n"
		 "	phx\n"
		 "	pha\n"
		 "	jsr mul16\n"
		 "	plx\n"
		 "	ply\n"
		, },


/*
 * Function call nodes.
 * Too many of them.
 */
/* FOREFF both direct and indirect */
{ UCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr CL; 1\n", },

{ CALL,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr CL\nZB;2 ", },

{ UCALL,	FOREFF,
	SBREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr (AL);3\n", },

{ CALL,		FOREFF,
	SBREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	jsr (AL);4\nZB", },

/* small scalar both direct and indirect */
{ UCALL,	INAREG,
	SCON,	TANY,
	SAREG,	TAREG,
		NAREG|NASL,	RESC1,
		"	jsr CL;5\n", },

{ CALL,		INAREG,
	SCON,	TANY,
	SAREG,	TAREG,
		NAREG|NASL,	RESC1,
		"	jsr CL;6\nZB", },

{ UCALL,	INAREG,
	SBREG,	TANY,
	SAREG,	TAREG,
		NAREG|NASL,	RESC1,
		"	jsr (AL);7\n", },

{ CALL,		INAREG,
	SBREG,	TANY,
	SAREG,	TAREG,
		NAREG|NASL,	RESC1,
		"	jsr (AL);8\nZB", },

/* long long both direct and indirect */
{ UCALL,	INCREG,
	SCON,	TANY,
	SCREG,	TLL,
		NCREG|NCSL,	RESC1,
		"	jsr CL;9\n", },

{ CALL,		INCREG,
	SCON,	TANY,
	SCREG,	TLL,
		NCREG|NCSL,	RESC1,
		"	jsr CL;10\nZB", },

{ UCALL,	INCREG,
	SBREG,	TANY,
	SCREG,	TLL,
		NCREG|NCSL,	RESC1,
		"	jsr (AL);11\n", },

{ CALL,		INCREG,
	SBREG,	TANY,
	SCREG,	TLL,
		NCREG|NCSL,	RESC1,
		"	jsr (AL);12\nZB", },

/* floats both direct and indirect */
{ UCALL,	INDREG,
	SCON,	TANY,
	SDREG,	TFP,
		NDREG|NDSL,	RESC1,
		"	jsr CL\n", },

{ CALL,		INDREG,
	SCON,	TANY,
	SDREG,	TFP,
		NDREG|NDSL,	RESC1,
		"	jsr CL\nZB", },

{ UCALL,	INDREG,
	SBREG,	TANY,
	SDREG,	TFP,
		NDREG|NDSL,	RESC1,
		"	jsr (AL)\n", },

{ CALL,		INDREG,
	SBREG,	TANY,
	SDREG,	TFP,
		NDREG|NDSL,	RESC1,
		"	jsr (AL)\nZB", },

/* pointers both direct and indirect */
{ UCALL,	INBREG,
	SCON,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"	jsr CL\n", },

{ CALL,		INBREG,
	SCON,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"	jsr CL\nZB", },

{ UCALL,	INBREG,
	SBREG,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"	jsr (AL)\n", },

{ CALL,		INBREG,
	SBREG,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"	jsr (AL)\nZB", },


/* struct return both direct and indirect */
{ USTCALL,	INBREG|FOREFF,
	SCON,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"ZP	jsr CL\n", },

{ STCALL,	INBREG|FOREFF,
	SCON,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"ZP	jsr CL\nZB", },

{ USTCALL,	INBREG|FOREFF,
	SBREG,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"ZP	jsr (AL)\n", },

{ STCALL,		INBREG|FOREFF,
	SBREG,	TANY,
	SBREG,	TWORD|TPOINT,
		NBREG|NBSL,	RESC1,
		"ZP	jsr (AL)\nZB", },


/*
 * Arguments to functions.
 */
{ FUNARG,	FOREFF,
	SAREG|SOREG|SNAME|SCON, TINT|TUNSIGNED,
	SANY,			TANY,
		0,	RNULL,
		"	move.l AL,-(%sp)\n", },

{ FUNARG,	FOREFF,
	SCREG|SOREG|SNAME|SCON, TLL,
	SANY,			TANY,
		0,	RNULL,
		"	move.l UL,-(%sp)\n	move.l AL,-(%sp)\n", },

{ FUNARG,	FOREFF,
	SCON,	TPOINT,
	SANY,	TANY,
		0,	RNULL,
		"	pea CL\n", },

{ FUNARG,	FOREFF,
	SCON|SABREG|SNAME,	TWORD|TPOINT,
	SANY,			TWORD|TPOINT,
		0,	RNULL,
		"	move.l AL,-(%sp)\n", },

{ FUNARG,	FOREFF,
	SOREG,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"	move.l AL,-(%sp)\n", },

{ FUNARG,	FOREFF,
	SDREG,	TFP,
	SANY,	TFP,
		0,	RNULL,
		"	fmove.ZA AL,-(%sp)\n", },

{ STARG,	FOREFF,
	SBREG,	TPTRTO|TSTRUCT,
	SANY,	TSTRUCT,
		NAREG|NBREG,	RNULL,
		"ZS", },

/*
 * Logical/branching operators
 */

/* Comparisions, take care of everything */
#if 0
{ OPLOG,	FORCC,
	SHLL|SOREG|SNAME,	TLL,
	SHLL,			TLL,
		0,	0,
		"ZD", },
#endif

{ OPLOG,	INCREG|FORCC,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RESCC|RLEFT, /* trash left nodes */
		"	sub.l UR,UL\n	subx.l AR,AL\n", },

{ OPLOG,	FORCC,
	SAREG,			TWORD,
	SCON|SAREG|SOREG|SNAME, TWORD,
		0,	RESCC,
		"	cmp.l AR,AL\n", },

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
{ GOTO,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp LL\n", },

#if defined(GCC_COMPAT) || defined(LANG_F77)
{ GOTO,		FOREFF,
	SBREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp (AL)\n", },
#endif

# define DF(x) FORREW,SANY,TANY,SANY,TANY,REWRITE,x,#x

{ UMUL, DF( UMUL ), },

{ ASSIGN, DF(ASSIGN), },

{ STASG, DF(STASG), },

{ FLD, DF(FLD), },

{ OPLEAF, DF(NAME), },

/* { INIT, DF(INIT), }, */

{ OPUNARY, DF(UMINUS), },

{ OPANY, DF(BITYPE), },

{ FREE, FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	"help; I'm in trouble\n" },
};

int tablesize = sizeof(table)/sizeof(table[0]);
