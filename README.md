# MiniC Compiler

Welcome to the MiniC Compiler, a lightweight compiler for the MiniC language! This project takes MiniC source files (.min) as input and generates assembly code (.s) and three-address code (.tac). It’s designed for educational purposes and compiler design enthusiasts.
Table of Contents

# Features
Prerequisites
Getting Started
Build the Compiler
Compile a Source File
View Generated Assembly
Run the Executable
View Three-Address Code
Clean Build Artifacts


Usage Example
Contributing
License

# Features

Compiles MiniC source code to assembly and TAC.
Supports basic arithmetic expressions and variable assignments.
Generates x86-64 assembly code for Linux.
Includes common subexpression elimination (CSE) for optimized TAC.
Easy-to-use command-line interface.

# Prerequisites
Ensure you have the following tools installed:

gcc (GNU Compiler Collection)
make (GNU Make)
sed (Stream Editor)
A Unix-like environment (Linux, macOS, or WSL on Windows)

Getting Started
All commands should be run from the project root directory.
Build the Compiler
Compile the MiniC compiler source code to create the minic executable.
make

Compile a Source File
Run the compiler on a MiniC source file (e.g., sample.min) to generate:

Assembly code:
```bash
output.s
```
Three-address code: 
```bash
output.tac
```
```bash
./minic sample.min
```

# View Generated Assembly
Inspect the first 200 lines of the generated assembly code (output.s).
```bash
ls -l output.s
```
```bash
sed -n '1,200p' output.s
```

# Run the Executable
Assemble the generated assembly code into an executable and run it.
```bash
gcc output.s -o output_exec
./output_exec
```

# View Three-Address Code
Display the generated TAC (output.tac) for debugging or analysis.
cat output.tac

# Clean Build Artifacts
Remove generated files (e.g., minic, output.s, output.tac) to clean the project directory.
```bash
make clean
```

# Usage Example

Create a sample.min file:a = 2 + 3;
b = a + 3;
c = b + 4;
print c;


Build and run:
```bash
make
./minic sample.min
cat output.tac
gcc output
```

