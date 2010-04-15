## -*- mode: awk -*-

## 
## This file is part of the SILC project (http://www.silc.de)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene USA
##    Forschungszentrum Juelich GmbH, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       define_functions.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

{
    print "    static uint32_t counter = 0;"
    print "    " $2 "_Definition*         new_definition = 0;"
    print "    " $2 "_Definition_Movable* new_movable    = 0;"
    print "    SILC_ALLOC_NEW_DEFINITION( " $2 "_Definition );"
    print "    SILC_DEFINITIONS_LIST_PUSH_FRONT( " $3 "_definitions_head_dummy );"
    print "    // Init new_definition"
    print "    new_definition->id = counter++;"
    print "    return new_movable;\n\n"
}
