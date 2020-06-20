#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "funcs.h"

// symbol table to hash a symbol
static unsigned symhash(char *sym) {
	unsigned int hash = 0;
	unsigned c;
	while(c = *sym++) {
		hash = hash*9 ^ c;
	}
	return hash;
}
struct symbol* lookup(char* sym) {
	struct symbol *sp = &symtab[symhash(sym)%NHASH];
	int scount = NHASH;
	while(--scount >= 0) {
		if(sp->name && !strcmp(sp->name, sym)) {
			return sp;
		}
		if(!sp->name) {
			sp->name = strdup(sym);
			sp->value = 0;
			sp->func = NULL;
			sp->syms = NULL;
			return sp;
		}

		if(++sp >= symtab+NHASH) sp = symtab; // try the next entry
	}
	yyerror("symbol table overflow\n");
	return NULL;
}
struct ast* newast(int nodetype, struct ast *l, struct ast *r) {
	struct ast *a = malloc(sizeof(struct ast));
	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}
struct ast* newnum(double d) {
	struct numval *a = malloc(sizeof(struct numval));
	a->nodetype = 'K';
	a->number = d;
	return (struct ast *)a;
}
struct ast* newcmp(int cmptype, struct ast *l, struct ast *r) {
	struct ast *a = malloc(sizeof(struct ast));
	a->nodetype = '0' + cmptype;
	a->l = l;
	a->r = r;
	return a;
}
struct ast* newfunc(int functype, struct ast *l) {
	struct fncall *a = malloc(sizeof(struct fncall));
	a->nodetype = 'F';
	a->l = l;
	a->functype = functype;
	return (struct ast *)a;
}
struct ast* newcall(struct symbol *s, struct ast *l) {
	struct ufncall *a = malloc(sizeof(struct ufncall));
	a->nodetype = 'C';
	a->l = l;
	a->s = s;
	return (struct ast *)a;
}
struct ast* newref(struct symbol *s) {
	struct symref *a = malloc(sizeof(struct symref));
	a->nodetype = 'N';
	a->s = s;
	return (struct ast *)a;
}
struct ast* newasgn(struct symbol *s, struct ast *v) {
	struct symasgn *a = malloc(sizeof(struct symasgn));
	a->nodetype = '=';
	a->s = s;
	a->v = v;
	return (struct ast *)a;
}
struct ast* newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el) {
	struct flow *a = malloc(sizeof(struct flow));
	a->nodetype = nodetype;
	a->cond = cond;
	a->tl = tl;
	a->el = el;
	return (struct ast *)a;
}
struct symlist* newsymlist(struct symbol *sym, struct symlist *next) {
	struct symlist *sl = malloc(sizeof(struct symlist));
	sl->sym = sym;
	sl->next = next;
	return sl;
}
void symlistfree(struct symlist *sl) {
	struct symlist *nsl;
	while(sl) {
		nsl = sl->next;
		free(sl);
		sl = nsl;
	}
}
void dodef(struct symbol *name, struct symlist *syms, struct ast *func) {
	if(name->syms) {
		symlistfree(name->syms);
	}
	if(name->func) {
		treefree(name->func);
	}
	name->syms = syms;
	name->func = func;
}
double eval(struct ast *a) {
	double v;
	if(!a) { // default value
		return 0.0;
	}
	switch(a->nodetype) {
		// constant number
		case 'K': v = ((struct numval *)a)->number; break;
		// name reference
		case 'N': v = ((struct symref *)a)->s->value; break;
		// assignment
		case '=': v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v); break;
		// expressions
		case '+': v = eval(a->l) + eval(a->r); break;
		case '-': v = eval(a->l) - eval(a->r); break;
		case '*': v = eval(a->l) * eval(a->r); break;
		case '/': v = eval(a->l) / eval(a->r); break;
		case '|': v = fabs(eval(a->l)); break;
		case 'M': v = -eval(a->l); break;
		// comparisons
		case '1': v = (eval(a->l) > eval(a->r))? 1 : 0; break;
		case '2': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
		case '3': v = (eval(a->l) != eval(a->r))? 1 : 0; break;
		case '4': v = (eval(a->l) == eval(a->r))? 1 : 0; break;
		case '5': v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
		case '6': v = (eval(a->l) <= eval(a->r))? 1 : 0; break;
		// if flow
		case 'I':
			if( eval( ((struct flow *)a)->cond) != 0) {
				if( ((struct flow *)a)->tl) { // else branch if exists
					v = eval( ((struct flow *)a)->tl);
				} else {
					v = 0.0; // default value
				}
			} else {
				if( ((struct flow *)a)->el) { // if branch if exists
					v = eval(((struct flow *)a)->el);
				} else {
					v = 0.0; // default value
				}
			}
			break;
		case 'W': // while flow
			v = 0.0;		/* default value */
			if( ((struct flow *)a)->tl) {
				while( eval(((struct flow *)a)->cond) != 0) { // loop
					v = eval(((struct flow *)a)->tl);
				}
			}
			break; // last value is the value returned
		// define user function						
		case 'L': eval(a->l); v = eval(a->r); break;
		// call built-in function
		case 'F': v = callbuiltin((struct fncall *)a); break;
		// call user-defined function
		case 'C': v = calluser((struct ufncall *)a); break;
		// error
		default: printf("internal error: bad node %c\n", a->nodetype);
	}
	return v;
}
static double callbuiltin(struct fncall *f) {
	enum bifs functype = f->functype;
	double v = eval(f->l);
	switch(functype) {
		case B_sqrt: return sqrt(v);
		case B_exp: return exp(v);
		case B_log: return log(v);
		case B_sin: return sin(v);
		case B_cos: return cos(v);
		case B_print: printf("= %4.4g\n", v); return v;
		default: yyerror("Unknown built-in function %d", functype); return 0.0;
	}
}
static double calluser(struct ufncall *f) {
	struct symbol *fn = f->s;	// function name
	struct symlist *sl;		// dummy arguments
	struct ast *args = f->l;	// actual arguments
	double *oldval, *newval;	// saved arg values
	double v;
	int nargs;
	int i;
	if(!fn->func) { // undefined function.
		yyerror("call to undefined function", fn->name);
		return 0;
	}
	// count the arguments
	sl = fn->syms;
	for(nargs = 0; sl; sl = sl->next) {
		nargs++;
	}
	// prepare to save them
	oldval = (double *)malloc(nargs * sizeof(double));
	newval = (double *)malloc(nargs * sizeof(double));
	if(!oldval || !newval) {
		return 0.0; // require for new space error, default return 0.0;
	}
	// evaluate the arguments
	for(i = 0; i < nargs; i++) {
		if(!args) {
			yyerror("too few args in call to %s", fn->name);
			free(oldval); free(newval);
			return 0;
		}
		if(args->nodetype == 'L') { // if this is a list node
			newval[i] = eval(args->l);
			args = args->r;
		} else { // if it's the end of the list
			newval[i] = eval(args);
			args = NULL;
		}
	}
				 
	// save old values of dummies, assign new ones
	sl = fn->syms;
	for(i = 0; i < nargs; i++) {
		struct symbol *s = sl->sym;

		oldval[i] = s->value;
		s->value = newval[i];
		sl = sl->next;
	}
	free(newval);
	// evaluate the function
	v = eval(fn->func);
	// put the dummies back
	sl = fn->syms;
	for(i = 0; i < nargs; i++) {
		struct symbol *s = sl->sym;

		s->value = oldval[i];
		sl = sl->next;
	}
	free(oldval);
	return v;
}
void treefree(struct ast *a) {
	switch(a->nodetype) {
		// two subtrees
		case '+':case '-':case '*':case '/':case '1':case '2':case '3':case '4':case '5':case '6':case 'L': treefree(a->r);
		// one subtree
		case '|':case 'M': case 'C': case 'F': treefree(a->l);
		// no subtree
		case 'K': case 'N': break;
		// assignment
		case '=': free( ((struct symasgn *)a)->v); break;
		// flow
		case 'I': case 'W':
			free( ((struct flow *)a)->cond);
			if( ((struct flow *)a)->tl) free( ((struct flow *)a)->tl);
			if( ((struct flow *)a)->el) free( ((struct flow *)a)->el);
			break;
		// default error
		default: printf("internal error: free bad node %c\n", a->nodetype);
	}
	free(a); // always free the node itself
}
void yyerror(char *s, ...) {
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "error: ");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
}
int yywrap() {
	return 1;
}

int main() {
	printf("> ");
	return yyparse();
}

