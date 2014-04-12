      INTEGER( KIND=8 ) :: pomp2_region_1

      CHARACTER (LEN=259), parameter :: pomp2_ctc_1 =&
      "255*regionType=parallel*sscl=jacobi.F90:56:56*escl=jacobi.F90:81:81**"

      INTEGER( KIND=8 ) :: pomp2_region_2

      CHARACTER (LEN=253), parameter :: pomp2_ctc_2 =&
      "249*regionType=do*sscl=jacobi.F90:57:57*escl=jacobi.F90:63:63**"

      INTEGER( KIND=8 ) :: pomp2_region_3

      CHARACTER (LEN=268), parameter :: pomp2_ctc_3 =&
      "264*regionType=do*sscl=jacobi.F90:64:64*"//&
      "escl=jacobi.F90:80:80*hasReduction=1**"


      integer ( kind=4 ) :: pomp2_lib_get_max_threads
      logical :: pomp2_test_lock
      integer ( kind=4 ) :: pomp2_test_nest_lock
      integer( kind=8 ) pomp_tpd
      common /pomp_tpd/ pomp_tpd
!$omp threadprivate(/pomp_tpd/)
      integer ( kind=8 ) :: pomp2_old_task, pomp2_new_task
      logical :: pomp2_if
      integer ( kind=4 ) :: pomp2_num_threads
      common /cb8j6lstatlxpyf/ pomp2_region_1,&
              pomp2_region_2,&
              pomp2_region_3
