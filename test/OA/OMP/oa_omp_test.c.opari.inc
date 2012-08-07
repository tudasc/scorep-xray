#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) pomp_tpd_;
#pragma omp threadprivate(pomp_tpd_)
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
#define POMP2_DLIST_00004 shared(pomp2_region_4,pomp2_region_5)
static POMP2_Region_handle pomp2_region_4 = NULL;
static POMP2_Region_handle pomp2_region_5 = NULL;
#define POMP2_DLIST_00006 shared(pomp2_region_6,pomp2_region_7,pomp2_region_8,pomp2_region_9,pomp2_region_10,pomp2_region_11,pomp2_region_12)
static POMP2_Region_handle pomp2_region_6 = NULL;
static POMP2_Region_handle pomp2_region_7 = NULL;
static POMP2_Region_handle pomp2_region_8 = NULL;
static POMP2_Region_handle pomp2_region_9 = NULL;
static POMP2_Region_handle pomp2_region_10 = NULL;
static POMP2_Region_handle pomp2_region_11 = NULL;
static POMP2_Region_handle pomp2_region_12 = NULL;

void POMP2_Init_regions_1337934361381170_12()
{
    POMP2_Assign_handle( &pomp2_region_1, "207*regionType=task*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:32:32*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:35:35**" );
    POMP2_Assign_handle( &pomp2_region_2, "209*regionType=taskwait*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:37:37*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**" );
    POMP2_Assign_handle( &pomp2_region_3, "211*regionType=parallel*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:50:50*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:53:53**" );
    POMP2_Assign_handle( &pomp2_region_4, "211*regionType=parallel*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:60:60*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:73:73**" );
    POMP2_Assign_handle( &pomp2_region_5, "209*regionType=single*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:70:70*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:71:71**" );
    POMP2_Assign_handle( &pomp2_region_6, "213*regionType=parallel*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:76:76*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:111:111**" );
    POMP2_Assign_handle( &pomp2_region_7, "206*regionType=for*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:79:79*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:83:83**" );
    POMP2_Assign_handle( &pomp2_region_8, "209*regionType=master*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:85:85*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:88:88**" );
    POMP2_Assign_handle( &pomp2_region_9, "208*regionType=barrier*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:90:90*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**" );
    POMP2_Assign_handle( &pomp2_region_10, "230*regionType=critical*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:92:92*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:95:95*criticalName=test1**" );
    POMP2_Assign_handle( &pomp2_region_11, "209*regionType=atomic*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:97:97*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:98:98**" );
    POMP2_Assign_handle( &pomp2_region_12, "229*regionType=sections*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:100:100*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:110:110*numSections=2**" );
}
