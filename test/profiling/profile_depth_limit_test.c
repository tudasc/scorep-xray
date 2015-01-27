/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @ file
 *
 * @brief Tests profile depth limitations.
 */

#include <config.h>

#include <scorep/SCOREP_User.h>

void
foo( int depth )
{
    SCOREP_USER_FUNC_BEGIN();

    if ( depth < 20 )
    {
        foo( depth + 1 );
    }

    SCOREP_USER_FUNC_END();
}

int
main()
{
    SCOREP_USER_FUNC_BEGIN();

    foo( 0 );

    SCOREP_USER_FUNC_END();
    return 0;
}
