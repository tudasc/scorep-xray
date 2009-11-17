
COMP = ../build-backend/silc --instrument
INST = comp:gnu 
COMPFLAGS = -c 
INCLUDE=
OBJ=-Wl,--whole-archive ../build-backend/.libs/libsilc.a -Wl,--no-whole-archive \
    ../vendor/otf2/vendor/utility/build-backend/.libs/libsilc_utilities.a -lm

hello: 
	$(COMP) $(INST) $(INCLUDE) $(COMPFLAGS) hello_functions.c  
	$(COMP) $(INST) $(OBJ) hello_functions.o -o hello

clean:
	-rm *.o hello


