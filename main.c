#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "codegen.h"

// Forward declaration of bison's parse function
extern int yyparse();
// Forward declaration of the AST root pointer from compiler.y
extern ASTNode *ast_root;
// Pointer to the input file
extern FILE *yyin;

// The main driver function
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the input file
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    printf("Starting compilation for input file: %s\n", argv[1]);

    // 1. Lexical Analysis and Parsing (AST Construction)
    if (yyparse() != 0) {
        fprintf(stderr, "Parsing failed.\n");
        fclose(yyin);
        return EXIT_FAILURE;
    }

    printf("Parsing successful. AST constructed.\n");

    // Close the input file
    fclose(yyin);

    // 2. Code Generation (AST Traversal and Assembly Output)
    const char *output_filename = "output.s";
    printf("Generating x86-64 assembly code to: %s\n", output_filename);
    generate_code(ast_root, output_filename);

    // Also generate three-address code
    const char *tac_filename = "output.tac";
    printf("Generating three-address code to: %s\n", tac_filename);
    generate_tac(ast_root, tac_filename);

    printf("Code generation complete.\n");

    // 3. Cleanup
    free_ast(ast_root);
    printf("Compilation finished successfully.\n");

    return EXIT_SUCCESS;
}