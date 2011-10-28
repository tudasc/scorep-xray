#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3,pomp2_region_4,pomp2_region_5,pomp2_region_6,pomp2_region_7)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;
static POMP2_Region_handle pomp2_region_4 = NULL;
static POMP2_Region_handle pomp2_region_5 = NULL;
static POMP2_Region_handle pomp2_region_6 = NULL;
static POMP2_Region_handle pomp2_region_7 = NULL;

void POMP2_Init_regions_1311686943562222_7()
{
    POMP2_Assign_handle( &pomp2_region_1, "181*regionType=parallel*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:26:26*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:61:61**" );
    POMP2_Assign_handle( &pomp2_region_2, "176*regionType=for*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:29:29*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:33:33**" );
    POMP2_Assign_handle( &pomp2_region_3, "179*regionType=master*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:35:35*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:38:38**" );
    POMP2_Assign_handle( &pomp2_region_4, "178*regionType=barrier*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:40:40*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0**" );
    POMP2_Assign_handle( &pomp2_region_5, "200*regionType=critical*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:42:42*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:45:45*criticalName=test1**" );
    POMP2_Assign_handle( &pomp2_region_6, "179*regionType=atomic*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:47:47*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:48:48**" );
    POMP2_Assign_handle( &pomp2_region_7, "195*regionType=sections*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:50:50*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:60:60*numSections=2**" );
}
