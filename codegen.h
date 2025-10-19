#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

// The main function to traverse the AST and generate assembly code.
// Writes the output to the specified filename (e.g., "output.s").
void generate_code(ASTNode *root, const char *filename);

// Generate three-address code (TAC) for the AST and write to filename (e.g. "output.tac").
void generate_tac(ASTNode *root, const char *filename);

#endif // CODEGEN_H