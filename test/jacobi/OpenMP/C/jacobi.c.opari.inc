#include <config.h>
#include "pomp2_lib.h"

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(FORTRAN_MANGLED(pomp_tpd))
#endif

#define POMP2_DLIST_00001 shared(pomp_region_1,pomp_region_2,pomp_region_3)
static POMP2_Region_handle pomp_region_1;
static POMP2_Region_handle pomp_region_2;
static POMP2_Region_handle pomp_region_3;

void POMP2_Init_regions_1269863383320327_3()
{
    POMP2_Assign_handle( &pomp_region_1, "62*regionType=parallel*sscl=jacobi.c:58:58*escl=jacobi.c:87:87**" );
    POMP2_Assign_handle( &pomp_region_2, "57*regionType=for*sscl=jacobi.c:60:60*escl=jacobi.c:67:67**" );
    POMP2_Assign_handle( &pomp_region_3, "57*regionType=for*sscl=jacobi.c:71:71*escl=jacobi.c:86:86**" );
}
