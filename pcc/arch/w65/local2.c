/*	$Id: local2.c,v 1.18 2016/09/26 16:45:42 ragge Exp $	*/
/*
 * Copyright (c) 2014 Anders Magnusson (ragge@ludd.luth.se).
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
# include <ctype.h>
# include <string.h>

static int stkpos;
static int regm, regf, fpsub, nfp;

// char *rnames[] = {
// 	"a", "x", "y", "ds1", "ds2", "ds3", "ds4", "ds5", "ds6", "ds7", "ds8", "dm1", "dm2", "dm3", "dm4", "dl1", "dl2"
// };
char *rnames[] = {
	"AC", "POS1", "POS1", "POS1", "POS1", "POS1", "POS1", "POS1", 
	"FP", "SP",
};


void deflab(int label) {
	printf(LABFMT ":\n", label);
}


void prologue(struct interpass_prolog *ipp) {
	/*
	 * Subtract both space for automatics and permanent regs.
	 * XXX - no struct return yet.
	 */

	fpsub = p2maxautooff;
	if (fpsub >= AUTOINIT/SZCHAR) {
		fpsub -= AUTOINIT/SZCHAR;
	}
	regm = regf = nfp = 0;
	fpsub += 4;


//	printf(";	link.%c %%fp,#%d;;prologue\n", fpsub > 32768 ? 'l' : 'w', -fpsub);
}

void eoftn(struct interpass_prolog *ipp) {
	// FIXME?
	if (ipp->ipp_ip.ip_lbl == 0)
		return; /* no code needs to be generated */

//	printf(";	unlk %% ;;interpass_prolog\n");
	printf("	rts\n");
//	printf(".endproc\n\n");
}

/*
 * Called for an O macro in the table, o is the opcode and
 * f is the argument.  Supposed to write out simple ops.
 *
 * Param given:
 */
void hopcode(int f, int o) {
	char *str;

	switch (o) {
	case AND:
		str = "and";
		break;
	case OR:
		str = "ora";
		break;
	case ER:
		str = "eor";
		break;
	default:
		comperr("hopcode2: %d", o);
		str = 0; /* XXX gcc */
	}
	printf("%s", str);
}

/*
 * Return type size in bytes.  Used by R2REGS, arg 2 to offset().
 */
int tlen(NODE *p) {
	switch(p->n_type) {
		case CHAR:
		case UCHAR:
			return(1);

		case SHORT:
		case USHORT:
			return(SZSHORT/SZCHAR);

		case DOUBLE:
			return(SZDOUBLE/SZCHAR);

		case INT:
		case UNSIGNED:
			return(SZINT/SZCHAR);

		case LONG:
		case ULONG:
		case LONGLONG:
		case ULONGLONG:
			return SZLONGLONG/SZCHAR;

		default:
			if (!ISPTR(p->n_type))
				comperr("tlen type %d not pointer");
			return SZPOINT(p->n_type)/SZCHAR;
		}
}

/**
 * fieldexpand
 * Field ops are found in the table (S, H, M or N), and
		this function may alter the default behaviour of printing
		out the values.  Return 1 if handled.
 * 
 */
int fldexpand(NODE *p, int cookie, char **cp) {
	// comperr("fldexpand %p %d %s");

	return 0;
}

static void starg(NODE *p) {
	int sz = attr_find(p->n_ap, ATTR_P2STRUCT)->iarg(0);
	int subsz = (sz + 3) & ~3;
	int fr, tr, cr;

	fr = regno(getlr(p, 'L')); /* from reg (struct pointer) */
	cr = regno(getlr(p, '1')); /* count reg (number of words) */
	tr = regno(getlr(p, '2')); /* to reg (stack) */

	/* Sub from stack and put in toreg */
	printf("	sub.l #%d,%%sp\n", subsz);
	printf("	move.l %%sp,%s\n", rnames[tr]);

	/* Gen an even copy start */
	if (sz & 1)
		expand(p, INBREG, "	move.b (AL)+,(A2)+\n");
	if (sz & 2)
		expand(p, INBREG, "	move.w (AL)+,(A2)+\n");
	sz -= (sz & 3);
	
	/* if more than 4 words, use loop, otherwise output instructions */
	if (sz > 16) {
		printf("	move.l #%d,%s\n", (sz/4)-1, rnames[cr]);
		expand(p, INBREG, "1:	move.l (AL)+,(A2)+\n");
		expand(p, INBREG, "	dbra A1,1b\n");
	} else {
		if (sz > 12)
			expand(p, INBREG, "	move.l (AL)+,(A2)+\n"), sz -= 4;
		if (sz > 8)
			expand(p, INBREG, "	move.l (AL)+,(A2)+\n"), sz -= 4;
		if (sz > 4)
			expand(p, INBREG, "	move.l (AL)+,(A2)+\n"), sz -= 4;
		if (sz == 4)
			expand(p, INBREG, "	move.l (AL)+,(A2)+\n");
	}
}

/**
  * gets called from expand (code), when Z is encountered
  *
  */
void zzzcode(NODE *p, int c) {
	TWORD t = p->n_type;
	char *s;
	NODE *r,*l;
	unsigned short int ival, pr;

	switch (c) {

	case 'C':  /* remove from stack after subroutine call */
#ifdef notdef
		if (p->n_left->n_flags & FSTDCALL)
			break;
#endif
        // XXX n_qual seems to assume LONG
		pr = p->n_qual / 2;
//		printf("; call flags %x\n", p->n_left->n_flags);
//		if (attr_find(p->n_ap, ATTR_I86_FPPOP))
//			printf("	fstp	st(0)\n");
		if (p->n_op == UCALL) {
			return; /* XXX remove ZC from UCALL */
			}
		if (pr) {
			if (pr == 2) {
				printf("	ply\n	ply\n");
			} else {
				printf("	tsc\n");
				printf("	sec\n");
				printf("	sbc #%d\n", pr);
				printf("	tcs\n");
				//
//				printf("	add sp,#%d\n", pr);
			}	
		}
		break;
	case 'E':	/*  PLUS/MINUS */
		if (getlval(p->n_right) == 1)
			{
			printf("%s", (p->n_op == PLUS ? "inc" : "dec") );
			return;
			}
		printf("%s ", (p->n_op == PLUS ? "adc" : "sbc") );
		adrput(stdout, p->n_right);
		return;
	case 'I': // indexed access;
		l = getlr( p, 'L' );
		r = getlr( p, 'R' );
		if (getlval(l)) {
			ival = getlval(l);
			printf("ldx #$%x ; l",  ival);
		}
		if (getlval(r)) {
			ival = getlval(r);
			printf("ldx #$%x ; r",  ival);
		}
//		fwalk(p, e2print, 0);
		break;
	case 'R': // output based on register
		r = getlr( p, 'L');
		printf("%s", r->n_rval==0?"p":rnames[r->n_rval]);
		break;
	case 'L':
		t = p->n_left->n_type;
		/* FALLTHROUGH */
	case 'A':
		s = (t == CHAR || t == UCHAR ? "b" :
		    t == SHORT || t == USHORT || t == INT ? "w" : 
		    t == FLOAT ? "s" :
		    t == DOUBLE ? "d" :
		    t == LDOUBLE ? "x" : "l");
		printf("%s", s);
		break;

	case 'B':
		if (p->n_qual)
			printf("	add.l #%d,%%sp ; (ZB)\n", (int)p->n_qual);
		break;
	case 'F': /* Emit float branches */
		switch (p->n_op) {
		case GT: s = "fjnle"; break;
		case GE: s = "fjnlt"; break;
		case LE: s = "fjngt"; break;
		case LT: s = "fjnge"; break;
		case NE: s = "fjne"; break;
		case EQ: s = "fjeq"; break;
		default: comperr("ZF"); s = 0;
		}
		printf("%s " LABFMT "\n", s, p->n_label);
		break;

	case 'P':
		printf("	lea -%d(%%fp),%%a0\n", stkpos);
		break;

	case 'Q': /* struct assign */
		printf("	move.l %d,-(%%sp)\n", 
		    attr_find(p->n_ap, ATTR_P2STRUCT)->iarg(0));
//		expand(p, INAREG, "	move.l AR,-(%sp)\n");
//		expand(p, INAREG, "	move.l AL,-(%sp)\n");
		printf("	jsr memcpy\n");
		printf("	add.l #12,%%sp ;\n");
		break;

	case 'S': /* struct arg */

		printf("macro truct arg");
//		starg(p);
		break;

	case '2':
		printf("macro 2");
//		if (regno(getlr(p, '2')) != regno(getlr(p, 'L')))
//			expand(p, INAREG, "	fmove.x AL,A2\n");
		break;

	default:
		comperr("zzzcode %c", c);
	}
}

/*
 * Does the bitfield shape match?
 */
int flshape(NODE *p) {
	comperr("flshape");
	return(0);
}

/* INTEMP shapes must not contain any temporary registers */
/* XXX should this go away now? */
int shtemp(NODE *p) {
	fprintf(stderr, "shtemp %p\n",p);
	return 0;
}

/**
 * Print out constants for M or N bitfield modifier.
 */
void adrcon(CONSZ val) {
	printf("#" CONFMT, val);
}

/**
 * Print out constant for C table directive.
 */
void conput(FILE *fp, NODE *p) {
	long val = getlval(p);

	if (p->n_type <= UCHAR)
		val &= 255;
	else if (p->n_type <= USHORT)
		val &= 65535;

	switch (p->n_op) {
	case ICON:
		if (p->n_name[0]) {
			fprintf(fp, "%s", p->n_name);
		} else {
		fprintf(fp, "%ld", val);
		}
		break;

	default:
		comperr("illegal conput, p %p", p);
	}
}

/*ARGSUSED*/
void insput(NODE *p) {
	comperr("insput");
}

/*
 * Write out the upper address, like the upper register of a 2-register
 * reference, or the next memory location.
 */
void upput(NODE *p, int size) {
	printf("uput");
	switch (p->n_op) {
	case REG:
		printf("%%%s", &rnames[p->n_rval][2]);
		break;
	case NAME:
	case OREG:
		setlval(p, getlval(p) + 4);
		adrput(stdout, p);
		setlval(p, getlval(p) - 4);
		break;

	case ICON:
		printf("#%d", (int)getlval(p));
		break;

	default:
		comperr("upput bad op %d size %d", p->n_op, size);
	}
}

/**
 * output address 
 */
void adrput(FILE *io, NODE *p) {
	int r;
fprintf(stderr,"adrput %p, $d\n",p, p->n_op);
	/* output an address, with offsets, from p */
	switch (p->n_op) {
	case NAME:
	    // 65816 has no displacement, muss load x instead
		// if (getlval(p))
		// 	fprintf(io, CONFMT "%s", getlval(p),
		// 	    *p->n_name ? "+" : "");
		if (p->n_name[0]) {
			fprintf(io, "%s", p->n_name);
		} else {
			comperr("adrput");
		}
		return;

	case OREG:
	case UMUL: // FIXME we dident turn into oreg so we end up with umul here

		r = p->n_rval;
		// this is the place where variables on the stack a accesses

	    // 65816 has no displacement, muss load x instead
		// if (getlval(p)) {
		// 	fprintf(io, CONFMT "%s", getlval(p),
		// 	    *p->n_name ? "+" : "");
		// }

		fprintf(io, "%d,S",p->n_rval);

		return ;
		fprintf(io, "||| %d ||| ",p->n_rval);

		if (p->n_name[0]) {
			fprintf(io,"%s", p->n_name);
		}
		if (R2TEST(r)) {
			int r1 = R2UPK1(r);
			int r2 = R2UPK2(r);
			int sh = R2UPK3(r);

			fprintf(io, "(%s,%s,%d)", 
			    r1 == MAXREGS ? "" : rnames[r1],
			    r2 == MAXREGS ? "" : rnames[r2], sh);
		} else {
			if (getlval(p)) {
				fprintf(io, "(%s,x)", rnames[p->n_rval]);
			} else {
				fprintf(io, "(%s)", rnames[p->n_rval]);
			}
		}
		return;
	case ICON:
		/* addressable value of the constant */
		if (p->n_type == LONGLONG || p->n_type == ULONGLONG) {
			fprintf(io, "#" CONFMT, getlval(p) >> 32);
		} else {
			fputc('#', io);
			conput(io, p);
		}
		return;

	case REG:
		// if ((p->n_type == LONGLONG || p->n_type == ULONGLONG) &&
		// 	/* XXX allocated reg may get wrong type here */
			
		//     (p->n_rval > A7 && p->n_rval < FP0)) {
		// 	fprintf(io, "%%%c%c", rnames[p->n_rval][0],
		// 	    rnames[p->n_rval][1]);
				
		// } else
		if (getlval(p)) {
			fprintf(io, "%s,x", rnames[p->n_rval]);
		} else {
			fprintf(io, "%s", rnames[p->n_rval]);
		}
		return;
		
	default:
	fprintf(io,"illegal address, op %d, node %p", p->n_op, p);
//		comperr("illegal address, op %d, node %p", p->n_op, p);
		return;

	}
}

static char *
ccbranches[] = {
	"beq",		/* jumpe */
	"bne",		/* jumpn */
	"beq",		/* jumple */
	"jlt",		/* jumpl */
	"beq",		/* jumpge */
	"jgt",		/* jumpg */
	"beq",		/* jumple (jlequ) */
	"jcs",		/* jumpl (jlssu) */
	"beq",		/* jumpge (jgequ) */
	"jhi",		/* jumpg (jgtru) */
};


/*   printf conditional and unconditional branches */
void cbgen(int o, int lab) {
	if (o < EQ || o > UGT)
		comperr("bad conditional branch: %s", opst[o]);
	printf("	%s " LABFMT "\n", ccbranches[o-EQ], lab);
}

static void mkcall(NODE *p, char *name) {
	p->n_op = CALL;
	p->n_right = mkunode(FUNARG, p->n_left, 0, p->n_left->n_type);
	p->n_left = mklnode(ICON, 0, 0, FTN|p->n_type);
	p->n_left->n_name = name;
}

static void mkcall2(NODE *p, char *name) {
	p->n_op = CALL;
	p->n_right = mkunode(FUNARG, p->n_right, 0, p->n_right->n_type);
	p->n_left = mkunode(FUNARG, p->n_left, 0, p->n_left->n_type);
	p->n_right = mkbinode(CM, p->n_left, p->n_right, INT);
	p->n_left = mklnode(ICON, 0, 0, FTN|p->n_type);
	p->n_left->n_name = name;
}


static void fixcalls(NODE *p, void *arg) {
	struct attr *ap;
	TWORD lt;

	switch (p->n_op) {
	case STCALL:
	case USTCALL:
		ap = attr_find(p->n_ap, ATTR_P2STRUCT);
		if (ap->iarg(0)+p2autooff > stkpos)
			stkpos = ap->iarg(0)+p2autooff;
		break;

	case DIV:
		if (p->n_type == LONGLONG)
			mkcall2(p, "__divdi3");
		else if (p->n_type == ULONGLONG)
			mkcall2(p, "__udivdi3");
		break;

	case MOD:
		if (p->n_type == LONGLONG)
			mkcall2(p, "__moddi3");
		else if (p->n_type == ULONGLONG)
			mkcall2(p, "__umoddi3");
		break;

	case MUL:
		if (p->n_type == LONGLONG || p->n_type == ULONGLONG)
			mkcall2(p, "__muldi3");
		break;

	case LS:
		if (p->n_type == LONGLONG || p->n_type == ULONGLONG)
			mkcall2(p, "__ashldi3");
		break;

	case RS:
		if (p->n_type == LONGLONG)
			mkcall2(p, "__ashrdi3");
		else if (p->n_type == ULONGLONG)
			mkcall2(p, "__lshrdi3");
		break;

	case SCONV:
		lt = p->n_left->n_type;
		switch (p->n_type) {
		case LONGLONG:
			if (lt == FLOAT)
				mkcall(p, "__fixsfdi");
			else if (lt == DOUBLE)
				mkcall(p, "__fixdfdi");
			else if (lt == LDOUBLE)
				mkcall(p, "__fixxfdi");
			break;
		case ULONGLONG:
			if (lt == FLOAT)
				mkcall(p, "__fixunssfdi");
			else if (lt == DOUBLE)
				mkcall(p, "__fixunsdfdi");
			else if (lt == LDOUBLE)
				mkcall(p, "__fixunsxfdi");
			break;
		case FLOAT:
			if (lt == LONGLONG)
				mkcall(p, "__floatdisf");
			else if (lt == ULONGLONG)
				mkcall(p, "__floatundisf");
			break;
		case DOUBLE:
			if (lt == LONGLONG)
				mkcall(p, "__floatdidf");
			else if (lt == ULONGLONG)
				mkcall(p, "__floatundidf");
			break;
		case LDOUBLE:
			if (lt == LONGLONG)
				mkcall(p, "__floatdixf");
			else if (lt == ULONGLONG)
				mkcall(p, "__floatundixf");
			break;
		}
		break;
#if 0
	case XASM:
		p->n_name = adjustname(p->n_name);
		break;
#endif
	}
}

/**
  * Directly when the whole list of trees for a function is
  * read into pass2 the list is given myreader() to let the
  * target do something with it
  */
void myreader(struct interpass *ipole) {
	struct interpass *ip;

	stkpos = p2autooff;
	DLIST_FOREACH(ip, ipole, qelem) {
		if (ip->type != IP_NODE)
			continue;
		walkf(ip->ip_node, fixcalls, 0);
	}
	if (stkpos > p2autooff)
		p2autooff = stkpos;
	if (stkpos > p2maxautooff)
		p2maxautooff = stkpos;
	if (x2debug)
		printip(ipole);
}

/*
 * Remove some PCONVs after OREGs are created.
 */
static void pconv2(NODE *p, void *arg) {
	NODE *q;

	if (p->n_op == PLUS) {
		if (p->n_type == (PTR|SHORT) || p->n_type == (PTR|USHORT)) {
			if (p->n_right->n_op != ICON)
				return;
			if (p->n_left->n_op != PCONV)
				return;
			if (p->n_left->n_left->n_op != OREG)
				return;
			q = p->n_left->n_left;
			nfree(p->n_left);
			p->n_left = q;
			/*
			 * This will be converted to another OREG later.
			 */
		}
	}
}

/**
 * the optimizer spits out a lot of TEMP node, these should be REGS I think
 */
static void tempconv(NODE *p, void *arg) {
	if (p->n_op == TEMP) {
		fprintf(stderr, "TEMP---\n");

		p->n_op = REG;
	}
}

/**
 * Called from canon() to do target-specific stuff with
 * the tree p.
 */
void mycanon(NODE *p) {
	 walkf(p, pconv2, 0);
// XXX used to turn temp into REG nodes..
//	 walkf(p, tempconv, 0);
}

/**
  * Called from optimize() with the linked list to do
  * target-dependent optimizations.
  */
void myoptim(struct interpass *ip) {
	 //walkf(p, eprint, 0);
}

/**
 * Print out a move instruction from register s to d, 
 * carrying data of type t.
 */ 
void rmove(int s, int d, TWORD t) {

	if (t >= FLOAT && t <= TDOUBLE)
		printf("	fmove.x %s,%s\n", rnames[s], rnames[d]);
	else
		printf("	move.l %s,%s; rmove\n", rnames[s], rnames[d]);
}

/*
 * For class cc, find worst-case displacement of the number of
 * registers in the array r[] indexed by class.
 */
int COLORMAP(int cc, int *r) {
	int a,c,i;

	a = r[CLASSA];
	c = r[CLASSB];

	fprintf(stderr, "colormap: %d %d %d %d \n", CLASSA, CLASSB, CLASSC, CLASSD);


	for (i=0;i<MAXREGS;i++) {
		fprintf(stderr, "%d ", r[i]);
	}
		fprintf(stderr, "\n");

	fprintf(stderr, "colormap: %d %d %d \n", cc, a, c);

	if (cc == CLASSA) {
		return r[CLASSA] == 0;
	}
	return r[CLASSB] < 7;

	switch (cc) {
	case CLASSA:
		if (c * 2 + a < 8)
			return 1;
		break;
	case CLASSB:
		return r[CLASSB] < 4;
	}
	return 0;
}



/*
 * Return a class suitable for a specific type.
 */
int gclass(TWORD t) {
	if (t > BTMASK)
		return CLASSB;
	if (t == LONGLONG || t == ULONGLONG)
		return CLASSC;
	if (t == FLOAT || t == DOUBLE || t == LDOUBLE)
		return CLASSD;
	return CLASSA;
}

static int argsiz(NODE *p) {
	TWORD t = p->n_type;

	if (t < LONGLONG || t == FLOAT || t > BTMASK) {
		return 4;
	}
	if (t == LONGLONG || t == ULONGLONG || t == DOUBLE) {
		return 8;
	}
	if (t == LDOUBLE) {
		return 12;
	}
	if (t == STRTY || t == UNIONTY) {
		return (attr_find(p->n_ap, ATTR_P2STRUCT)->iarg(0)+3) & ~3;
	}
	comperr("argsiz");
	return 0;
}

/*
 * Calculate argument sizes.
 */
void lastcall(NODE *p) {
	NODE *op = p;
	int size = 0;

	p->n_qual = 0;
	if (p->n_op != CALL && p->n_op != FORTCALL && p->n_op != STCALL)
		return;
	for (p = p->n_right; p->n_op == CM; p = p->n_left)
		size += argsiz(p->n_right);
	size += argsiz(p);
	op->n_qual = size; /* XXX */
}

/*
 * Special shapes.
 */
int special(NODE *p, int shape) {
	fprintf(stderr, "special %s", prcook(shape));
	return SRNOPE;
}

/*
 * Target-dependent command-line options.
 */
void mflags(char *str) {
}

/*
 * Do something target-dependent for xasm arguments.
 */
int myxasm(struct interpass *ip, NODE *p) {
	int cw = xasmcode(p->n_name);
	int ww;
	char *w;

	ww = XASMVAL(cw);
again:	switch (ww) {
	case 'd': /* Just convert to reg */
	case 'a':
		p->n_name = tmpstrdup(p->n_name);
		w = strchr(p->n_name, XASMVAL(cw));
		*w = 'r'; /* now reg */
		break;
	case 'o': /* offsetable reg */
		if (p->n_left->n_op == UMUL || p->n_left->n_op == OREG ||
		    p->n_left->n_op == NAME) {
			return 1;
		}
		if (ww == XASMVAL(cw))
			ww = XASMVAL1(cw);
		else
			ww = XASMVAL2(cw);
		goto again;
	}
	return 0;
}

/*
 * Handle special characters following % in gcc extended assembler.
 */
int targarg(char *w, void *arg) {
	switch (w[1]) {
	case '.': /* Remove dot if not needed */
		printf(".");
		break;
	default:
		return 0;
	}
	return 1;
}
