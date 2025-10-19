%{
// Ensure POSIX functions like strdup/fileno/isatty are declared
#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include "compiler.tab.h" // For Bison token definitions
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// Use Bison's yylval (defined in compiler.tab.h)
extern YYSTYPE yylval;

// Function to handle errors
void yyerror(const char *s);
%}

%option noyywrap
%option yylineno

%%
[ \t\n]             ;                                   /* Ignore whitespace */

"print"             { return PRINT; }                   /* Keyword */

[a-zA-Z][a-zA-Z0-9]* {                                  /* Identifier */
                        yylval.sval = strdup(yytext);
                        return ID;
                    }

[0-9]+              {                                   /* Integer Literal */
                        yylval.ival = atoi(yytext);
                        return INT_LIT;
                    }

"="                 { return ASSIGN; }
"+"                 { return ADD; }
"-"                 { return SUB; }
"*"                 { return MUL; }
"/"                 { return DIV; }
";"                 { return SEMICOLON; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }

.                   {
                        fprintf(stderr, "Lexical Error: Invalid character '%s' on line %d\n", yytext, yylineno);
                        exit(1);
                    }

%%