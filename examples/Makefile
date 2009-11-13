
COMP = ../build-backend/silc --instrument
INST = comp:gnu 
COMPFLAGS = -c 
INCLUDE=
OBJ=../build-backend/libsilc_comp_la-SILC_compiler_gnu.o

hello: 
	$(COMP) $(INST) $(INCLUDE) $(COMPFLAGS) hello_functions.c  
	$(COMP) $(INST) $(OBJ) hello_functions.o -o hello

clean:
	rm *.o hello


