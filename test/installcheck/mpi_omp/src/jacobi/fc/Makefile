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

## file       test/installcheck/serial/src/jacobi/fc/Makefile
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


#------------------------------------------------------------------------------
# Testcase specific variables and rules
#------------------------------------------------------------------------------

OBJECTS = \
	$(OBJDIR)/VariableDef.o \
	$(OBJDIR)/jacobi.o \
	$(OBJDIR)/main.o

LIBS =

all: 


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

$(OBJDIR)/jacobi.o: jacobi.F90
$(OBJDIR)/main.o: main.F90
$(OBJDIR)/VariableDef.o: VariableDef.f90


#------------------------------------------------------------------------------
# Generic rules and variables, don't move above OBJECTS and LIBS
#------------------------------------------------------------------------------

include $(INCDIR)/Makefile
