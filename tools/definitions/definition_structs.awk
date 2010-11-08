## -*- mode: awk -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
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

## file       definition_structs.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

{
    print "SCOREP_MOVABLE_TYPE( " $2 "_Definition );"
    print "struct " $2 "_Definition"
    print "{"
    print "    " $2 "_Definition_Movable next;"
    print "    uint32_t id;"
    print "    // Keep above order to be able to cast between definition types.
    print "    // Add " $2 " stuff from here on."
    print "};\n\n"
}
