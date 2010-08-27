/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SILC_POMP_FORTRAN_H
#define SILC_POMP_FORTRAN_H

/**
 * @file       SILC_Pomp_Fortran.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP
 *
 * @brief Provides defines for function name decoration for POMP Fortran wrappers.
 */

#include <pomp_lib.h>

#include "SILC_Fortran_Wrapper.h"

/* Fortran types */
typedef uint64_t POMP_Region_handle_fortran;

#define SILC_POMP_F2C_REGION( handle ) ( ( POMP_Region_handle* )SILC_F2C_POINTER( handle ) )
#define SILC_POMP_C2F_REGION( handle ) SILC_C2F_POINTER( handle )

/*
 * Fortan subroutine external name setup
 */

#define POMP_Finalize_U         POMP_FINALIZE
#define POMP_Init_U             POMP_INIT
#define POMP_Off_U              POMP_OFF
#define POMP_On_U               POMP_ON
#define POMP_Atomic_enter_U     POMP_ATOMIC_ENTER
#define POMP_Atomic_exit_U      POMP_ATOMIC_EXIT
#define POMP_Barrier_enter_U    POMP_BARRIER_ENTER
#define POMP_Barrier_exit_U     POMP_BARRIER_EXIT
#define POMP_Flush_enter_U      POMP_FLUSH_ENTER
#define POMP_Flush_exit_U       POMP_FLUSH_EXIT
#define POMP_Critical_begin_U   POMP_CRITICAL_BEGIN
#define POMP_Critical_end_U     POMP_CRITICAL_END
#define POMP_Critical_enter_U   POMP_CRITICAL_ENTER
#define POMP_Critical_exit_U    POMP_CRITICAL_EXIT
#define POMP_Do_enter_U         POMP_DO_ENTER
#define POMP_Do_exit_U          POMP_DO_EXIT
#define POMP_Master_begin_U     POMP_MASTER_BEGIN
#define POMP_Master_end_U       POMP_MASTER_END
#define POMP_Parallel_begin_U   POMP_PARALLEL_BEGIN
#define POMP_Parallel_end_U     POMP_PARALLEL_END
#define POMP_Parallel_fork_U    POMP_PARALLEL_FORK
#define POMP_Parallel_join_U    POMP_PARALLEL_JOIN
#define POMP_Section_begin_U    POMP_SECTION_BEGIN
#define POMP_Section_end_U      POMP_SECTION_END
#define POMP_Sections_enter_U   POMP_SECTIONS_ENTER
#define POMP_Sections_exit_U    POMP_SECTIONS_EXIT
#define POMP_Single_begin_U     POMP_SINGLE_BEGIN
#define POMP_Single_end_U       POMP_SINGLE_END
#define POMP_Single_enter_U     POMP_SINGLE_ENTER
#define POMP_Single_exit_U      POMP_SINGLE_EXIT
#define POMP_Workshare_enter_U  POMP_WORKSHARE_ENTER
#define POMP_Workshare_exit_U   POMP_WORKSHARE_EXIT
#define POMP_Begin_U            POMP_BEGIN
#define POMP_End_U              POMP_END
#define POMP_Init_lock_U        POMP_INIT_LOCK
#define POMP_Destroy_lock_U     POMP_DESTROY_LOCK
#define POMP_Set_lock_U         POMP_SET_LOCK
#define POMP_Unset_lock_U       POMP_UNSET_LOCK
#define POMP_Test_lock_U        POMP_TEST_LOCK
#define POMP_Init_nest_lock_U   POMP_INIT_NEST_LOCK
#define POMP_Destroy_nest_lock_U        POMP_DESTROY_NEST_LOCK
#define POMP_Set_nest_lock_U    POMP_SET_NEST_LOCK
#define POMP_Unset_nest_lock_U  POMP_UNSET_NEST_LOCK
#define POMP_Test_nest_lock_U   POMP_TEST_NEST_LOCK
#define POMP_Assign_handle_U    POMP_ASSIGN_HANDLE
#define omp_init_lock_U         OMP_INIT_LOCK
#define omp_destroy_lock_U      OMP_DESTROY_LOCK
#define omp_set_lock_U          OMP_SET_LOCK
#define omp_unset_lock_U        OMP_UNSET_LOCK
#define omp_test_lock_U         OMP_TEST_LOCK
#define omp_init_nest_lock_U    OMP_INIT_NEST_LOCK
#define omp_destroy_nest_lock_U OMP_DESTROY_NEST_LOCK
#define omp_set_nest_lock_U     OMP_SET_NEST_LOCK
#define omp_unset_nest_lock_U   OMP_UNSET_NEST_LOCK
#define omp_test_nest_lock_U    OMP_TEST_NEST_LOCK

#define POMP_Finalize_L         pomp_finalize
#define POMP_Init_L             pomp_init
#define POMP_Off_L              pomp_off
#define POMP_On_L               pomp_on
#define POMP_Atomic_enter_L     pomp_atomic_enter
#define POMP_Atomic_exit_L      pomp_atomic_exit
#define POMP_Barrier_enter_L    pomp_barrier_enter
#define POMP_Barrier_exit_L     pomp_barrier_exit
#define POMP_Flush_enter_L      pomp_flush_enter
#define POMP_Flush_exit_L       pomp_flush_exit
#define POMP_Critical_begin_L   pomp_critical_begin
#define POMP_Critical_end_L     pomp_critical_end
#define POMP_Critical_enter_L   pomp_critical_enter
#define POMP_Critical_exit_L    pomp_critical_exit
#define POMP_Do_enter_L         pomp_do_enter
#define POMP_Do_exit_L          pomp_do_exit
#define POMP_Master_begin_L     pomp_master_begin
#define POMP_Master_end_L       pomp_master_end
#define POMP_Parallel_begin_L   pomp_parallel_begin
#define POMP_Parallel_end_L     pomp_parallel_end
#define POMP_Parallel_fork_L    pomp_parallel_fork
#define POMP_Parallel_join_L    pomp_parallel_join
#define POMP_Section_begin_L    pomp_section_begin
#define POMP_Section_end_L      pomp_section_end
#define POMP_Sections_enter_L   pomp_sections_enter
#define POMP_Sections_exit_L    pomp_sections_exit
#define POMP_Single_begin_L     pomp_single_begin
#define POMP_Single_end_L       pomp_single_end
#define POMP_Single_enter_L     pomp_single_enter
#define POMP_Single_exit_L      pomp_single_exit
#define POMP_Workshare_enter_L  pomp_workshare_enter
#define POMP_Workshare_exit_L   pomp_workshare_exit
#define POMP_Begin_L            pomp_begin
#define POMP_End_L              pomp_end
#define POMP_Init_lock_L        pomp_init_lock
#define POMP_Destroy_lock_L     pomp_destroy_lock
#define POMP_Set_lock_L         pomp_set_lock
#define POMP_Unset_lock_L       pomp_unset_lock
#define POMP_Test_lock_L        pomp_test_lock
#define POMP_Init_nest_lock_L   pomp_init_nest_lock
#define POMP_Destroy_nest_lock_L        pomp_destroy_nest_lock
#define POMP_Set_nest_lock_L    pomp_set_nest_lock
#define POMP_Unset_nest_lock_L  pomp_unset_nest_lock
#define POMP_Test_nest_lock_L   pomp_test_nest_lock
#define POMP_Assign_handle_L    pomp_assign_handle
#define omp_init_lock_L         omp_init_lock
#define omp_destroy_lock_L      omp_destroy_lock
#define omp_set_lock_L          omp_set_lock
#define omp_unset_lock_L        omp_unset_lock
#define omp_test_lock_L         omp_test_lock
#define omp_init_nest_lock_L    omp_init_nest_lock
#define omp_destroy_nest_lock_L omp_destroy_nest_lock
#define omp_set_nest_lock_L     omp_set_nest_lock
#define omp_unset_nest_lock_L   omp_unset_nest_lock
#define omp_test_nest_lock_L    omp_test_nest_lock

#endif // SILC_POMP_FORTRAN_H
