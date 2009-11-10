/** @file SILC_User_Region.c
    @author Daniel Lorenz

    This file containes the implementation of user adapter functions concerning
    regions.
 */

#include "SILC_User_Functions.h"
#include "SILC_Definitions.h"
#include "SILC_Events.h"
#include "SILC_Error.h"
#include "SILC_User_Init.h"
#include "SILC_Types.h"

/** @internal
    Translates the region type of the user adapter to the silc region types.
    The user adapter uses a bitvector for the type, silc has an enum. Where possible
    combinations are explicit.
    @param user_type The region type in the user adapter.
    @returns The region type in SILC measurement definitions. If the combination is
             invalid, an subset of the combinations is selected.
 */
SILC_RegionType
silc_user_to_silc_region_type( const SILC_User_RegionType user_type )
{
    switch ( user_type )
    {
        case 0:  // SILC_USER_REGION_TYPE_COMMON
            return SILC_REGION_USER;
        case 1:  // FUNCTION
            return SILC_REGION_FUNCTION;
        case 2:  // LOOP
            return SILC_REGION_LOOP;
        case 3:  // FUNCTION + LOOP -> Invalid -> use loop
            return SILC_REGION_LOOP;
        case 4:  // DYNAMIC
            return SILC_REGION_DYNAMIC;
        case 5:  // DYNAMIC + FUNCTION
            return SILC_REGION_DYNAMIC_FUNCTION;
        case 6:  // DYNAMIC + LOOP
            return SILC_REGION_DYNAMIC_LOOP;
        case 7:  // DYNAMIC + FUNCTION + LOOP -> Invalid -> use dynamic loop
            return SILC_REGION_DYNAMIC_LOOP;
        case 8:  // PHASE
            return SILC_REGION_PHASE;
        case 9:  // PHASE + FUNCTION -> use phase
            return SILC_REGION_PHASE;
        case 10: // PHASE + LOOP -> use phase
            return SILC_REGION_PHASE;
        case 11: // PHASE + FUNCTION + LOOP -> Invalid -> use phase
            return SILC_REGION_PHASE;
        case 12: // PHASE + DYNAMIC
            return SILC_REGION_DYNAMIC_PHASE;
        case 13: // PHASE + DYNAMIC + FUNCTION -> use dynamic phase
            return SILC_REGION_DYNAMIC_PHASE;
        case 14: // PHASE + DYNAMIC + LOOP
            return SILC_REGION_DYNAMIC_LOOP_PHASE;
        case 15: // PHASE + DYNAMIC + LOOP + FUNCTION -> Invalid -> use dynamic, phase, loop
            return SILC_REGION_DYNAMIC_LOOP_PHASE;
        default: // Not known
            return SILC_REGION_UNKNOWN;
    }
}

void
SILC_User_RegionBegin
(
    SILC_RegionHandle*         handle,
    SILC_SourceFileHandle*     file,
    const char*                name,
    const SILC_User_RegionType regionType,
    const char*                fileName,
    const uint32_t             lineNo
)
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED

    /* If the handle is invalid, then register the new region */
    if ( *handle == SILC_INVALID_REGION )
    {
        /* Traslate region type from user adapter type to SILC measurement type */
        SILC_RegionType region_type = silc_user_to_silc_region_type( regionType );

        /* register file if invalid */
        if ( *file == SILC_INVALID_SOURCE_FILE )
        {
            *file = SILC_DefineSourceFile( fileName );
        }

        /* Regiter new region */
        *handle = SILC_DefineRegion( name,
                                     *file,
                                     lineNo,
                                     SILC_INVALID_LINE_NO,
                                     SILC_ADAPTER_USER,
                                     region_type );
    }

    /* Generate region event */
    SILC_EnterRegion( *handle );
}


void
SILC_User_RegionEnd
(
    const SILC_RegionHandle handle
)
{
    /* Generate exit event */
    SILC_ExitRegion( handle );
}
