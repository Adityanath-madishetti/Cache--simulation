cc = g++
cf = -w
cc2 = g++ -c

all:  riscv_sim 

 riscv_sim: main.o memoryclass.o conversionspace.o Helper.o encoder_functions.o extras.o  libriscv_asm 
	$(cc) $(cf) main.o memoryclass.o  conversionspace.o Helper.o encoder_functions.o extras.o -L./project_RISC_V -lriscv_asm  -o riscv_sim


main.o: main.cpp resources.hh
	$(cc2) main.cpp -o main.o

memoryclass.o: memoryclass.cpp  resources.hh
	$(cc2) memoryclass.cpp -o memoryclass.o

conversionspace.o: conversionspace.cpp resources.hh
	$(cc2) conversionspace.cpp -o conversionspace.o

Helper.o: Helper.cpp  resources.hh
	$(cc2) Helper.cpp -o Helper.o

encoder_functions.o: encoder_functions.cpp resources.hh
	$(cc2) encoder_functions.cpp -o encoder_functions.o

extras.o: extras.cpp resources.hh
	$(cc2) extras.cpp -o extras.o

libriscv_asm:
	$(MAKE) -C project_RISC_V

clean:
	@echo "Cleaning B's build..."
	rm -f *.o riscv_sim
	$(MAKE) -C project_RISC_V clean 

clean2:
	rm -f *.o


