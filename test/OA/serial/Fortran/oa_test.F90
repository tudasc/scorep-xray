#ifndef SCOREP_COMPILER_PGI
#include <scorep/SCOREP_User.inc>
#else
#include <scorep/SCOREP_User_Pgi.inc>
#endif

program oa_test


integer, volatile :: k
integer ierr

SCOREP_USER_REGION_DEFINE(mainRegion)

do k=1,30

SCOREP_USER_OA_PHASE_BEGIN(mainRegion,"mainRegion",SCOREP_USER_REGION_TYPE_COMMON)

SCOREP_USER_OA_PHASE_END(mainRegion)

enddo

end
