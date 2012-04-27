## -*- mode: makefile -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       test/installcheck/serial/src/jacobi/cc/Makefile
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


#------------------------------------------------------------------------------
# Testcase specific variables and rules
#------------------------------------------------------------------------------

OBJECTS = \
	$(OBJDIR)/jacobi.o \
	$(OBJDIR)/main.o

LIBS = -lm

all:


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

$(OBJDIR)/jacobi.o: jacobi.c jacobi.h
$(OBJDIR)/main.o: main.c jacobi.h


#------------------------------------------------------------------------------
# Generic rules and variables, don't move above OBJECTS and LIBS
#------------------------------------------------------------------------------

include $(INCDIR)/Makefile
