/** @file SILC_User_Control.c
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de
    @brief Contain the implementation of the control functions for Fortran.
 */

#include "SILC_User_Init.h"
#include "SILC_RuntimeManagement.h"
#include "SILC_Fortran_Wrapper.h"

#define SILC_User_EnableRecordingF_U  SILC_USER_ENABLERECORDINGF
#define SILC_User_DisableRecordingF_U SILC_USER_DISABLERECORDINGF
#define SILC_User_RecordingEnabledF_U SILC_USER_RECORDINGENABLEDF
#define SILC_User_EnableRecordingF_L  silc_user_enablerecordingf
#define SILC_User_DisableRecordingF_L silc_user_disablerecordingf
#define SILC_User_RecordingEnabledF_L silc_user_recordingenabledf

void
FSUB( SILC_User_EnableRecordingF ) ()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SILC_EnableRecording();
}

void
FSUB( SILC_User_DisableRecordingF ) ()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SILC_DisableRecording();
}

void
FSUB( SILC_User_RecordingEnabledF ) ( int* enabled )
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* Return value */
    *enabled = ( int )SILC_RecordingEnabled();
}
