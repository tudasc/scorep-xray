#include "pomp_lib.h"

#define POMP_DLIST_00001 shared(pomp_region_1,pomp_region_2,pomp_region_3,pomp_region_4,pomp_region_5,pomp_region_6,pomp_region_7)
static POMP_Region_handle pomp_region_1;
static POMP_Region_handle pomp_region_2;
static POMP_Region_handle pomp_region_3;
static POMP_Region_handle pomp_region_4;
static POMP_Region_handle pomp_region_5;
static POMP_Region_handle pomp_region_6;
static POMP_Region_handle pomp_region_7;

void POMP_Init_regions_1263287966599960_7()
{
    POMP_Assign_handle( &pomp_region_1, "66*regionType=parallel*sscl=pomp_test.c:6:6*escl=pomp_test.c:41:41**" );
    POMP_Assign_handle( &pomp_region_2, "61*regionType=for*sscl=pomp_test.c:9:9*escl=pomp_test.c:13:13**" );
    POMP_Assign_handle( &pomp_region_3, "66*regionType=master*sscl=pomp_test.c:15:15*escl=pomp_test.c:18:18**" );
    POMP_Assign_handle( &pomp_region_4, "65*regionType=barrier*sscl=pomp_test.c:20:20*escl=pomp_test.c:0:0**" );
    POMP_Assign_handle( &pomp_region_5, "68*regionType=critical*sscl=pomp_test.c:22:22*escl=pomp_test.c:25:25**" );
    POMP_Assign_handle( &pomp_region_6, "66*regionType=atomic*sscl=pomp_test.c:27:27*escl=pomp_test.c:28:28**" );
    POMP_Assign_handle( &pomp_region_7, "82*regionType=sections*sscl=pomp_test.c:30:30*escl=pomp_test.c:40:40*numSections=2**" );
}
