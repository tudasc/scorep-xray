#include <config.h>
#include "pomp2_lib.h"

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(FORTRAN_MANGLED(pomp_tpd))
#endif

#define POMP_DLIST_00001 shared( pomp_region_1 )
static POMP2_Region_handle pomp_region_1;

void POMP2_Init_regions_1274996139600418_1()
{
    POMP2_Assign_handle( &pomp_region_1, "66*regionType=parallel*sscl=omp_test.c:45:45*escl=omp_test.c:48:48**" );
}
