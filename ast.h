#ifndef AST_H
#define AST_H

// Required for the declaration of strdup()
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- ENUMERATIONS ---
typedef enum {
    NODE_STMT_LIST,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_BINOP,
    NODE_INT_LITERAL,
    NODE_IDENTIFIER
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} BinOpType;


// --- STRUCTURES ---
struct ASTNode;

typedef struct {
    BinOpType op;
    struct ASTNode *left;
    struct ASTNode *right;
} NodeBinOp;

typedef struct {
    int value;
} NodeIntLiteral;

typedef struct {
    char *name;
} NodeIdentifier;

typedef struct {
    char *id_name;
    struct ASTNode *expr;
} NodeAssign;

typedef struct {
    struct ASTNode *expr;
} NodePrint;

typedef struct {
    struct ASTNode **statements;
    int count;
    int capacity;
} NodeStmtList;

typedef struct ASTNode {
    NodeType type;
    union {
        NodeStmtList stmt_list;
        NodeAssign assign;
        NodePrint print;
        NodeBinOp binop;
        NodeIntLiteral int_literal;
        NodeIdentifier identifier;
    } data;
} ASTNode;


// --- FUNCTION PROTOTYPES ---
ASTNode *create_stmt_list_node();
ASTNode *create_assign_node(char *id_name, ASTNode *expr);
ASTNode *create_print_node(ASTNode *expr);
ASTNode *create_binop_node(BinOpType op, ASTNode *left, ASTNode *right);
ASTNode *create_int_literal_node(int value);
ASTNode *create_identifier_node(char *name);
void stmt_list_append(ASTNode *list, ASTNode *stmt);
void free_ast(ASTNode *node);

#endif // AST_H