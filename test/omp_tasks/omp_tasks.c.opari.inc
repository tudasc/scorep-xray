#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "229*regionType=task*sscl=omp_tasks.c:48:48*escl=omp_tasks.c:51:51**"
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "233*regionType=taskwait*sscl=omp_tasks.c:53:53*escl=omp_tasks.c:53:53**"
#define POMP2_DLIST_00003 shared(pomp2_region_3,pomp2_region_4)
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "233*regionType=parallel*sscl=omp_tasks.c:62:62*escl=omp_tasks.c:66:66**"
static POMP2_Region_handle pomp2_region_4 = NULL;
#define pomp2_ctc_4 "231*regionType=single*sscl=omp_tasks.c:64:64*escl=omp_tasks.c:65:65**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_jl6l9ddtlx35j_4()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
    POMP2_Assign_handle( &pomp2_region_4, pomp2_ctc_4 );
}
