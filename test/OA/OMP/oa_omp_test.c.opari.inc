#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
static const char pomp2_ctc_1[] = "173*regionType=task*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:33:33*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:36:36**";
static POMP2_Region_handle pomp2_region_2 = NULL;
static const char pomp2_ctc_2[] = "177*regionType=taskwait*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:38:38*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:38:38**";
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
static const char pomp2_ctc_3[] = "177*regionType=parallel*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:51:51*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:54:54**";
#define POMP2_DLIST_00004 shared(pomp2_region_4,pomp2_region_5)
static POMP2_Region_handle pomp2_region_4 = NULL;
static const char pomp2_ctc_4[] = "177*regionType=parallel*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:61:61*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:74:74**";
static POMP2_Region_handle pomp2_region_5 = NULL;
static const char pomp2_ctc_5[] = "175*regionType=single*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:71:71*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:72:72**";
#define POMP2_DLIST_00006 shared(pomp2_region_6,pomp2_region_7,pomp2_region_8,pomp2_region_9,pomp2_region_10,pomp2_region_11,pomp2_region_12)
static POMP2_Region_handle pomp2_region_6 = NULL;
static const char pomp2_ctc_6[] = "179*regionType=parallel*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:77:77*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:112:112**";
static POMP2_Region_handle pomp2_region_7 = NULL;
static const char pomp2_ctc_7[] = "172*regionType=for*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:80:80*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:84:84**";
static POMP2_Region_handle pomp2_region_8 = NULL;
static const char pomp2_ctc_8[] = "175*regionType=master*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:86:86*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:89:89**";
static POMP2_Region_handle pomp2_region_9 = NULL;
static const char pomp2_ctc_9[] = "176*regionType=barrier*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:91:91*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:91:91**";
static POMP2_Region_handle pomp2_region_10 = NULL;
static const char pomp2_ctc_10[] = "196*regionType=critical*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:93:93*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:96:96*criticalName=test1**";
static POMP2_Region_handle pomp2_region_11 = NULL;
static const char pomp2_ctc_11[] = "175*regionType=atomic*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:98:98*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:99:99**";
static POMP2_Region_handle pomp2_region_12 = NULL;
static const char pomp2_ctc_12[] = "195*regionType=sections*sscl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:101:101*escl=/home/oleynik/work/external_folder/trunk/test/OA/OMP/oa_omp_test.c:111:111*numSections=2**";

#ifdef __cplusplus 
 extern "C" 
#endif
void POMP2_Init_reg_1344359699524161_12()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
    POMP2_Assign_handle( &pomp2_region_4, pomp2_ctc_4 );
    POMP2_Assign_handle( &pomp2_region_5, pomp2_ctc_5 );
    POMP2_Assign_handle( &pomp2_region_6, pomp2_ctc_6 );
    POMP2_Assign_handle( &pomp2_region_7, pomp2_ctc_7 );
    POMP2_Assign_handle( &pomp2_region_8, pomp2_ctc_8 );
    POMP2_Assign_handle( &pomp2_region_9, pomp2_ctc_9 );
    POMP2_Assign_handle( &pomp2_region_10, pomp2_ctc_10 );
    POMP2_Assign_handle( &pomp2_region_11, pomp2_ctc_11 );
    POMP2_Assign_handle( &pomp2_region_12, pomp2_ctc_12 );
}
