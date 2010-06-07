#include "pomp_lib.h"

#define POMP_DLIST_00001 shared(pomp_region_1)
static POMP_Region_handle pomp_region_1;

void POMP_Init_regions_1275696211160436_1()
{
    POMP_Assign_handle( &pomp_region_1, "74*regionType=parallel*sscl=mpi_omp_test.c:45:45*escl=mpi_omp_test.c:49:49**" );
}
