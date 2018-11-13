      INTEGER( KIND=8 ) :: opari2_region_1

      CHARACTER (LEN=201), parameter :: opari2_ctc_1 =&
      "198*regionType=task*sscl=/home/roessel/work/scorep/src/tr"//&
      "unk/test/omp_tasks/omp_tasks_untied.f90:13:13*escl=/home"//&
      "/roessel/work/scorep/src/trunk/test/omp_tasks/omp_tasks_"//&
      "untied.f90:15:15*hasUntied=1**"

      INTEGER( KIND=8 ) :: opari2_region_2

      CHARACTER (LEN=193), parameter :: opari2_ctc_2 =&
      "190*regionType=taskwait*sscl=/home/roessel/work/scorep/sr"//&
      "c/trunk/test/omp_tasks/omp_tasks_untied.f90:16:16*escl=/"//&
      "home/roessel/work/scorep/src/trunk/test/omp_tasks/omp_ta"//&
      "sks_untied.f90:16:16**"

      INTEGER( KIND=8 ) :: opari2_region_3

      CHARACTER (LEN=193), parameter :: opari2_ctc_3 =&
      "190*regionType=parallel*sscl=/home/roessel/work/scorep/sr"//&
      "c/trunk/test/omp_tasks/omp_tasks_untied.f90:21:21*escl=/"//&
      "home/roessel/work/scorep/src/trunk/test/omp_tasks/omp_ta"//&
      "sks_untied.f90:25:25**"

      INTEGER( KIND=8 ) :: opari2_region_4

      CHARACTER (LEN=191), parameter :: opari2_ctc_4 =&
      "188*regionType=single*sscl=/home/roessel/work/scorep/src/"//&
      "trunk/test/omp_tasks/omp_tasks_untied.f90:22:22*escl=/ho"//&
      "me/roessel/work/scorep/src/trunk/test/omp_tasks/omp_task"//&
      "s_untied.f90:24:24**"

      common /cbsjwin65ixa08/ opari2_region_1,&
              opari2_region_2,&
              opari2_region_3,&
              opari2_region_4

      integer ( kind=4 ) :: pomp2_lib_get_max_threads
      logical :: pomp2_test_lock
      integer ( kind=4 ) :: pomp2_test_nest_lock
      integer ( kind=8 ) :: pomp2_old_task, pomp2_new_task 
      logical :: pomp2_if 
      integer ( kind=4 ) :: pomp2_num_threads 
