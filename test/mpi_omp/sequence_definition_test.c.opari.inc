#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(opari2_region_1)
static OPARI2_Region_handle opari2_region_1 = NULL;
#define opari2_ctc_1 "196*regionType=parallel*sscl=/home/lorenz/src/scorep/incubation/test/mpi_omp/sequence_definition_test.c:31:31*escl=/home/lorenz/src/scorep/incubation/test/mpi_omp/sequence_definition_test.c:36:36**"

#ifdef __cplusplus 
 extern "C" 
#endif
void POMP2_Init_reg_yzn8k655ma1pr3_1()
{
    POMP2_Assign_handle( &opari2_region_1, opari2_ctc_1 );
}
