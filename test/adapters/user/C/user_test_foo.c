/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#include <config.h>

#include <scorep/SCOREP_User.h>

SCOREP_USER_METRIC_EXTERNAL( globalMetric )

void
foo()
{
    SCOREP_USER_REGION_DEFINE( foo_handle )
    SCOREP_USER_REGION_BEGIN( foo_handle, "foo", SCOREP_USER_REGION_TYPE_COMMON );
    SCOREP_USER_METRIC_INT64( globalMetric, 3 );
    SCOREP_USER_REGION_END( foo_handle );
}
