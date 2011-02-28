#include <config.h>
#include <pomp2_lib.h>

#define POMP_TPD_MANGLED FORTRAN_MANGLED( pomp_tpd )
#define SCOREP_PRAGMA_STR_( str ) #str
#define SCOREP_PRAGMA_STR( str ) SCOREP_PRAGMA_STR_( str )
#define PRAGMA_OP( x ) _Pragma( x )
#define SCOREP_PRAGMA( pragma ) PRAGMA_OP( SCOREP_PRAGMA_STR( pragma ) )
#define SCOREP_PRAGMA_OMP( omp_pragma ) SCOREP_PRAGMA( omp omp_pragma )

extern int64_t FORTRAN_ALIGNED POMP_TPD_MANGLED;
SCOREP_PRAGMA_OMP( threadprivate( POMP_TPD_MANGLED ) )

#define POMP2_DLIST_00001 shared(pomp_region_1)
static POMP2_Region_handle pomp_region_1;

void POMP2_Init_regions_1275696211160436_1()
{
    POMP2_Assign_handle( &pomp_region_1, "74*regionType=parallel*sscl=mpi_omp_test.c:45:45*escl=mpi_omp_test.c:49:49**" );
}
