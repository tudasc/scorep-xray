/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#include <config.h>

#include "SILC_User.h"

SILC_USER_METRIC_GROUP_EXTERNAL( globalGroup )
SILC_USER_METRIC_EXTERNAL( globalMetric )

void
foo()
{
    SILC_USER_REGION_DEFINE( foo_handle )
    SILC_USER_REGION_BEGIN( foo_handle, "foo", SILC_USER_REGION_TYPE_COMMON );
    SILC_USER_METRIC_INT64( globalMetric, 3 );
    SILC_USER_REGION_END( foo_handle );
}
