#include <pomp2_lib.h>

extern void* pomp_tpd_;
#pragma omp threadprivate(pomp_tpd_)
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_1287563443944711_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "65*regionType=parallelfor*sscl=main.c:140:140*escl=main.c:156:156**" );
}
