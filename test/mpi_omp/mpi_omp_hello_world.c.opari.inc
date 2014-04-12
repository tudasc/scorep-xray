#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "249*regionType=parallel*sscl=mpi_omp_hello_world.c:49:49*escl=mpi_omp_hello_world.c:53:53**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_1m6la4etldzk5_1()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
}
