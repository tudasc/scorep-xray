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

## file       definition_handles.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

{
    print "typedef struct " $2 "_Definition " $2 "_Definition;"
    print "typedef struct " $2 "_Definition_Movable " $2 "_Definition_Movable;"
    print "typedef " $2 "_Definition_Movable* " $2 "Handle;\n\n" 
}

