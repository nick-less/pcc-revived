/*	$Id: order.c,v 1.5 2016/01/30 17:26:19 ragge Exp $	*/
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


#include "pass2.h"

#include <string.h>

int canaddr(NODE *);

/* is it legal to make an OREG or NAME entry which has an
 * offset of off, (from a register of r), if the
 * resulting thing had type t */
int notoff(TWORD t, int r, CONSZ off, char *cp) {

	return(1);  /* NO */

	if (off > MAX_SHORT || off < MIN_SHORT)
		return 1; /* max signed 16-bit offset */
	return(0);  /* YES */
}

/*
 * Turn a UMUL-referenced node into OREG.
 * Be careful about register classes, this is a place where classes change.
 * Especially on m68k we must be careful about class changes;
 * Pointers can only have a scalar added to it if PLUS, but when
 * MINUS may be either only pointers or left pointer and right scalar.
 *
 * So far we only handle the trivial OREGs here.
 */
void offstar(NODE *p, int shape) {
	NODE *r;

fprintf(stderr, "offstar %p, %d\n", p, shape);

 	if (isreg(p))
		return; /* Is already OREG */

	r = p->n_right;
	if( p->n_op == PLUS || p->n_op == MINUS ){
		if( r->n_op == ICON ){
			if (isreg(p->n_left) == 0) {
				(void)geninsn(p->n_left, INAREG);
				/* Converted in ormake() */
			}
			return;
		}
		if (r->n_op == LS && r->n_right->n_op == ICON &&
		    getlval(r->n_right) == 2 && p->n_op == PLUS) {
			if (isreg(p->n_left) == 0) {
 				(void)geninsn(p->n_left, INAREG);
			}
			if (isreg(r->n_left) == 0) {
				(void)geninsn(r->n_left, INAREG);
			}
			return;
		}
	}
	(void)geninsn(p, INAREG);
}


/*
 * Shape matches for UMUL.  Cooperates with offstar().
 */
int shumul(NODE *p, int shape) {
fprintf(stderr, "shumul %p, %08x %s\n", p, shape, prcook(shape));


	if (shape & SOREG) {
		fprintf(stderr,"shumul SROREG\n");
		return SROREG;
	}
	if ((shape & STARNM) && (p->n_op == NAME)) {
		fprintf(stderr,"shumul SRDIR\n");
		return SRDIR;
	}
	if (shape & STARREG) {
		fprintf(stderr,"shumul SROREG\n");
		return SROREG;
	}
		fprintf(stderr,"shumul SRNOPE\n");
	return SRNOPE;
}


/*
 * Do the actual conversion of offstar-found OREGs into real OREGs.
 * For simple OREGs conversion should already be done.
 */
void myormake(NODE *q) {
		NODE *p, *r;

fprintf(stderr, "myormake %p %d %d\n", q, q->n_op, q->n_left->n_op);
//		fwalk(q, e2print, 0);
return;
	p = q->n_left;
	if (p->n_op == PLUS && (r = p->n_right)->n_op == LS &&
	    r->n_right->n_op == ICON && getlval(r->n_right) == 2 &&
	    p->n_left->n_op == REG && r->n_left->n_op == REG) {
		q->n_op = OREG;
		setlval(q, 0);
		q->n_rval = R2PACK(p->n_left->n_rval, r->n_left->n_rval, 0);
		tfree(p);
	}

}

/*
 * Rewrite operations on binary operators (like +, -, etc...).
 * Called as a result of table lookup.
 */
int setbin(NODE *p) {

	if (x2debug)
		fprintf(stderr, "setbin(%p)\n", p);
	return 0;

}

/* setup for assignment operator */
int setasg(NODE *p, int cookie) {
fprintf(stderr, "setasg %p, %s\n", p, prcook(cookie));
		fwalk(p, e2print, 0);

	if (x2debug)
		printf("setasg(%p)\n", p);
	return(0);
}

/* setup for unary operator */
int setuni(NODE *p, int cookie) {
	return 0;
}

/*
 * Special handling of some instruction register allocation.
 * - left is the register that left node wants.
 * - right is the register that right node wants.
 * - res is in which register the result will end up.
 * - mask is registers that will be clobbered.
 *
 */
struct rspecial * nspecial(struct optab *q) {
fprintf(stderr, "nspecial %p\n", q);


switch (q->op) {
	case ASSIGN:
		// if (q->lshape == STARREG && q->rshape == SNAME) {
		// 	static struct rspecial s[] = {
		// 		{ NEVER, AC }, { NRES, AC }, { 0 }
		// 	};
		// 	return s;
		// }
		break;
	default:
		comperr("nspecial entry %d", q - table);
	}
	return 0;

	// switch (q->op) {
	// case STASG:
	// 	{
	// 		static struct rspecial s[] = {
	// 			{ NEVER, D0 }, { NEVER, D1 },
	// 			{ NEVER, A0 }, { NEVER, A1 },
	// 			/* { NRES, A0 }, */ { 0 } };
	// 		return s;
	// 	}
	// }
	// comperr("nspecial entry %d", q - table);
	return 1; /* XXX gcc */
}

/*
 * Set evaluation order of a binary node if it differs from default.
 */
int setorder(NODE *p) {
	return 0;
}

/*
 * set registers in calling conventions live.
 */
int * livecall(NODE *p) {

	static int r[] = { -1 };
	return r;

	//  static int r[] = { AX, -1 };
	//  return &r[1];

	// if (p->n_op == STCALL)
	// 	return r; /* only if struct return */
	// return &r[1];
}

/*
 * Signal whether the instruction is acceptable for this target.
 * Targets may use the unused bits in the visit field and
 *	then check them here when generating code.
 */
int acceptable(struct optab *op) {


	return 1;
}
