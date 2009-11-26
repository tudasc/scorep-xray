/** @file SILC_User_Control.c
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de
    @brief Contain the implementation of the control functions for C.
 */

#include "SILC_User_Functions.h"
#include "SILC_User_Init.h"
#include "SILC_RuntimeManagement.h"

void
SILC_User_EnableRecording()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SILC_EnableRecording();
}

void
SILC_User_DisableRecording()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SILC_DisableRecording();
}

bool
SILC_User_RecordingEnabled()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* Return value */
    return SILC_RecordingEnabled();
}
