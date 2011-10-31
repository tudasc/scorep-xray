      INTEGER( KIND=8 ) :: pomp2_region_1
      INTEGER( KIND=8 ) :: pomp2_region_2
      INTEGER( KIND=8 ) :: pomp2_region_3
      integer pomp_get_max_threads1320070372569861
      integer( kind=8 ) pomp_tpd 
      common /pomp_tpd/ pomp_tpd 
!$omp threadprivate(/pomp_tpd/)
      integer ( kind=8 ) :: pomp2_old_task, pomp2_new_task 
      logical :: pomp_if 
      integer pomp_num_threads 
      common /cb1320070372569861/ pomp2_region_1,&
              pomp2_region_2,&
              pomp2_region_3
