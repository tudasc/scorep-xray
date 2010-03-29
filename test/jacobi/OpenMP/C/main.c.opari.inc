#include "pomp_lib.h"

#define POMP_DLIST_00001 shared(pomp_region_1)
static POMP_Region_handle pomp_region_1;

void POMP_Init_regions_1269863448102139_1()
{
    POMP_Assign_handle( &pomp_region_1, "65*regionType=parallelfor*sscl=main.c:123:123*escl=main.c:139:139**" );
}
