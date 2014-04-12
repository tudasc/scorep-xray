
OBJ_DIR?=..
COMP = $(OBJ_DIR)/build-backend/scorep
INST = comp:gnu
COMPFLAGS = -c
INCLUDE=
OBJ=-Wl,--whole-archive \
    $(OBJ_DIR)/build-backend/.libs/libscorep_serial.a \
    $(OBJ_DIR)/vendor/otf2/build-backend/.libs/libotf2.a \
    $(OBJ_DIR)/vendor/otf2/vendor/utility/build-backend/.libs/libscorep_utilities.a \
    -Wl,--no-whole-archive \
    -lm -lbfd

hello:
	$(COMP) $(INST) $(INCLUDE) $(COMPFLAGS) hello_functions.c
	$(COMP) $(INST) $(OBJ) hello_functions.o -o hello

clean:
	-rm *.o hello


