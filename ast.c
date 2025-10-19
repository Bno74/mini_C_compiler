#include "ast.h"

#define STMT_LIST_INITIAL_CAPACITY 4

// --- NODE CREATION FUNCTIONS ---

ASTNode *create_stmt_list_node() {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) { perror("malloc failed"); exit(EXIT_FAILURE); }
    node->type = NODE_STMT_LIST;
    node->data.stmt_list.count = 0;
    node->data.stmt_list.capacity = STMT_LIST_INITIAL_CAPACITY;
    node->data.stmt_list.statements = (ASTNode **)malloc(
        STMT_LIST_INITIAL_CAPACITY * sizeof(ASTNode *)
    );
    if (!node->data.stmt_list.statements) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    return node;
}

ASTNode *create_assign_node(char *id_name, ASTNode *expr) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) { perror("malloc failed"); exit(EXIT_FAILURE); }
    node->type = NODE_ASSIGN;
    node->data.assign.id_name = strdup(id_name); // Duplicate the name
    node->data.assign.expr = expr;
    return node;
}

ASTNode *create_print_node(ASTNode *expr) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) { perror("malloc failed"); exit(EXIT_FAILURE); }
    node->type = NODE_PRINT;
    node->data.print.expr = expr;
    return node;
}

ASTNode *create_binop_node(BinOpType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) { perror("malloc failed"); exit(EXIT_FAILURE); }
    node->type = NODE_BINOP;
    node->data.binop.op = op;
    node->data.binop.left = left;
    node->data.binop.right = right;
    return node;
}

ASTNode *create_int_literal_node(int value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) { perror("malloc failed"); exit(EXIT_FAILURE); }
    node->type = NODE_INT_LITERAL;
    node->data.int_literal.value = value;
    return node;
}

ASTNode *create_identifier_node(char *name) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) { perror("malloc failed"); exit(EXIT_FAILURE); }
    node->type = NODE_IDENTIFIER;
    node->data.identifier.name = strdup(name); // Duplicate the name
    return node;
}

// --- UTILITY FUNCTIONS ---

void stmt_list_append(ASTNode *list, ASTNode *stmt) {
    if (list->type != NODE_STMT_LIST) return;

    if (list->data.stmt_list.count == list->data.stmt_list.capacity) {
        list->data.stmt_list.capacity *= 2;
        list->data.stmt_list.statements = (ASTNode **)realloc(
            list->data.stmt_list.statements,
            list->data.stmt_list.capacity * sizeof(ASTNode *)
        );
        if (!list->data.stmt_list.statements) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
    }
    list->data.stmt_list.statements[list->data.stmt_list.count++] = stmt;
}

// --- FREEING THE AST ---

void free_ast(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_STMT_LIST:
            for (int i = 0; i < node->data.stmt_list.count; i++) {
                free_ast(node->data.stmt_list.statements[i]);
            }
            free(node->data.stmt_list.statements);
            break;

        case NODE_ASSIGN:
            free(node->data.assign.id_name);
            free_ast(node->data.assign.expr);
            break;

        case NODE_PRINT:
            free_ast(node->data.print.expr);
            break;

        case NODE_BINOP:
            free_ast(node->data.binop.left);
            free_ast(node->data.binop.right);
            break;

        case NODE_IDENTIFIER:
            free(node->data.identifier.name);
            break;

        case NODE_INT_LITERAL:
            break;
    }
    free(node);
}