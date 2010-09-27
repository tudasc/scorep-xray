## -*- mode: awk -*-

## 
## This file is part of the SCOREP project (http://www.scorep.de)
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

## file       definition_manager.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

BEGIN{
    print "typedef struct SCOREP_DefinitionManager SCOREP_DefinitionManager;"
    print "struct SCOREP_DefinitionManager"
    print "{"
}

{
    print "    " $2 "_Definition " $3 "_definitions_head_dummy;"
}

END{
    print "};\n"
}
