%{
#	include <stdio.h>
#	include <stdlib.h>
#	include "funcs.h"
%}

%union {
	struct ast *a;
	double d;
	struct symbol *s;		/* which symbol */
	struct symlist *sl;
	int fn;			/* which function */
}

/* declare tokens */
%token <d> NUMBER
%token <s> NAME
%token <fn> FUNC
%token EOL

%token IF THEN ELSE WHILE DO DEF

%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'

%type <a> expression statement block explist
%type <sl> symlist

%start calclist

%%

statement: IF expression THEN '{' block '}'			{ $$ = newflow('I', $2, $5, NULL); }
	 | IF expression THEN '{' block '}' ELSE '{' block '}'	{ $$ = newflow('I', $2, $5, $9); }
	 | WHILE expression DO '{' block '}'			{ $$ = newflow('W', $2, $5, NULL); }
	 | expression ';'
	;

block: /* nothing */ { $$ = NULL; }
	| statement block {
			if ($2 == NULL) {
				$$ = $1;
			} else {
				$$ = newast('L', $1, $2);
			}
		}
	 ;

expression: expression CMP expression	{ $$ = newcmp($2, $1, $3); }
	 | expression '+' expression	{ $$ = newast('+', $1,$3); }
	 | expression '-' expression	{ $$ = newast('-', $1,$3);}
	 | expression '*' expression	{ $$ = newast('*', $1,$3); }
	 | expression '/' expression	{ $$ = newast('/', $1,$3); }
	 | '|' expression '|'		{ $$ = newast('|', $2, NULL); }
	 | '(' expression ')'		{ $$ = $2; }
	 | '-' expression		{ $$ = newast('M', $2, NULL); }
	 | NUMBER			{ $$ = newnum($1); }
	 | FUNC '(' explist ')'		{ $$ = newfunc($1, $3); }
	 | NAME				{ $$ = newref($1); }
	 | NAME '=' expression		{ $$ = newasgn($1, $3); }
	 | NAME '(' explist ')'		{ $$ = newcall($1, $3); }
	;

explist: expression
	| expression ',' explist	{ $$ = newast('L', $1, $3); }
	;
symlist: NAME				{ $$ = newsymlist($1, NULL); }
	| NAME ',' symlist		{ $$ = newsymlist($1, $3); }
	;

calclist: /* nothing */
	| calclist statement EOL {
			printf("= %4.4g\n> ", eval($2));
			treefree($2);
		}
	| calclist DEF NAME '(' symlist ')' '{' block '}' EOL {
			dodef($3, $5, $8);
			printf("Function Defined %s\n> ", $3->name);
		}
	| calclist error EOL {
			yyerrok; printf("> ");
		}
	;
%%


