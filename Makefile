# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS =

# Output executable name
TARGET = minic

# Source files
C_SOURCES = main.c ast.c codegen.c
# Generated source files
GEN_SOURCES = lex.yy.c compiler.tab.c

# Object files
OBJECTS = $(C_SOURCES:.c=.o) $(GEN_SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

# Rule for the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

# Rule for the Bison parser (generates compiler.tab.c AND compiler.tab.h)
compiler.tab.c compiler.tab.h: compiler.y
	bison -d compiler.y -o compiler.tab.c

# Rule for the Flex lexer. MUST depend on compiler.tab.h to ensure tokens are defined.
lex.yy.c: compiler.l compiler.tab.h
	flex compiler.l

# Generic rule for compiling C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Explicit dependency for the lexer object file
lex.yy.o: lex.yy.c compiler.tab.h

# Cleanup rule
clean:
	rm -f $(TARGET) $(OBJECTS) $(GEN_SOURCES) compiler.tab.h output.s