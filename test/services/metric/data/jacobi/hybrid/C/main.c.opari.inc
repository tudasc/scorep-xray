#include <pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_1287563639864646_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "65*regionType=parallelfor*sscl=main.c:194:194*escl=main.c:210:210**" );
}
