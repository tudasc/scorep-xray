## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

ALL_PLUGINS = \
    libHelloWorld.so

all: $(ALL_PLUGINS)

libHelloWorld.so.o: hello_world.c
	gcc -c -fPIC hello_world.c   \
            -o libHelloWorld.so.o  \
            `scorep-config --cppflags`

libHelloWorld.so: libHelloWorld.so.o
	gcc -shared -Wl,-soname,libHelloWorld.so  \
            -o libHelloWorld.so libHelloWorld.so.o

clean:
	$(RM) $(ALL_PLUGINS) *.o
