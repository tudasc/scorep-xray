#include <pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
extern "C" 
{
void POMP2_Init_regions_1287563699634359_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "69*regionType=parallelfor*sscl=main.cpp:185:185*escl=main.cpp:201:201**" );
}
}
