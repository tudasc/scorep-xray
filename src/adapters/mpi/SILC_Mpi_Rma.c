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


/**
 * @file  SILC_Mpi_Rma.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for one-sided communication
 */

#include <config.h>
#include "SILC_Mpi.h"

/** internal id counter for rma operations */
static int silc_rma_id = 0;

/** current rma id to use in event generation */
#define SILC_CURR_RMA_ID   silc_rma_id
/** increment and get rma id to use in event generation */
#define SILC_NEXT_RMA_ID ++silc_rma_id

/**
 * @name C wrappers for access functions
 * @{
 */
#if HAVE( DECL_PMPI_ACCUMULATE ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Accumulate )
/**
 * Measurement wrapper for MPI_Accumulate
 * @note Auto-generated by wrapgen from template: SILC_Mpi_RmaPut.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Accumulate( void*        origin_addr,
                int          origin_count,
                MPI_Datatype origin_datatype,
                int          target_rank,
                MPI_Aint     target_disp,
                int          target_count,
                MPI_Datatype target_datatype,
                MPI_Op       op,
                MPI_Win      win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
/* One-sided communication not handled in first version
    int     sendsz;
    elg_ui4 dpid;
    elg_ui4 wid;
 */
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ACCUMULATE ] );

/* One-sided communication not handled in first version
    dpid = silc_mpi_win_rank_to_pe( target_rank, win );
    wid = silc_mpi_win_rank_id( win );

    PMPI_Type_size(origin_datatype, &sendsz);
    esd_mpi_put_1ts( dpid, wid, SILC_NEXT_RMA_ID, origin_count * sendsz);
 */
        return_val = PMPI_Accumulate( origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win );

/* One-sided communication not handled in first version
    esd_mpi_put_1te_remote(dpid, wid, SILC_CURR_RMA_ID);
 */

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ACCUMULATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Accumulate( origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GET ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Get
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Get( void*        origin_addr,
         int          origin_count,
         MPI_Datatype origin_datatype,
         int          target_rank,
         MPI_Aint     target_disp,
         int          target_count,
         MPI_Datatype target_datatype,
         MPI_Win      win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
/* One-sided communication not handled in first version
    int32_t              sendsz;
    SILC_Mpi_Rank        dest_proc_rank;
    SILC_MPIWindowHandle win_handle;
 */

        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GET ] );

/* One-sided communication not handled in first version
    dest_proc_rank = silc_mpi_win_rank_to_pe( target_rank, win );
    win_handle = silc_mpi_win_id( win );

    /* in MPI_GET the target buffer is remote
    PMPI_Type_size(target_datatype, &sendsz);
    esd_mpi_get_1ts_remote(dest_proc_rank, win_handle, SILC_NEXT_RMA_ID, target_count * sendsz);
 */
        return_val = PMPI_Get( origin_addr, origin_count,
                               origin_datatype, target_rank, target_disp,
                               target_count, target_datatype, win );

        /* in MPI_GET the origin buffer is local */
/* One-sided communication not handled in first version
    esd_mpi_get_1te( dest_proc_rank, win_handle, SILC_CURR_RMA_ID);
 */
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GET ] );

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Get( origin_addr, origin_count,
                               origin_datatype, target_rank, target_disp,
                               target_count, target_datatype, win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_PUT ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Put )
/**
 * Measurement wrapper for MPI_Put
 * @note Auto-generated by wrapgen from template: SILC_Mpi_RmaPut.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Put( void*        origin_addr,
         int          origin_count,
         MPI_Datatype origin_datatype,
         int          target_rank,
         MPI_Aint     target_disp,
         int          target_count,
         MPI_Datatype target_datatype,
         MPI_Win      win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
/* One-sided communication not handled in first version
    int     sendsz;
    elg_ui4 dpid;
    elg_ui4 wid;
 */
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_PUT ] );

/* One-sided communication not handled in first version
    dpid = silc_mpi_win_rank_to_pe( target_rank, win );
    wid = silc_mpi_win_rank_id( win );

    PMPI_Type_size(origin_datatype, &sendsz);
    esd_mpi_put_1ts( dpid, wid, SILC_NEXT_RMA_ID, origin_count * sendsz);
 */
        return_val = PMPI_Put( origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win );

/* One-sided communication not handled in first version
    esd_mpi_put_1te_remote(dpid, wid, SILC_CURR_RMA_ID);
 */

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_PUT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Put( origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win );
    }

    return return_val;
}
#endif

/**
 * @}
 * @name C wrappers for window management functions
 * @{
 */

#if HAVE( DECL_PMPI_WIN_CREATE ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_create
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_create( void*    base,
                MPI_Aint size,
                int      disp_unit,
                MPI_Info info,
                MPI_Comm comm,
                MPI_Win* win )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_CREATE ] );
    }

    return_val = PMPI_Win_create( base, size, disp_unit,
                                  info, comm, win );

/* One-sided communication not handled in first version
   if (*win != MPI_WIN_NULL)
   {
    silc_mpi_win_rank_create( *win, comm );
   }
 */

    if ( event_gen_active )
    {
/* One-sided communication not handled in first version
    esd_mpi_wincollexit(silc_mpi_regid[SILC__MPI_WIN_CREATE], silc_mpi_win_rank_id(*win));
 */
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_FREE ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_free
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_free( MPI_Win* win )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA );
    int       return_val;
/* One-sided communication not handled in first version
   SILC_MPIWindowHanle win_handle;
 */

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_FREE ] );
/* One-sided communication not handled in first version
    win_handle = silc_mpi_win_rank_id(*win);
 */
    }

/* One-sided communication not handled in first version
   silc_mpi_win_rank_free(*win);
 */

    return_val = PMPI_Win_free( win );

    if ( event_gen_active )
    {
/* One-sided communication not handled in first version
    esd_mpi_wincollexit(silc_mpi_regid[SILC__MPI_WIN_FREE], win_handle);
 */

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

/**
 * @}
 * @name C wrappers for synchonization functions
 * @{
 */

#if HAVE( DECL_PMPI_WIN_COMPLETE ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_complete
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_complete( MPI_Win win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_COMPLETE ] );

        return_val = PMPI_Win_complete( win );

/* One-sided communication not handled in first version
    esd_mpi_winexit(silc_mpi_regid[SILC__MPI_WIN_COMPLETE],
                    silc_mpi_win_rank_id(win), silc_mpi_winacc_get_gid(win, 1), 1);
    silc_mpi_winacc_end(win, 1);
 */
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_complete( win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_FENCE ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_fence
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_fence( int     assert,
               MPI_Win win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_FENCE ] );

        return_val = PMPI_Win_fence( assert, win );

/* One-sided communication not handled in first version
    esd_mpi_wincollexit(silc_mpi_regid[SILC__MPI_WIN_FENCE], silc_mpi_win_rank_id(win));
 */
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_fence( assert, win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_LOCK ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_lock
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_lock( int     lock_type,
              int     rank,
              int     assert,
              MPI_Win win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_LOCK ] );

/* One-sided communication not handled in first version
    esd_mpi_win_lock( rank, silc_mpi_win_rank_id(win), lock_type==MPI_LOCK_EXCLUSIVE);
 */
        return_val = PMPI_Win_lock( lock_type, rank,
                                    assert, win );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_LOCK ] );

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_lock( lock_type, rank,
                                    assert, win );
    }
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_POST ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_post
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_post( MPI_Group group,
              int       assert,
              MPI_Win   win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_POST ] );

/* One-sided communication not handled in first version
    silc_mpi_winacc_start(win, group, 0);
 */
        return_val = PMPI_Win_post( group, assert, win );

/* One-sided communication not handled in first version
    esd_mpi_winexit(silc_mpi_regid[SILC__MPI_WIN_POST],
                    silc_mpi_win_rank_id(win), silc_mpi_winacc_get_gid(win, 0), 0);
 */
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_post( group, assert, win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_START ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_start
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_start( MPI_Group group,
               int       assert,
               MPI_Win   win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_START ] );

/* One-sided communication not handled in first version
    silc_mpi_winacc_start(win, group, 1);
 */
        return_val = PMPI_Win_start( group, assert, win );

/* One-sided communication not handled in first version
    esd_mpi_winexit(silc_mpi_regid[SILC__MPI_WIN_START],
                    silc_mpi_win_rank_id(win), silc_mpi_winacc_get_gid(win, 1), 0);
 */
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_start( group, assert, win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_TEST ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_test
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_test( MPI_Win win,
              int*    flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_TEST ] );

        return_val = PMPI_Win_test( win, flag );

/* One-sided communication not handled in first version
    if (*flag != 0)
    {
      esd_mpi_winexit(silc_mpi_regid[SILC__MPI_WIN_TEST],
                      silc_mpi_win_rank_id(win), silc_mpi_winacc_get_gid(win, 0), 1);
      silc_mpi_winacc_end(win, 0);
    }
    else
    {
      esd_mpi_winexit(silc_mpi_regid[SILC__MPI_WIN_TEST],
                      silc_mpi_win_rank_id(win), silc_mpi_winacc_get_gid(win, 0), 0);
    }
 */
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_test( win, flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_UNLOCK ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_unlock
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_unlock( int     rank,
                MPI_Win win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_UNLOCK ] );

        return_val = PMPI_Win_unlock( rank, win );
/* One-sided communication not handled in first version
    esd_mpi_win_unlock(rank, silc_mpi_win_rank_id(win));
 */

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_UNLOCK ] );

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_unlock( rank, win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_WAIT ) && !defined( SILC_MPI_NO_RMA )
/**
 * Measurement wrapper for MPI_Win_wait
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter and exit event.
 */
int
MPI_Win_wait( MPI_Win win )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA ) )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_WAIT ] );

        return_val = PMPI_Win_wait( win );

/* One-sided communication not handled in first version
    esd_mpi_winexit(silc_mpi_regid[SILC__MPI_WIN_WAIT],
                    silc_mpi_win_rank_id(win), silc_mpi_winacc_get_gid(win, 0), 1);
    silc_mpi_winacc_end(win, 0);
 */

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_wait( win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_GET_GROUP ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_get_group )
/**
 * Measurement wrapper for MPI_Win_get_group
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Group.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma
 * Triggers an enter event and an exit event. Furthermore, a communicator definition
 * event is called between enter and exit.
 */
int
MPI_Win_get_group( MPI_Win    win,
                   MPI_Group* group )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_GROUP ] );
    }

    return_val = PMPI_Win_get_group( win, group );
    if ( *group != MPI_GROUP_NULL )
    {
        silc_mpi_group_create( *group );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_GROUP ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

/**
 * @}
 * @name C wrappers for error handling functions
 * @{
 */
#if HAVE( DECL_PMPI_WIN_CALL_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_call_errhandler )
/**
 * Measurement wrapper for MPI_Win_call_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_err
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_call_errhandler( MPI_Win win,
                         int     errorcode )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_CALL_ERRHANDLER ] );

        return_val = PMPI_Win_call_errhandler( win, errorcode );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_CALL_ERRHANDLER ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_call_errhandler( win, errorcode );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_CREATE_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_create_errhandler )
/**
 * Measurement wrapper for MPI_Win_create_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_err
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_create_errhandler( MPI_Win_errhandler_fn* function,
                           MPI_Errhandler*        errhandler )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_CREATE_ERRHANDLER ] );

        return_val = PMPI_Win_create_errhandler( function, errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_CREATE_ERRHANDLER ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_create_errhandler( function, errhandler );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_GET_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_get_errhandler )
/**
 * Measurement wrapper for MPI_Win_get_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_err
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_get_errhandler( MPI_Win         win,
                        MPI_Errhandler* errhandler )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_ERRHANDLER ] );

        return_val = PMPI_Win_get_errhandler( win, errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_ERRHANDLER ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_get_errhandler( win, errhandler );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_SET_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_set_errhandler )
/**
 * Measurement wrapper for MPI_Win_set_errhandler
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_err
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_set_errhandler( MPI_Win        win,
                        MPI_Errhandler errhandler )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_ERR ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_SET_ERRHANDLER ] );

        return_val = PMPI_Win_set_errhandler( win, errhandler );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_SET_ERRHANDLER ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_set_errhandler( win, errhandler );
    }

    return return_val;
}
#endif


/**
 * @}
 * @name C wrappers for external interface functions
 * @{
 */
#if HAVE( DECL_PMPI_WIN_CREATE_KEYVAL ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_create_keyval )
/**
 * Measurement wrapper for MPI_Win_create_keyval
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_create_keyval( MPI_Win_copy_attr_function*   win_copy_attr_fn,
                       MPI_Win_delete_attr_function* win_delete_attr_fn,
                       int*                          win_keyval,
                       void*                         extra_state )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_CREATE_KEYVAL ] );

        return_val = PMPI_Win_create_keyval( win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_CREATE_KEYVAL ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_create_keyval( win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_DELETE_ATTR ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_delete_attr )
/**
 * Measurement wrapper for MPI_Win_delete_attr
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_delete_attr( MPI_Win win,
                     int     win_keyval )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_DELETE_ATTR ] );

        return_val = PMPI_Win_delete_attr( win, win_keyval );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_DELETE_ATTR ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_delete_attr( win, win_keyval );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_FREE_KEYVAL ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_free_keyval )
/**
 * Measurement wrapper for MPI_Win_free_keyval
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_free_keyval( int* win_keyval )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_FREE_KEYVAL ] );

        return_val = PMPI_Win_free_keyval( win_keyval );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_FREE_KEYVAL ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_free_keyval( win_keyval );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_GET_ATTR ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_get_attr )
/**
 * Measurement wrapper for MPI_Win_get_attr
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_get_attr( MPI_Win win,
                  int     win_keyval,
                  void*   attribute_val,
                  int*    flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_ATTR ] );

        return_val = PMPI_Win_get_attr( win, win_keyval, attribute_val, flag );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_ATTR ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_get_attr( win, win_keyval, attribute_val, flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_GET_NAME ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_get_name )
/**
 * Measurement wrapper for MPI_Win_get_name
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_get_name( MPI_Win win,
                  char*   win_name,
                  int*    resultlen )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_NAME ] );

        return_val = PMPI_Win_get_name( win, win_name, resultlen );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_GET_NAME ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_get_name( win, win_name, resultlen );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_SET_ATTR ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Win_set_attr )
/**
 * Measurement wrapper for MPI_Win_set_attr
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_set_attr( MPI_Win win,
                  int     win_keyval,
                  void*   attribute_val )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_SET_ATTR ] );

        return_val = PMPI_Win_set_attr( win, win_keyval, attribute_val );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_SET_ATTR ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_set_attr( win, win_keyval, attribute_val );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_SET_NAME ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_set_name )
/**
 * Measurement wrapper for MPI_Win_set_name
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Win_set_name( MPI_Win win,
                  char*   win_name )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_EXT ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_SET_NAME ] );

        return_val = PMPI_Win_set_name( win, win_name );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_SET_NAME ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_set_name( win, win_name );
    }

    return return_val;
}
#endif


/**
 * @}
 * @name C wrappers for handle conversion functions
 * @{
 */
#if HAVE( DECL_PMPI_WIN_C2F ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_MISC ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_c2f )
/**
 * Measurement wrapper for MPI_Win_c2f
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_misc
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
MPI_Fint
MPI_Win_c2f( MPI_Win win )
{
    MPI_Fint return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_C2F ] );

        return_val = PMPI_Win_c2f( win );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_C2F ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_c2f( win );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WIN_F2C ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_MISC ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_f2c )
/**
 * Measurement wrapper for MPI_Win_f2c
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup rma_misc
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
MPI_Win
MPI_Win_f2c( MPI_Fint win )
{
    MPI_Win return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_RMA_MISC ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WIN_F2C ] );

        return_val = PMPI_Win_f2c( win );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WIN_F2C ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Win_f2c( win );
    }

    return return_val;
}
#endif


/**
 * @}
 */
