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

## file       definition_structs.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

{
    print "SILC_MOVABLE_TYPE( " $2 "_Definition );"
    print "struct " $2 "_Definition"
    print "{"
    print "    " $2 "_Definition_Movable next;"
    print "    uint32_t id;"
    print "    // Keep order above! See struct silc_any_definition."
    print "    // Add " $2 " stuff from here on."
    print "};\n\n"
}
