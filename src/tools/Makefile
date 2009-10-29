COMP= g++ 
COMP_ARG= -c
INCLUDE=-I ../../vendor/utility/src -I ../../vendor/utility/src/exception

clean:
	rm -rf  *.o

all:
	$(COMP) $(COMP_ARG) $(INCLUDE) *.cpp 
	$(COMP) $(INCLUDE) *.o -o silc
