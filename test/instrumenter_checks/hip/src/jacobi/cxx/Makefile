## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2018, 2022,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       test/instrumenter_checks/hip/src/jacobi/cxx/Makefile


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

$(OBJDIR)/kernel.o: $(SRCDIR)/kernel.cpp $(SRCDIR)/definitions.h
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/definitions.h


#------------------------------------------------------------------------------
# Generic rules and variables, don't move above OBJECTS and LIBS
#------------------------------------------------------------------------------

include $(INSTRUMENTERCHECK_SRCDIR)/Makefile.common_rules_base

SCOREP_V_HIPCC = $(scorep__v_hipcc_$(V))
scorep__v_hipcc_ = $(scorep__v_hipcc_0)
scorep__v_hipcc_0 = @$(scorep__tty_colors); echo "  SCOREP $(SRP)HIPCC   $${blu}$@$$std";

SCOREP_V_HIPCCLD = $(scorep__v_hipccld_$(V))
scorep__v_hipccld_ = $(scorep__v_hipccld_0)
scorep__v_hipccld_0 = @$(scorep__tty_colors); echo "  SCOREP $(SRP)HIPCCLD $${grn}$@$$std";

$(BINDIR)/$(CODE)-$(PARADIGM)-cxx$(SUFFIX): $(OBJECTS) $(TOOLS)
	$(SCOREP_V_HIPCCLD)$(PREP) $(SCOREP_V_verbose) $(HIPCC) $(CXXFLAGS) -o $@ $(OBJECTS) $(LIBS) $(LIBS_INTERNAL)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(TOOLS)
	$(SCOREP_V_HIPCC)$(PREP) $(SCOREP_V_verbose) $(HIPCC) $(CXXFLAGS) -o $@ -c $<
