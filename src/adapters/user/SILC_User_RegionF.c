/** @file SILC_User_Region.c
    @author Daniel Lorenz

    This file containes the implementation of user adapter functions concerning
    regions.
 */

#include "SILC_User_Functions.h"
#include "SILC_Types.h"
#include "SILC_Utils.h"
#include "SILC_Error.h"
#include "SILC_Fortran_Wrapper.h"

#define SILC_User_RegionBeginF_U SILC_USER_REGIONBEGINF
#define SILC_User_RegionEndF_U SILC_USER_REGIONENDF

#define SILC_User_RegionBeginF_L silc_user_regionbeginf
#define SILC_User_RegionEndF_L silc_user_regionendf

void
FSUB( SILC_User_RegionBeginF ) ( int32_t * handle,
                                 char*    name,
                                 int32_t * type,
                                 char*    fileName,
                                 int32_t * lineNo )
{
    SILC_User_RegionBegin( handle, NULL, NULL, name, *type, fileName, *lineNo );
}

void
FSUB( SILC_User_RegionEndF ) ( int32_t * handle )
{
    SILC_User_RegionEnd( *handle );
}
