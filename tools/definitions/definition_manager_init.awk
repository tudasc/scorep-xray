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

## file       definition_manager_init.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

BEGIN{
    print "SILC_DefinitionManager silc_definition_manager ="
    print "{"
    print "     { { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }"
}

{
    if (NR == 1)
    {
        next;
    }
    print "    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }"
}

END{
    print "};\n"
}

    
