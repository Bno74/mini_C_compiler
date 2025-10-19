MiniC Compiler
Welcome to the MiniC Compiler project! This compiler takes MiniC source files (.min) as input and generates assembly code (.s) and three-address code (.tac). Below are the commands to build, run, and inspect the compiler's output.
Getting Started
Ensure you are in the project root directory before running these commands.
1. Build the Compiler
Compiles the compiler source code to create the minic executable.
make

2. Compile a MiniC Source File
Runs the MiniC compiler on a source file (e.g., sample.min) to produce:

Assembly code: output.s
Three-address code: output.tac

./minic sample.min

3. View Generated Assembly Code
Displays the first 200 lines of the generated assembly code (output.s) for inspection.
ls -l output.s
sed -n '1,200p' output.s

4. Assemble and Run the Generated Executable
Assembles the generated assembly code (output.s) into an executable and runs it.
gcc output.s -o output_exec
./output_exec

5. View Three-Address Code (TAC)
Displays the generated three-address code (output.tac) for debugging or analysis.
cat output.tac

6. Clean Build Artifacts
Removes generated files (e.g., executables, output.s, output.tac) to clean the project directory.
make clean

Notes

Ensure you have make, gcc, and sed installed on your system.
Replace sample.min with your MiniC source file.
Run all commands from the project root directory.

For issues or contributions, check the Issues section or submit a pull request!
