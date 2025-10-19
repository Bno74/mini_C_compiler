// Required for the declaration of strdup()
#define _POSIX_C_SOURCE 200809L

#include "codegen.h"
#include <errno.h>

// --- Symbol Table (Simple Stack-Based) ---
#define MAX_VARS 64

typedef struct {
    char *name;
    int stack_offset; // Offset from %rbp (e.g., -8, -16, -24, ...)
} Symbol;

static Symbol g_symbol_table[MAX_VARS];
static int g_var_count = 0;
static int g_current_stack_offset = 0; // Starts at 0, decremented by 8

static int get_var_offset(const char *name) {
    for (int i = 0; i < g_var_count; i++) {
        if (strcmp(g_symbol_table[i].name, name) == 0) {
            return g_symbol_table[i].stack_offset;
        }
    }

    if (g_var_count >= MAX_VARS) {
        fprintf(stderr, "Error: Too many variables (max %d).\n", MAX_VARS);
        exit(EXIT_FAILURE);
    }

    g_current_stack_offset -= 8; // Allocate 8 bytes for an integer
    g_symbol_table[g_var_count].name = strdup(name);
    g_symbol_table[g_var_count].stack_offset = g_current_stack_offset;
    g_var_count++;

    return g_current_stack_offset;
}

static void free_symbol_table() {
    for (int i = 0; i < g_var_count; i++) {
        free(g_symbol_table[i].name);
    }
    g_var_count = 0;
    g_current_stack_offset = 0;
}

// --- Code Generation Helper Functions ---

static void codegen_expr(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT_LITERAL:
            fprintf(out, "\tmovq\t$%d, %%rax\n", node->data.int_literal.value);
            break;

        case NODE_IDENTIFIER: {
            int offset = get_var_offset(node->data.identifier.name);
            fprintf(out, "\tmovq\t%d(%%rbp), %%rax\n", offset);
            break;
        }

        case NODE_BINOP: {
            codegen_expr(node->data.binop.left, out);
            fprintf(out, "\tpushq\t%%rax\n");

            codegen_expr(node->data.binop.right, out);

            if (node->data.binop.op == OP_DIV) {
                fprintf(out, "\tpopq\t%%rcx\n");
                fprintf(out, "\tmovq\t%%rax, %%rbx\n");
                fprintf(out, "\tmovq\t%%rcx, %%rax\n");
                fprintf(out, "\tcltd\n");
                fprintf(out, "\tidivq\t%%rbx\n");
            } else {
                fprintf(out, "\tpopq\t%%rbx\n");
                switch (node->data.binop.op) {
                    case OP_ADD:
                        fprintf(out, "\taddq\t%%rbx, %%rax\n");
                        break;
                    case OP_SUB:
                        fprintf(out, "\tsubq\t%%rax, %%rbx\n");
                        fprintf(out, "\tmovq\t%%rbx, %%rax\n");
                        break;
                    case OP_MUL:
                        fprintf(out, "\timulq\t%%rbx, %%rax\n");
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        default:
            fprintf(stderr, "CodeGen Error: Unknown expression node type: %d\n", node->type);
            exit(EXIT_FAILURE);
    }
}

static void codegen_stmt(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGN: {
            codegen_expr(node->data.assign.expr, out);
            int offset = get_var_offset(node->data.assign.id_name);
            fprintf(out, "\tmovq\t%%rax, %d(%%rbp)\n", offset);
            break;
        }

        case NODE_PRINT: {
            codegen_expr(node->data.print.expr, out);
            fprintf(out, "\tmovq\t%%rax, %%rsi\n");
            fprintf(out, "\tleaq\t.L_fmt_d(%%rip), %%rdi\n");
            fprintf(out, "\tmovl\t$0, %%eax\n");
            fprintf(out, "\tcall\tprintf@PLT\n");
            break;
        }

        default:
            fprintf(stderr, "CodeGen Error: Unknown statement node type: %d\n", node->type);
            exit(EXIT_FAILURE);
    }
}

void generate_code(ASTNode *root, const char *filename) {
    FILE *out = fopen(filename, "w");
    if (!out) {
        perror("Error opening output file");
        return;
    }

    if (!root || root->type != NODE_STMT_LIST) {
        fprintf(stderr, "Error: AST root is invalid.\n");
        fclose(out);
        return;
    }

    fprintf(out, ".data\n");
    fprintf(out, ".L_fmt_d:\n");
    fprintf(out, "\t.string \"%%d\\n\"\n");

    fprintf(out, ".text\n");
    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");

    fprintf(out, "\tpushq\t%%rbp\n");
    fprintf(out, "\tmovq\t%%rsp, %%rbp\n");

    for (int i = 0; i < root->data.stmt_list.count; i++) {
        codegen_stmt(root->data.stmt_list.statements[i], out);
    }

    if (g_current_stack_offset != 0) {
        int stack_size = (abs(g_current_stack_offset) + 15) & ~15;
        fprintf(out, "\tsubq\t$%d, %%rsp\n", stack_size);
    }

    fprintf(out, "\tmovl\t$0, %%eax\n");
    fprintf(out, "\tleave\n");
    fprintf(out, "\tret\n");

    free_symbol_table();
    fclose(out);
}

// --- Simple Three-Address Code (TAC) Generator ---
// We'll emit TAC lines like: t1 = 2
//                          t2 = t1 + 3
//                          a = t2
//                          print t2

static int tac_temp_counter = 0;

static char *new_temp() {
    tac_temp_counter++;
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", tac_temp_counter);
    return strdup(buf);
}

// Emit TAC for expression and return a string naming the place (temp or var or literal)
static char *tac_emit_expr(ASTNode *node, FILE *out) {
    if (!node) return strdup("0");

    switch (node->type) {
        case NODE_INT_LITERAL: {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", node->data.int_literal.value);
            return strdup(buf);
        }
        case NODE_IDENTIFIER: {
            return strdup(node->data.identifier.name);
        }
        case NODE_BINOP: {
            char *left = tac_emit_expr(node->data.binop.left, out);
            char *right = tac_emit_expr(node->data.binop.right, out);
            char *t = new_temp();
            const char *opstr = "";
            switch (node->data.binop.op) {
                case OP_ADD: opstr = "+"; break;
                case OP_SUB: opstr = "-"; break;
                case OP_MUL: opstr = "*"; break;
                case OP_DIV: opstr = "/"; break;
                default: opstr = "?"; break;
            }
            fprintf(out, "%s = %s %s %s\n", t, left, opstr, right);
            free(left);
            free(right);
            return t;
        }
        default:
            return strdup("0");
    }
}

static void tac_emit_stmt(ASTNode *node, FILE *out) {
    if (!node) return;
    switch (node->type) {
        case NODE_ASSIGN: {
            char *r = tac_emit_expr(node->data.assign.expr, out);
            fprintf(out, "%s = %s\n", node->data.assign.id_name, r);
            free(r);
            break;
        }
        case NODE_PRINT: {
            char *r = tac_emit_expr(node->data.print.expr, out);
            fprintf(out, "print %s\n", r);
            free(r);
            break;
        }
        default:
            break;
    }
}

void generate_tac(ASTNode *root, const char *filename) {
    FILE *out = fopen(filename, "w");
    if (!out) { perror("Error opening TAC output file"); return; }

    if (!root || root->type != NODE_STMT_LIST) {
        fprintf(stderr, "TAC Error: AST root invalid.\n");
        fclose(out);
        return;
    }

    tac_temp_counter = 0;
    for (int i = 0; i < root->data.stmt_list.count; i++) {
        tac_emit_stmt(root->data.stmt_list.statements[i], out);
    }

    fclose(out);
}