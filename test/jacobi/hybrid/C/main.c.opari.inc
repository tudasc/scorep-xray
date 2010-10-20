#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_1287559963395350_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "65*regionType=parallelfor*sscl=main.c:194:194*escl=main.c:210:210**" );
}
