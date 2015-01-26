/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015
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
 */



/**
 * @file
 *
 *
 */


#include <config.h>
#include <stdio.h>
#include <scorep/SCOREP_User.h>

void
filtered2();
void
filter_not2();

void
filtered1()
{
    SCOREP_USER_FUNC_BEGIN();
    printf( "%s\n", "1" );
    SCOREP_USER_FUNC_END();
}


void
filter_not1()
{
    SCOREP_USER_FUNC_BEGIN();
    printf( "%s\n", "4" );
    SCOREP_USER_FUNC_END();
}

static void
enter_by_name( const char* name )
{
    SCOREP_USER_REGION_BY_NAME_BEGIN( name, SCOREP_USER_REGION_TYPE_COMMON )
}

static void
exit_by_name( const char* name )
{
    SCOREP_USER_REGION_BY_NAME_END( name )
}

static void
by_name( const char* name,
         const char* value )
{
    enter_by_name( name );
    printf( "%s\n", value );
    exit_by_name( name );
}

int
main()
{
    SCOREP_USER_FUNC_BEGIN();
    printf( "Run filter test:\n" );
    filtered1();
    filtered2();
    by_name( "filtered3", "3" );
    filter_not1();
    filter_not2();
    by_name( "filter_not3", "6" );
    SCOREP_USER_FUNC_END();
    return 0;
}
