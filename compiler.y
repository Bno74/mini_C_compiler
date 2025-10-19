%{
#include "ast.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Bison/Flex Communication ---
// Bison/Flex token value is declared in the generated header (compiler.tab.h)

// AST Root Pointer (The final tree)
ASTNode *ast_root = NULL;

// Function declarations
int yylex();
void yyerror(const char *s);
// Line number from the lexer
extern int yylineno;
%}

// --- TOKEN DEFINITIONS ---
%union {
    int ival;           // For integer literals
    char *sval;         // For identifiers
    ASTNode *nval;      // For AST nodes
}

// Tokens (passed from Flex)
%token <ival> INT_LIT
%token <sval> ID
%token ADD SUB MUL DIV ASSIGN SEMICOLON LPAREN RPAREN
%token PRINT

// Types for non-terminals (AST node pointers)
%type <nval> program stmt_list stmt expr

// Operator precedence and associativity (highest to lowest)
%right ASSIGN
%left ADD SUB
%left MUL DIV
%right NEG // Unary minus

%%
// --- GRAMMAR RULES ---

program:
        stmt_list
        {
            // The result of the program is the final statement list
            ast_root = $1;
        }
    ;

stmt_list:
        stmt_list stmt
        {
            // Append the new statement ($2) to the existing list ($1)
            stmt_list_append($1, $2);
            $$ = $1;
        }
    |   stmt
        {
            // Create a new statement list for the first statement
            $$ = create_stmt_list_node();
            stmt_list_append($$, $1);
        }
    ;

stmt:
        ID ASSIGN expr SEMICOLON
        {
            // ID = EXPR; -> Assignment node
            // The name is duplicated in the node creator, so we free the original.
            $$ = create_assign_node($1, $3);
            free($1);
        }
    |   PRINT expr SEMICOLON
        {
            // print EXPR; -> Print node
            $$ = create_print_node($2);
        }
    ;

expr:
        INT_LIT
        {
            // Integer literal node
            $$ = create_int_literal_node($1);
        }
    |   ID
        {
            // Identifier node
            // The name is duplicated in the node creator, so we free the original.
            $$ = create_identifier_node($1);
            free($1);
        }
    |   expr ADD expr
        {
            $$ = create_binop_node(OP_ADD, $1, $3);
        }
    |   expr SUB expr
        {
            $$ = create_binop_node(OP_SUB, $1, $3);
        }
    |   expr MUL expr
        {
            $$ = create_binop_node(OP_MUL, $1, $3);
        }
    |   expr DIV expr
        {
            $$ = create_binop_node(OP_DIV, $1, $3);
        }
    |   SUB expr %prec NEG
        {
            // Unary negation: translate as 0 - expr
            ASTNode *zero = create_int_literal_node(0);
            $$ = create_binop_node(OP_SUB, zero, $2);
        }
    |   LPAREN expr RPAREN
        {
            // Parentheses for grouping expressions
            $$ = $2;
        }
    ;
%%

// --- ERROR HANDLING ---
void yyerror(const char *s) {
    fprintf(stderr, "Parsing Error: %s on line %d\n", s, yylineno);
    // Exit on parse error
    exit(EXIT_FAILURE);
}

// --- MAIN DRIVER (Defined in main.c) ---