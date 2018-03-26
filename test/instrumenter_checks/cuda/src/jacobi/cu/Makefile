## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2018,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       test/instrumenter_checks/cuda/src/jacobi/cu/Makefile


#------------------------------------------------------------------------------
# Testcase specific variables and rules
#------------------------------------------------------------------------------

OBJECTS = \
	$(OBJDIR)/kernel.o \
	$(OBJDIR)/main.o

LIBS_INTERNAL = -lm

all:


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

$(OBJDIR)/kernel.o: $(SRCDIR)/kernel.cu $(SRCDIR)/definitions.cuh
$(OBJDIR)/main.o: $(SRCDIR)/main.cu $(SRCDIR)/definitions.cuh


#------------------------------------------------------------------------------
# Generic rules and variables, don't move above OBJECTS and LIBS
#------------------------------------------------------------------------------

include $(INCDIR)/Makefile.common_rules
