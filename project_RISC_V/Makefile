cc = g++ -I.
cf = -w
cc2 = g++ -c -I.

all : libriscv_asm

libriscv_asm : parser.o variables.o helper_functions.o assembler.o
	ar rcs libriscv_asm.a parser.o variables.o helper_functions.o assembler.o
	@echo -e "Created static library"

riscv_asm : parser.o variables.o main.o helper_functions.o assembler.o
	$(cc) $(cf) parser.o variables.o main.o helper_functions.o assembler.o -o riscv_asm
	@echo -e " Linked Everything"

parser.o : parser.cpp 
	$(cc2) parser.cpp -o parser.o

variables.o : variables.cpp
	$(cc2) variables.cpp -o variables.o

main.o : main.cpp
	$(cc2) main.cpp -o main.o

helper_functions.o : helper_functions.cpp
	$(cc2) helper_functions.cpp -o helper_functions.o

assembler.o : assembler.cpp
	$(cc2) assembler.cpp -o assembler.o

clean:
	@echo "cleaning everything.."
	rm -f *.o riscv_asm
	rm -f *.o libriscv_asm.a
