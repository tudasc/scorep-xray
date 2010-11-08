## -*- mode: awk -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       scorep_meets_tau.awk
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


BEGIN{
    print_line = 0
}


/scorep_meets_tau_begin/ { print_line = 1; }


/scorep_meets_tau_end/ { print_line = 0; print "" }


{
    if (print_line == 2)
    {
        print $0
    }
    if (print_line == 1) # ignore the line containing scorep_meets_tau_begin
    {
        print_line = 2
    }
}

