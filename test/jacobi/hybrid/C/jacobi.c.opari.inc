#include "pomp_lib.h"

#define POMP_DLIST_00001 shared(pomp_region_1,pomp_region_2)
static POMP_Region_handle pomp_region_1;
static POMP_Region_handle pomp_region_2;
#define POMP_DLIST_00003 shared(pomp_region_3)
static POMP_Region_handle pomp_region_3;

void POMP_Init_regions_1269874420956379_3()
{
    POMP_Assign_handle( &pomp_region_1, "62*regionType=parallel*sscl=jacobi.c:64:64*escl=jacobi.c:83:83**" );
    POMP_Assign_handle( &pomp_region_2, "57*regionType=for*sscl=jacobi.c:67:67*escl=jacobi.c:82:82**" );
    POMP_Assign_handle( &pomp_region_3, "69*regionType=parallelfor*sscl=jacobi.c:153:153*escl=jacobi.c:160:160**" );
}
