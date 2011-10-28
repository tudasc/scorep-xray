#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_131168713688096_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "159*regionType=parallel*sscl=/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c:49:49*escl=/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c:52:52**" );
}
