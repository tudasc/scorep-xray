#include <config.h>
#include "pomp2_lib.h"

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(FORTRAN_MANGLED(pomp_tpd))
#endif

#define POMP2_DLIST_00001 shared(pomp_region_1,pomp_region_2)
static POMP2_Region_handle pomp_region_1;
static POMP2_Region_handle pomp_region_2;
#define POMP2_DLIST_00003 shared(pomp_region_3)
static POMP2_Region_handle pomp_region_3;

void POMP2_Init_regions_1269874420956379_3()
{
    POMP2_Assign_handle( &pomp_region_1, "62*regionType=parallel*sscl=jacobi.c:64:64*escl=jacobi.c:83:83**" );
    POMP2_Assign_handle( &pomp_region_2, "57*regionType=for*sscl=jacobi.c:67:67*escl=jacobi.c:82:82**" );
    POMP2_Assign_handle( &pomp_region_3, "69*regionType=parallelfor*sscl=jacobi.c:153:153*escl=jacobi.c:160:160**" );
}
