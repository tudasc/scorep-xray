#include <scorep/SCOREP_User.inc>

program oa_test


integer, volatile :: k
integer ierr

SCOREP_USER_REGION_DEFINE(mainRegion)

do k=1,30

SCOREP_USER_OA_PHASE_BEGIN(mainRegion,"mainRegion",SCOREP_USER_REGION_TYPE_COMMON)

SCOREP_USER_OA_PHASE_END(mainRegion)

enddo

end
