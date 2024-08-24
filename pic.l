%{
	#include "pic.hh"
	#include "parse.tab.h"
	using namespace std;
%}

%option noyywrap 

/* DEFINITIONS */

comment										("//".*)
digit                       				[0-9]
fraction                    				([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)
ws											[ \r\n]*
operator									[-+*/=;]
letter                      				[a-zA-Z_]

/* RULES */

%%

{comment}									;
let 										{ yylval.name = new string(yytext); return LET; }
for 										{ yylval.name = new string(yytext); return FOR; }
if 											{ yylval.name = new string(yytext); return IF; }
else 										{ yylval.name = new string(yytext); return ELSE; }
fn 											{ yylval.name = new string(yytext); return FN; }
while 										{ yylval.name = new string(yytext); return WHILE; }
return 										{ yylval.name = new string(yytext); return RETURN; }
"("											{ return '(' ;}
")"											{ return ')' ;}
"{"											{ return '{' ;}
"}"											{ return '}' ;}
"["											{ return '[' ;}
"]"				 							{ return ']' ;}  
","											{ return ',' ;}
"<"											{ return '<' ;}
">"											{ return '>' ;}
"=="										{ yylval.name = new string(yytext); return EQUAL;}
"!="										{ yylval.name = new string(yytext); return NOT_EQUAL;}
"!"											{ return NOT ;}
{digit}+									{ yylval.name = new string(yytext); return INT_CONST; }
{fraction}									{ yylval.name = new string(yytext); return FLT_CONST; }
{operator}									{ return yytext[0]; }
{letter}({letter}|{digit})* 				{ yylval.name = new string(yytext); return NAME; }
{ws}       									; /* Ignore spaces */
.         									{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }
