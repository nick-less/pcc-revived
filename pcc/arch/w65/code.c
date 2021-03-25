/*	$Id: code.c,v 1.8 2016/01/30 17:26:19 ragge Exp $	*/
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


# include "pass1.h"

#ifdef LANG_CXX
#define p1listf listf
#define p1tfree tfree
#else
#define NODE P1ND
#define talloc p1alloc
#define tfree p1tfree
#endif

extern int gotnr;

/*
 * Print out assembler segment name.
 */
void setseg(int seg, char *name) {
	switch (seg) {
	case PROG: name = ".segment 	\"CODE\""; break;
	case DATA:
	case LDATA: name = ".segment 	\"BSS\""; break;
	case STRNG:
	case RDATA: name = ".segment 	\"RODATA\""; break;
	case UDATA: break;
	case PICLDATA:
	case PICDATA: name = ".segment .data.rel.rw,\"aw\",@progbits"; break;
	case PICRDATA: name = ".segment .data.rel.ro,\"aw\",@progbits"; break;
	case TLSDATA: name = ".segment .tdata,\"awT\",@progbits"; break;
	case TLSUDATA: name = ".segment .tbss,\"awT\",@nobits"; break;
	case CTORS: name = ".segment\t.ctors,\"aw\",@progbits"; break;
	case DTORS: name = ".segment\t.dtors,\"aw\",@progbits"; break;
	case NMSEG: 
		printf("\t.segment %s,\"a%c\",@progbits\n", name,
		    cftnsp ? 'x' : 'w');
		return;
	}
	printf("\t%s\n", name);
}

static char* mygettype (int size) {
	switch (size) {
		case 2:
		return ".word";
		case 4:
		return ".dword";
	}
	return ".byte";
}
/*
 * Define everything needed to print out some data (or text).
 * This means segment, alignment, visibility, etc.
 */
void defloc(struct symtab *sp) {
	char *name;
printf("defloc %s", sp->sname);
	name = getexname(sp);
	if (sp->sclass == EXTDEF) {
		printf("\t.global %s\n", name);
		if (ISFTN(sp->stype)) {
			printf(";\t.type %s,@function\n", name);
			printf(".proc %s: near\n", name);
		} else {
			printf(";\t.type %s,@object\n", name);
			printf(";\t.size %s,%d\n", name,
			    (int)tsize(sp->stype, sp->sdf, sp->sap)/SZCHAR);

//			printf("%s:\t%s\t", name, mygettype(tsize(sp->stype, sp->sdf, sp->sap)/SZCHAR));
			
		}
	}
	if (sp->slevel == 0)
		printf("%s:\n", name);
	else
		printf(LABFMT ":\n", sp->soffset);
}

/*
 * code for the end of a function
 * deals with struct return here
 * The return value is in (or pointed to by) RETREG.
 */
int sttemp;

void efcode(void) {
	if (cftnsp->stype != STRTY+FTN && cftnsp->stype != UNIONTY+FTN)
		return;
}

/*
 * code for the beginning of a function; a is an array of
 * indices in symtab for the arguments; n is the number
 */
void bfcode(struct symtab **sp, int cnt) {
	NODE *p, *p2;
	char *c;
	int i, l;
	printf("bfcode\n");
return ;
	/* handcraft isz */
	l = strlen(cftnsp->sname)+10;
	c = tmpalloc(l);
	snprintf(c, l, "	isz %s\n", exname(cftnsp->sname));

	/* Convert param symtab entries to NAMEs */
	for (i = 0; i < cnt; i++) {
		sp[i]->sclass = STATIC;
		sp[i]->soffset = getlab();
		p2 = cast(buildtree(ADDROF,
			nametree(cftnsp), 0), INCREF(INCREF(sp[i]->stype)), 0);
		p = buildtree(ASSIGN, nametree(sp[i]),
		    buildtree(UMUL, buildtree(UMUL, p2, 0), 0));
		ecomp(p);
		send_passt(IP_ASM, c);
		printf(LABFMT ":	0\n", sp[i]->soffset);
	}
}


/* called just before final exit */
/* flag is 1 if errors, 0 if none */
void ejobcode(int flag) {
	printf("ejobcode%d\n", 0);
	fflush(stdout);

	if (flag)
		return;

}

/* begin of job emit code */
void bjobcode(void) { 
	printf("\t.fopt compiler,\"PCC: %s\"\n", VERSSTR);


	printf(".P816\n");
	printf(".A16\n");
	printf(".I16\n");

	printf("	.importzp	zp1l, zp2l, zp3l, zp4l\n");

	/* Set correct names for our types */
	astypnames[SHORT] = astypnames[USHORT] = "\t.word";
	astypnames[INT] = astypnames[UNSIGNED] = "\t.word";
	astypnames[LONG] = astypnames[ULONG] = "\t.dword";
	astypnames[LONGLONG] = astypnames[ULONGLONG] = "\t.res 8,0";
	fflush(stdout);

}

/*
 * Called with a function call with arguments as argument.
 * This is done early in buildtree() and only done once.
 * Returns p.
 */
NODE * funcode(NODE *p) {
	NODE *r, *l;
	TWORD t = DECREF(DECREF(p->n_left->n_type));
	int stcall;

	stcall = ISSOU(t);
	/*
	 * We may have to prepend:
	 * - Hidden arg0 for struct return (in reg or on stack).
	 * - ebx in case of PIC code.
	 */

	/* Fix function call arguments. On x86, just add funarg */
	for (r = p->n_right; r->n_op == CM; r = r->n_left) {
		if (r->n_right->n_op != STARG)
			r->n_right = block(FUNARG, r->n_right, NIL,
			    r->n_right->n_type, r->n_right->n_df,
			    r->n_right->n_ap);
	}
	if (r->n_op != STARG) {
		l = talloc();
		*l = *r;
		r->n_op = FUNARG;
		r->n_left = l;
		r->n_type = l->n_type;
	}
	return p;

}

/* fix up type of field p */
void fldty(struct symtab *p) {
}

/*
 * XXX - fix genswitch.
 */
int mygenswitch(int num, TWORD type, struct swents **p, int n) {
	return 0;
}

/*
 * Return return as given by a.
 */
NODE * builtin_return_address(const struct bitable *bt, NODE *a) {
	int nframes;
	NODE *f;

cerror((char *)__func__);
	nframes = glval(a);
	tfree(a);

	f = block(REG, NIL, NIL, PTR+VOID, 0, 0);
	regno(f) = FPREG;

	while (nframes--)
		f = block(UMUL, f, NIL, PTR+VOID, 0, 0);

	f = block(PLUS, f, bcon(8), INCREF(PTR+VOID), 0, 0);
	f = buildtree(UMUL, f, NIL);

	return f;
}

/*
 * Return frame as given by a.
 */
NODE * builtin_frame_address(const struct bitable *bt, NODE *a) {
	int nframes;
	NODE *f;

cerror((char *)__func__);
	nframes = glval(a);
	tfree(a);

	f = block(REG, NIL, NIL, PTR+VOID, 0, 0);
	regno(f) = FPREG;

	while (nframes--)
		f = block(UMUL, f, NIL, PTR+VOID, 0, 0);

	return f;
}

/*
 * Return "canonical frame address".
 */
NODE *
builtin_cfa(const struct bitable *bt, NODE *a) {
	NODE *f;

cerror((char *)__func__);
	f = block(REG, NIL, NIL, PTR+VOID, 0, 0);
	regno(f) = FPREG;
	return block(PLUS, f, bcon(16), INCREF(PTR+VOID), 0, 0);
}
