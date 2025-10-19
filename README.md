Quick commands (run in project root)
Build the compiler:


make
Compile a source file with the compiler (produces output.s and output.tac):


./minic sample.min
View the generated assembly:


ls -l output.ssed -n '1,200p' output.s
Assemble and link the generated assembly into an executable and run it:


gcc output.s -o output_exec./output_exec
View the generated three-address code (TAC):


cat output.tac
Clean build artifacts:
