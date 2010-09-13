#include <config.h>
#include "pomp2_lib.h"

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(FORTRAN_MANGLED(pomp_tpd))
#endif

extern "C"
{

#define POMP2_DLIST_00001 shared(pomp_region_1,pomp_region_2,pomp_region_3)
static POMP2_Region_handle pomp_region_1;
static POMP2_Region_handle pomp_region_2;
static POMP2_Region_handle pomp_region_3;

void POMP2_Init_regions_1269949776865275_3()
{
    POMP2_Assign_handle( &pomp_region_1, "66*regionType=parallel*sscl=jacobi.cpp:55:55*escl=jacobi.cpp:85:85**" );
    POMP2_Assign_handle( &pomp_region_2, "61*regionType=for*sscl=jacobi.cpp:57:57*escl=jacobi.cpp:64:64**" );
    POMP2_Assign_handle( &pomp_region_3, "61*regionType=for*sscl=jacobi.cpp:69:69*escl=jacobi.cpp:84:84**" );
}

}
