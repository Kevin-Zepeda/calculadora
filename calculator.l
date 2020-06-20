/* recognize tokens for the calculator */

%{
# include "funcs.h"
# include "calculator.tab.h"
%}

/* define float exponent mode */
EXP		([Ee][-+]?[0-9]+)

%%
 /* single character ops */		 
[-+*/=|,;(){}] 	{return yytext[0];}

 /* comparison ops */
">"		{ yylval.fn = 1; return CMP; }
"<"		{ yylval.fn = 2; return CMP; }
"<>"		{ yylval.fn = 3; return CMP; }
"!="		{ yylval.fn = 3; return CMP; }
"=="		{ yylval.fn = 4; return CMP; }
">="		{ yylval.fn = 5; return CMP; }
"<="		{ yylval.fn = 6; return CMP; }

 /* keywords */
"if"		{ return IF; }
"then"		{ return THEN; }
"else"		{ return ELSE; }
"while"		{ return WHILE; }
"do"		{ return DO; }
"def"		{ return DEF;}

 /* built in functions */
"sqrt"		{ yylval.fn = B_sqrt; return FUNC; }
"exp"		{ yylval.fn = B_exp; return FUNC; }
"log"		{ yylval.fn = B_log; return FUNC; }
"sin"		{ yylval.fn = B_sin; return FUNC; }
"cos"		{ yylval.fn = B_cos; return FUNC; }
"print"		{ yylval.fn = B_print; return FUNC; }

 /* names refference */
[a-zA-Z][a-zA-Z0-9]*	{ yylval.s = lookup(yytext); return NAME; }

 /* exp float number */
[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}?	{ yylval.d = atof(yytext); return NUMBER; } 

 /* ignore white space and check end of line */ 
[ \t]		{}
"\n"		{ return EOL; }
 /* unknown character */
.		{ yyerror("Mystery character %c\n", *yytext); }
%%

