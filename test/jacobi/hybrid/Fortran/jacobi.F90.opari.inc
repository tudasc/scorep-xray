      INTEGER( KIND=8 ) pomp_region_1
      INTEGER( KIND=8 ) pomp_region_2
      INTEGER( KIND=8 ) pomp_region_3
      include "omp_lib.h"
      integer( kind=8 ) pomp_num_threads 
      integer( kind=8 ) pomp_tpd 
      common /pomp_tpd/ pomp_tpd 
!$omp threadprivate(/pomp_tpd/)
      common /cb1276000468650595/ pomp_region_1,&
              pomp_region_2,&
              pomp_region_3
