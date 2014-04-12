      INTEGER( KIND=8 ) :: pomp2_region_1

      CHARACTER (LEN=259), parameter :: pomp2_ctc_1 =&
      "255*regionType=parallel*sscl=jacobi.F90:58:58*"//&
      "escl=jacobi.F90:76:76**"

      INTEGER( KIND=8 ) :: pomp2_region_2

      CHARACTER (LEN=268), parameter :: pomp2_ctc_2 =&
      "264*regionType=do*sscl=jacobi.F90:59:59*"//&
      "escl=jacobi.F90:75:75*hasReduction=1**"

      INTEGER( KIND=8 ) :: pomp2_region_3

      CHARACTER (LEN=265), parameter :: pomp2_ctc_3 =&
      "261*regionType=paralleldo*sscl=jacobi.F90:138:138*"//&
      "escl=jacobi.F90:144:144**"


      integer ( kind=4 ) :: pomp2_lib_get_max_threads
      logical :: pomp2_test_lock
      integer ( kind=4 ) :: pomp2_test_nest_lock
      integer( kind=8 ) pomp_tpd
      common /pomp_tpd/ pomp_tpd
!$omp threadprivate(/pomp_tpd/)
      integer ( kind=8 ) :: pomp2_old_task, pomp2_new_task
      logical :: pomp2_if
      integer ( kind=4 ) :: pomp2_num_threads
      common /cbkh6l759tlp3ei/ pomp2_region_1,&
              pomp2_region_2,&
              pomp2_region_3
