#include <config.h>
#include "pomp2_lib.h"

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(FORTRAN_MANGLED(pomp_tpd))
#endif

#define POMP2_DLIST_00001 shared(pomp_region_1)
static POMP2_Region_handle pomp_region_1;

void POMP2_Init_regions_1269874414236437_1()
{
    POMP2_Assign_handle( &pomp_region_1, "65*regionType=parallelfor*sscl=main.c:177:177*escl=main.c:193:193**" );
}
