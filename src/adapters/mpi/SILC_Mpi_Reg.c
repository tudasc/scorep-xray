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
 * @file SILC_Mpi_Reg.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Registration of MPI regions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "SILC_Mpi.h"
#include "config.h"

#define SILC_REGION_MPI SILC_REGION_FUNCTION

/**
 * MPI operation event type
 */
typedef struct
{
    char*    name;
    uint32_t type;
} silc_mpi_type;

/** MPI point-to-point function types
 * @note contents must be alphabetically sorted */
static const silc_mpi_type silc_mpi_pt2pt[] = {
    { "MPI_Bsend",            SILC_MPI_TYPE__SEND                               },
    { "MPI_Ibsend",           SILC_MPI_TYPE__SEND                               },
    { "MPI_Irsend",           SILC_MPI_TYPE__SEND                               },
    { "MPI_Isend",            SILC_MPI_TYPE__SEND                               },
    { "MPI_Issend",           SILC_MPI_TYPE__SEND                               },
    { "MPI_Recv",             SILC_MPI_TYPE__RECV                               },
    { "MPI_Rsend",            SILC_MPI_TYPE__SEND                               },
    { "MPI_Send",             SILC_MPI_TYPE__SEND                               },
    { "MPI_Sendrecv",         SILC_MPI_TYPE__SEND & SILC_MPI_TYPE__RECV         },
    { "MPI_Sendrecv_replace", SILC_MPI_TYPE__SEND & SILC_MPI_TYPE__RECV         },
    { "MPI_Ssend",            SILC_MPI_TYPE__SEND                               },
    { "MPI_Start",            SILC_MPI_TYPE__SEND                               },
    { "MPI_Startall",         SILC_MPI_TYPE__SEND                               },
    { "MPI_Test",             SILC_MPI_TYPE__RECV                               },
    { "MPI_Testall",          SILC_MPI_TYPE__RECV                               },
    { "MPI_Testany",          SILC_MPI_TYPE__RECV                               },
    { "MPI_Testsome",         SILC_MPI_TYPE__RECV                               },
    { "MPI_Wait",             SILC_MPI_TYPE__RECV                               },
    { "MPI_Waitall",          SILC_MPI_TYPE__RECV                               },
    { "MPI_Waitany",          SILC_MPI_TYPE__RECV                               },
    { "MPI_Waitsome",         SILC_MPI_TYPE__RECV                               },
};

/** MPI collective function types
 * @note contents must be alphabetically sorted */
static const silc_mpi_type silc_mpi_colls[] = {
    { "MPI_Allgather",      SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Allgatherv",     SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Allreduce",      SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Alltoall",       SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Alltoallv",      SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Alltoallw",      SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Barrier",        SILC_COLL_TYPE__BARRIER           },
    { "MPI_Bcast",          SILC_COLL_TYPE__ONE2ALL           },
    { "MPI_Cart_create",    SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Cart_sub",       SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_create",    SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_dup",       SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_free",      SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_split",     SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Exscan",         SILC_COLL_TYPE__PARTIAL           },
    { "MPI_Finalize",       SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Gather",         SILC_COLL_TYPE__ALL2ONE           },
    { "MPI_Gatherv",        SILC_COLL_TYPE__ALL2ONE           },
    { "MPI_Init",           SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Init_thread",    SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Reduce",         SILC_COLL_TYPE__ALL2ONE           },
    { "MPI_Reduce_scatter", SILC_COLL_TYPE__ALL2ALL           },
    { "MPI_Scan",           SILC_COLL_TYPE__PARTIAL           },
    { "MPI_Scatter",        SILC_COLL_TYPE__ONE2ALL           },
    { "MPI_Scatterv",       SILC_COLL_TYPE__ONE2ALL           },
    { "MPI_Win_create",     SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Win_fence",      SILC_COLL_TYPE__IMPLIED           },
    { "MPI_Win_free",       SILC_COLL_TYPE__IMPLIED           }
};

/** Region IDs of MPI functions */
SILC_RegionHandle silc_mpi_regid[ SILC__MPI_NUMFUNCS + 1 ];

/** Bit vector for runtime measurement wrapper enabling/disabling */
uint64_t silc_mpi_enabled = 0;

/**
 * Register MPI functions and initialize data structures
 */
void
silc_mpi_register_regions()
{
    SILC_SourceFileHandle file_id = SILC_DefineSourceFile( "MPI" );

#if HAVE( DECL_PMPI_ABORT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Abort )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_ABORT ] =
            SILC_DefineRegion( "MPI_Abort", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ACCUMULATE ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Accumulate )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_ACCUMULATE ] =
            SILC_DefineRegion( "MPI_Accumulate", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ADD_ERROR_CLASS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Add_error_class )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ADD_ERROR_CLASS ] =
            SILC_DefineRegion( "MPI_Add_error_class", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ADD_ERROR_CODE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Add_error_code )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ADD_ERROR_CODE ] =
            SILC_DefineRegion( "MPI_Add_error_code", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ADD_ERROR_STRING ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Add_error_string )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ADD_ERROR_STRING ] =
            SILC_DefineRegion( "MPI_Add_error_string", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ADDRESS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Address )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_ADDRESS ] =
            SILC_DefineRegion( "MPI_Address", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ALLGATHER )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_ALLGATHER ] =
            SILC_DefineRegion( "MPI_Allgather", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLGATHERV ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Allgatherv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_ALLGATHERV ] =
            SILC_DefineRegion( "MPI_Allgatherv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLOC_MEM ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Alloc_mem )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_ALLOC_MEM ] =
            SILC_DefineRegion( "MPI_Alloc_mem", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ALLREDUCE ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Allreduce )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_ALLREDUCE ] =
            SILC_DefineRegion( "MPI_Allreduce", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLTOALL ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Alltoall )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_ALLTOALL ] =
            SILC_DefineRegion( "MPI_Alltoall", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLTOALLV ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Alltoallv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_ALLTOALLV ] =
            SILC_DefineRegion( "MPI_Alltoallv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLTOALLW ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Alltoallw )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_ALLTOALLW ] =
            SILC_DefineRegion( "MPI_Alltoallw", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ATTR_DELETE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Attr_delete )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_ATTR_DELETE ] =
            SILC_DefineRegion( "MPI_Attr_delete", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ATTR_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Attr_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_ATTR_GET ] =
            SILC_DefineRegion( "MPI_Attr_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ATTR_PUT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Attr_put )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_ATTR_PUT ] =
            SILC_DefineRegion( "MPI_Attr_put", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_BARRIER ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Barrier )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_BARRIER ] =
            SILC_DefineRegion( "MPI_Barrier", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_BARRIER );
    }
#endif
#if HAVE( DECL_PMPI_BCAST ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Bcast )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_BCAST ] =
            SILC_DefineRegion( "MPI_Bcast", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ONE2ALL );
    }
#endif
#if HAVE( DECL_PMPI_BSEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Bsend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_BSEND ] =
            SILC_DefineRegion( "MPI_Bsend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_BSEND_INIT ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Bsend_init )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_BSEND_INIT ] =
            SILC_DefineRegion( "MPI_Bsend_init", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_BUFFER_ATTACH ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Buffer_attach )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_BUFFER_ATTACH ] =
            SILC_DefineRegion( "MPI_Buffer_attach", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_BUFFER_DETACH ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Buffer_detach )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_BUFFER_DETACH ] =
            SILC_DefineRegion( "MPI_Buffer_detach", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CANCEL ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Cancel )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_CANCEL ] =
            SILC_DefineRegion( "MPI_Cancel", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_COORDS ] =
            SILC_DefineRegion( "MPI_Cart_coords", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_CREATE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_CREATE ] =
            SILC_DefineRegion( "MPI_Cart_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_GET ] =
            SILC_DefineRegion( "MPI_Cart_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_MAP ] =
            SILC_DefineRegion( "MPI_Cart_map", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_RANK ] =
            SILC_DefineRegion( "MPI_Cart_rank", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_SHIFT ] =
            SILC_DefineRegion( "MPI_Cart_shift", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CART_SUB )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CART_SUB ] =
            SILC_DefineRegion( "MPI_Cart_sub", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_CARTDIM_GET ] =
            SILC_DefineRegion( "MPI_Cartdim_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_CLOSE_PORT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Close_port )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_CLOSE_PORT ] =
            SILC_DefineRegion( "MPI_Close_port", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_ACCEPT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_accept )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_ACCEPT ] =
            SILC_DefineRegion( "MPI_Comm_accept", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Comm_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_MISC )
    {
        silc_mpi_regid[ SILC__MPI_COMM_C2F ] =
            SILC_DefineRegion( "MPI_Comm_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CALL_ERRHANDLER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Comm_call_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_ERR )
    {
        silc_mpi_regid[ SILC__MPI_COMM_CALL_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Comm_call_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_COMPARE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Comm_compare )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_COMPARE ] =
            SILC_DefineRegion( "MPI_Comm_compare", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CONNECT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_connect )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_CONNECT ] =
            SILC_DefineRegion( "MPI_Comm_connect", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CREATE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_CREATE ] =
            SILC_DefineRegion( "MPI_Comm_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_ERRHANDLER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Comm_create_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_ERR )
    {
        silc_mpi_regid[ SILC__MPI_COMM_CREATE_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Comm_create_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_create_keyval )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_CREATE_KEYVAL ] =
            SILC_DefineRegion( "MPI_Comm_create_keyval", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_DELETE_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_delete_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_DELETE_ATTR ] =
            SILC_DefineRegion( "MPI_Comm_delete_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_DISCONNECT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_disconnect )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_DISCONNECT ] =
            SILC_DefineRegion( "MPI_Comm_disconnect", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_DUP )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_DUP ] =
            SILC_DefineRegion( "MPI_Comm_dup", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Comm_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_MISC )
    {
        silc_mpi_regid[ SILC__MPI_COMM_F2C ] =
            SILC_DefineRegion( "MPI_Comm_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_FREE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_FREE ] =
            SILC_DefineRegion( "MPI_Comm_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_FREE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_free_keyval )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_FREE_KEYVAL ] =
            SILC_DefineRegion( "MPI_Comm_free_keyval", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_get_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_GET_ATTR ] =
            SILC_DefineRegion( "MPI_Comm_get_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_ERRHANDLER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Comm_get_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_ERR )
    {
        silc_mpi_regid[ SILC__MPI_COMM_GET_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Comm_get_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_get_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_GET_NAME ] =
            SILC_DefineRegion( "MPI_Comm_get_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_PARENT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_get_parent )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_GET_PARENT ] =
            SILC_DefineRegion( "MPI_Comm_get_parent", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GROUP )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_GROUP ] =
            SILC_DefineRegion( "MPI_Comm_group", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_JOIN ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_join )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_JOIN ] =
            SILC_DefineRegion( "MPI_Comm_join", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_RANK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MINI ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Comm_rank )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_RANK ] =
            SILC_DefineRegion( "MPI_Comm_rank", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_GROUP )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_REMOTE_GROUP ] =
            SILC_DefineRegion( "MPI_Comm_remote_group", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MINI ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Comm_remote_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_REMOTE_SIZE ] =
            SILC_DefineRegion( "MPI_Comm_remote_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_set_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SET_ATTR ] =
            SILC_DefineRegion( "MPI_Comm_set_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SET_ERRHANDLER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Comm_set_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_ERR )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SET_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Comm_set_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Comm_set_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SET_NAME ] =
            SILC_DefineRegion( "MPI_Comm_set_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MINI ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Comm_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SIZE ] =
            SILC_DefineRegion( "MPI_Comm_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SPAWN ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_spawn )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SPAWN ] =
            SILC_DefineRegion( "MPI_Comm_spawn", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SPAWN_MULTIPLE ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Comm_spawn_multiple )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SPAWN_MULTIPLE ] =
            SILC_DefineRegion( "MPI_Comm_spawn_multiple", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SPLIT )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_SPLIT ] =
            SILC_DefineRegion( "MPI_Comm_split", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_COMM_TEST_INTER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Comm_test_inter )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_COMM_TEST_INTER ] =
            SILC_DefineRegion( "MPI_Comm_test_inter", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_DIMS_CREATE ] =
            SILC_DefineRegion( "MPI_Dims_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Errhandler_create )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ERRHANDLER_CREATE ] =
            SILC_DefineRegion( "MPI_Errhandler_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Errhandler_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ERRHANDLER_FREE ] =
            SILC_DefineRegion( "MPI_Errhandler_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Errhandler_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ERRHANDLER_GET ] =
            SILC_DefineRegion( "MPI_Errhandler_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_SET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Errhandler_set )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ERRHANDLER_SET ] =
            SILC_DefineRegion( "MPI_Errhandler_set", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ERROR_CLASS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Error_class )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ERROR_CLASS ] =
            SILC_DefineRegion( "MPI_Error_class", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ERROR_STRING ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Error_string )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ERR )
    {
        silc_mpi_regid[ SILC__MPI_ERROR_STRING ] =
            SILC_DefineRegion( "MPI_Error_string", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_EXSCAN ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Exscan )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_EXSCAN ] =
            SILC_DefineRegion( "MPI_Exscan", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_OTHER );
    }
#endif
#if HAVE( DECL_PMPI_FILE_C2F ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_File_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO_MISC )
    {
        silc_mpi_regid[ SILC__MPI_FILE_C2F ] =
            SILC_DefineRegion( "MPI_File_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_CALL_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_call_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO_ERR )
    {
        silc_mpi_regid[ SILC__MPI_FILE_CALL_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_File_call_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_CLOSE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_close )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_CLOSE ] =
            SILC_DefineRegion( "MPI_File_close", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_CREATE_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_create_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO_ERR )
    {
        silc_mpi_regid[ SILC__MPI_FILE_CREATE_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_File_create_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_DELETE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_delete )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_DELETE ] =
            SILC_DefineRegion( "MPI_File_delete", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_F2C ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_File_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO_MISC )
    {
        silc_mpi_regid[ SILC__MPI_FILE_F2C ] =
            SILC_DefineRegion( "MPI_File_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_AMODE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_amode )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_AMODE ] =
            SILC_DefineRegion( "MPI_File_get_amode", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_ATOMICITY ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_atomicity )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_ATOMICITY ] =
            SILC_DefineRegion( "MPI_File_get_atomicity", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_BYTE_OFFSET ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_byte_offset )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_BYTE_OFFSET ] =
            SILC_DefineRegion( "MPI_File_get_byte_offset", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_get_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO_ERR )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_File_get_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_GROUP ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_group )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_GROUP ] =
            SILC_DefineRegion( "MPI_File_get_group", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_INFO ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_info )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_INFO ] =
            SILC_DefineRegion( "MPI_File_get_info", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_position )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_POSITION ] =
            SILC_DefineRegion( "MPI_File_get_position", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_position_shared )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_POSITION_SHARED ] =
            SILC_DefineRegion( "MPI_File_get_position_shared", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_SIZE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_SIZE ] =
            SILC_DefineRegion( "MPI_File_get_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_TYPE_EXTENT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_type_extent )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_TYPE_EXTENT ] =
            SILC_DefineRegion( "MPI_File_get_type_extent", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_VIEW ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_get_view )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_GET_VIEW ] =
            SILC_DefineRegion( "MPI_File_get_view", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IREAD ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_IREAD ] =
            SILC_DefineRegion( "MPI_File_iread", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread_at )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_IREAD_AT ] =
            SILC_DefineRegion( "MPI_File_iread_at", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iread_shared )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_IREAD_SHARED ] =
            SILC_DefineRegion( "MPI_File_iread_shared", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_IWRITE ] =
            SILC_DefineRegion( "MPI_File_iwrite", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite_at )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_IWRITE_AT ] =
            SILC_DefineRegion( "MPI_File_iwrite_at", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_iwrite_shared )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_IWRITE_SHARED ] =
            SILC_DefineRegion( "MPI_File_iwrite_shared", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_OPEN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_open )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_OPEN ] =
            SILC_DefineRegion( "MPI_File_open", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_PREALLOCATE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_preallocate )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_PREALLOCATE ] =
            SILC_DefineRegion( "MPI_File_preallocate", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ ] =
            SILC_DefineRegion( "MPI_File_read", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_ALL ] =
            SILC_DefineRegion( "MPI_File_read_all", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all_begin )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_ALL_BEGIN ] =
            SILC_DefineRegion( "MPI_File_read_all_begin", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_all_end )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_ALL_END ] =
            SILC_DefineRegion( "MPI_File_read_all_end", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_AT ] =
            SILC_DefineRegion( "MPI_File_read_at", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_AT_ALL ] =
            SILC_DefineRegion( "MPI_File_read_at_all", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all_begin )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_AT_ALL_BEGIN ] =
            SILC_DefineRegion( "MPI_File_read_at_all_begin", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_at_all_end )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_AT_ALL_END ] =
            SILC_DefineRegion( "MPI_File_read_at_all_end", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_ORDERED ] =
            SILC_DefineRegion( "MPI_File_read_ordered", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered_begin )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_ORDERED_BEGIN ] =
            SILC_DefineRegion( "MPI_File_read_ordered_begin", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_ordered_end )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_ORDERED_END ] =
            SILC_DefineRegion( "MPI_File_read_ordered_end", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_read_shared )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_READ_SHARED ] =
            SILC_DefineRegion( "MPI_File_read_shared", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SEEK ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_seek )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SEEK ] =
            SILC_DefineRegion( "MPI_File_seek", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SEEK_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_seek_shared )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SEEK_SHARED ] =
            SILC_DefineRegion( "MPI_File_seek_shared", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_ATOMICITY ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_atomicity )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SET_ATOMICITY ] =
            SILC_DefineRegion( "MPI_File_set_atomicity", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_ERRHANDLER ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_File_set_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO_ERR )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SET_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_File_set_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_INFO ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_info )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SET_INFO ] =
            SILC_DefineRegion( "MPI_File_set_info", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_SIZE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SET_SIZE ] =
            SILC_DefineRegion( "MPI_File_set_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_VIEW ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_set_view )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SET_VIEW ] =
            SILC_DefineRegion( "MPI_File_set_view", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SYNC ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_sync )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_SYNC ] =
            SILC_DefineRegion( "MPI_File_sync", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE ] =
            SILC_DefineRegion( "MPI_File_write", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_ALL ] =
            SILC_DefineRegion( "MPI_File_write_all", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all_begin )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_ALL_BEGIN ] =
            SILC_DefineRegion( "MPI_File_write_all_begin", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_all_end )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_ALL_END ] =
            SILC_DefineRegion( "MPI_File_write_all_end", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_AT ] =
            SILC_DefineRegion( "MPI_File_write_at", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_AT_ALL ] =
            SILC_DefineRegion( "MPI_File_write_at_all", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all_begin )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_AT_ALL_BEGIN ] =
            SILC_DefineRegion( "MPI_File_write_at_all_begin", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_at_all_end )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_AT_ALL_END ] =
            SILC_DefineRegion( "MPI_File_write_at_all_end", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_ORDERED ] =
            SILC_DefineRegion( "MPI_File_write_ordered", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_BEGIN ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered_begin )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_ORDERED_BEGIN ] =
            SILC_DefineRegion( "MPI_File_write_ordered_begin", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_END ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_ordered_end )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_ORDERED_END ] =
            SILC_DefineRegion( "MPI_File_write_ordered_end", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_SHARED ) && !defined( SILC_MPI_NO_IO ) && !defined( MPI_File_write_shared )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_FILE_WRITE_SHARED ] =
            SILC_DefineRegion( "MPI_File_write_shared", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FINALIZE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_FINALIZE ] =
            SILC_DefineRegion( "MPI_Finalize", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FINALIZED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV ) && !defined( MPI_Finalized )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_FINALIZED ] =
            SILC_DefineRegion( "MPI_Finalized", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_FREE_MEM ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Free_mem )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_FREE_MEM ] =
            SILC_DefineRegion( "MPI_Free_mem", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GATHER ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Gather )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_GATHER ] =
            SILC_DefineRegion( "MPI_Gather", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ONE );
    }
#endif
#if HAVE( DECL_PMPI_GATHERV ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Gatherv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_GATHERV ] =
            SILC_DefineRegion( "MPI_Gatherv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ONE );
    }
#endif
#if HAVE( DECL_PMPI_GET ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_GET ] =
            SILC_DefineRegion( "MPI_Get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GET_ADDRESS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Get_address )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_GET_ADDRESS ] =
            SILC_DefineRegion( "MPI_Get_address", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GET_COUNT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Get_count )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_GET_COUNT ] =
            SILC_DefineRegion( "MPI_Get_count", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GET_ELEMENTS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Get_elements )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_GET_ELEMENTS ] =
            SILC_DefineRegion( "MPI_Get_elements", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GET_PROCESSOR_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Get_processor_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_GET_PROCESSOR_NAME ] =
            SILC_DefineRegion( "MPI_Get_processor_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GET_VERSION ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Get_version )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_GET_VERSION ] =
            SILC_DefineRegion( "MPI_Get_version", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_CREATE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_GRAPH_CREATE ] =
            SILC_DefineRegion( "MPI_Graph_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_GRAPH_GET ] =
            SILC_DefineRegion( "MPI_Graph_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_GRAPH_MAP ] =
            SILC_DefineRegion( "MPI_Graph_map", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_GRAPH_NEIGHBORS ] =
            SILC_DefineRegion( "MPI_Graph_neighbors", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_GRAPH_NEIGHBORS_COUNT ] =
            SILC_DefineRegion( "MPI_Graph_neighbors_count", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_GRAPHDIMS_GET ] =
            SILC_DefineRegion( "MPI_Graphdims_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GREQUEST_COMPLETE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Grequest_complete )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_GREQUEST_COMPLETE ] =
            SILC_DefineRegion( "MPI_Grequest_complete", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GREQUEST_START ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Grequest_start )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_GREQUEST_START ] =
            SILC_DefineRegion( "MPI_Grequest_start", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Group_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_MISC )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_C2F ] =
            SILC_DefineRegion( "MPI_Group_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_COMPARE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_compare )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_COMPARE ] =
            SILC_DefineRegion( "MPI_Group_compare", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_DIFFERENCE ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_difference )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_DIFFERENCE ] =
            SILC_DefineRegion( "MPI_Group_difference", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_EXCL ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_excl )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_EXCL ] =
            SILC_DefineRegion( "MPI_Group_excl", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Group_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_MISC )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_F2C ] =
            SILC_DefineRegion( "MPI_Group_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_FREE ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_FREE ] =
            SILC_DefineRegion( "MPI_Group_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_INCL ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_incl )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_INCL ] =
            SILC_DefineRegion( "MPI_Group_incl", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_INTERSECTION ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_intersection )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_INTERSECTION ] =
            SILC_DefineRegion( "MPI_Group_intersection", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_EXCL ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_range_excl )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_RANGE_EXCL ] =
            SILC_DefineRegion( "MPI_Group_range_excl", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_INCL ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_range_incl )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_RANGE_INCL ] =
            SILC_DefineRegion( "MPI_Group_range_incl", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_RANK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_rank )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_RANK ] =
            SILC_DefineRegion( "MPI_Group_rank", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_SIZE ] =
            SILC_DefineRegion( "MPI_Group_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_TRANSLATE_RANKS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_translate_ranks )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_TRANSLATE_RANKS ] =
            SILC_DefineRegion( "MPI_Group_translate_ranks", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_UNION ) && !defined( SILC_MPI_NO_CG ) && !defined( MPI_Group_union )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_GROUP_UNION ] =
            SILC_DefineRegion( "MPI_Group_union", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_IBSEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Ibsend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_IBSEND ] =
            SILC_DefineRegion( "MPI_Ibsend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_C2F ] =
            SILC_DefineRegion( "MPI_Info_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_create )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_CREATE ] =
            SILC_DefineRegion( "MPI_Info_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_DELETE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_delete )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_DELETE ] =
            SILC_DefineRegion( "MPI_Info_delete", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_DUP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_dup )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_DUP ] =
            SILC_DefineRegion( "MPI_Info_dup", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_F2C ] =
            SILC_DefineRegion( "MPI_Info_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_FREE ] =
            SILC_DefineRegion( "MPI_Info_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_get )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_GET ] =
            SILC_DefineRegion( "MPI_Info_get", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET_NKEYS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_get_nkeys )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_GET_NKEYS ] =
            SILC_DefineRegion( "MPI_Info_get_nkeys", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET_NTHKEY ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_get_nthkey )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_GET_NTHKEY ] =
            SILC_DefineRegion( "MPI_Info_get_nthkey", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET_VALUELEN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_get_valuelen )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_GET_VALUELEN ] =
            SILC_DefineRegion( "MPI_Info_get_valuelen", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INFO_SET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Info_set )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_INFO_SET ] =
            SILC_DefineRegion( "MPI_Info_set", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INIT )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_INIT ] =
            SILC_DefineRegion( "MPI_Init", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INIT_THREAD )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_INIT_THREAD ] =
            SILC_DefineRegion( "MPI_Init_thread", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INITIALIZED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV ) && !defined( MPI_Initialized )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_INITIALIZED ] =
            SILC_DefineRegion( "MPI_Initialized", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INTERCOMM_CREATE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_INTERCOMM_CREATE ] =
            SILC_DefineRegion( "MPI_Intercomm_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_INTERCOMM_MERGE )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG )
    {
        silc_mpi_regid[ SILC__MPI_INTERCOMM_MERGE ] =
            SILC_DefineRegion( "MPI_Intercomm_merge", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_IPROBE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Iprobe )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_IPROBE ] =
            SILC_DefineRegion( "MPI_Iprobe", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_IRECV ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Irecv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_IRECV ] =
            SILC_DefineRegion( "MPI_Irecv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_IRSEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Irsend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_IRSEND ] =
            SILC_DefineRegion( "MPI_Irsend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_IS_THREAD_MAIN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_IS_THREAD_MAIN ] =
            SILC_DefineRegion( "MPI_Is_thread_main", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ISEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Isend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_ISEND ] =
            SILC_DefineRegion( "MPI_Isend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_ISSEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Issend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_ISSEND ] =
            SILC_DefineRegion( "MPI_Issend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_KEYVAL_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Keyval_create )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_KEYVAL_CREATE ] =
            SILC_DefineRegion( "MPI_Keyval_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_KEYVAL_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_CG ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Keyval_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_CG_EXT )
    {
        silc_mpi_regid[ SILC__MPI_KEYVAL_FREE ] =
            SILC_DefineRegion( "MPI_Keyval_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_LOOKUP_NAME ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Lookup_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_LOOKUP_NAME ] =
            SILC_DefineRegion( "MPI_Lookup_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_OP_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Op_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_OP_C2F ] =
            SILC_DefineRegion( "MPI_Op_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_OP_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Op_create )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_OP_CREATE ] =
            SILC_DefineRegion( "MPI_Op_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_OP_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Op_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_OP_F2C ] =
            SILC_DefineRegion( "MPI_Op_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_OP_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Op_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_OP_FREE ] =
            SILC_DefineRegion( "MPI_Op_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_OPEN_PORT ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Open_port )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_OPEN_PORT ] =
            SILC_DefineRegion( "MPI_Open_port", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PACK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Pack )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_PACK ] =
            SILC_DefineRegion( "MPI_Pack", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Pack_external )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_PACK_EXTERNAL ] =
            SILC_DefineRegion( "MPI_Pack_external", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Pack_external_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_PACK_EXTERNAL_SIZE ] =
            SILC_DefineRegion( "MPI_Pack_external_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PACK_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Pack_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_PACK_SIZE ] =
            SILC_DefineRegion( "MPI_Pack_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PROBE ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Probe )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_PROBE ] =
            SILC_DefineRegion( "MPI_Probe", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PUBLISH_NAME ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Publish_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_PUBLISH_NAME ] =
            SILC_DefineRegion( "MPI_Publish_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_PUT ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Put )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_PUT ] =
            SILC_DefineRegion( "MPI_Put", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_QUERY_THREAD ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV ) && !defined( MPI_Query_thread )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_ENV )
    {
        silc_mpi_regid[ SILC__MPI_QUERY_THREAD ] =
            SILC_DefineRegion( "MPI_Query_thread", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_RECV ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Recv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_RECV ] =
            SILC_DefineRegion( "MPI_Recv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_RECV_INIT ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Recv_init )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_RECV_INIT ] =
            SILC_DefineRegion( "MPI_Recv_init", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_REDUCE ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Reduce )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_REDUCE ] =
            SILC_DefineRegion( "MPI_Reduce", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ONE );
    }
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_REDUCE_SCATTER ] =
            SILC_DefineRegion( "MPI_Reduce_scatter", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_REGISTER_DATAREP ) && !defined( SILC_MPI_NO_IO ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Register_datarep )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_IO )
    {
        silc_mpi_regid[ SILC__MPI_REGISTER_DATAREP ] =
            SILC_DefineRegion( "MPI_Register_datarep", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Request_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_REQUEST_C2F ] =
            SILC_DefineRegion( "MPI_Request_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Request_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_REQUEST_F2C ] =
            SILC_DefineRegion( "MPI_Request_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Request_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_REQUEST_FREE ] =
            SILC_DefineRegion( "MPI_Request_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_GET_STATUS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Request_get_status )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_REQUEST_GET_STATUS ] =
            SILC_DefineRegion( "MPI_Request_get_status", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_RSEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Rsend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_RSEND ] =
            SILC_DefineRegion( "MPI_Rsend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_RSEND_INIT ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Rsend_init )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_RSEND_INIT ] =
            SILC_DefineRegion( "MPI_Rsend_init", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SCAN ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Scan )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_SCAN ] =
            SILC_DefineRegion( "MPI_Scan", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_OTHER );
    }
#endif
#if HAVE( DECL_PMPI_SCATTER ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Scatter )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_SCATTER ] =
            SILC_DefineRegion( "MPI_Scatter", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ONE2ALL );
    }
#endif
#if HAVE( DECL_PMPI_SCATTERV ) && !defined( SILC_MPI_NO_COLL ) && !defined( MPI_Scatterv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_COLL )
    {
        silc_mpi_regid[ SILC__MPI_SCATTERV ] =
            SILC_DefineRegion( "MPI_Scatterv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_ONE2ALL );
    }
#endif
#if HAVE( DECL_PMPI_SEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Send )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_SEND ] =
            SILC_DefineRegion( "MPI_Send", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SEND_INIT ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Send_init )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_SEND_INIT ] =
            SILC_DefineRegion( "MPI_Send_init", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SENDRECV ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Sendrecv )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_SENDRECV ] =
            SILC_DefineRegion( "MPI_Sendrecv", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SENDRECV_REPLACE ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Sendrecv_replace )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_SENDRECV_REPLACE ] =
            SILC_DefineRegion( "MPI_Sendrecv_replace", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SIZEOF ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Sizeof )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_SIZEOF ] =
            SILC_DefineRegion( "MPI_Sizeof", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SSEND ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Ssend )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_SSEND ] =
            SILC_DefineRegion( "MPI_Ssend", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_SSEND_INIT ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Ssend_init )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_SSEND_INIT ] =
            SILC_DefineRegion( "MPI_Ssend_init", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_START ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Start )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_START ] =
            SILC_DefineRegion( "MPI_Start", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_STARTALL ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Startall )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_STARTALL ] =
            SILC_DefineRegion( "MPI_Startall", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Status_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_STATUS_C2F ] =
            SILC_DefineRegion( "MPI_Status_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Status_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_MISC )
    {
        silc_mpi_regid[ SILC__MPI_STATUS_F2C ] =
            SILC_DefineRegion( "MPI_Status_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_SET_CANCELLED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Status_set_cancelled )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_STATUS_SET_CANCELLED ] =
            SILC_DefineRegion( "MPI_Status_set_cancelled", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_SET_ELEMENTS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Status_set_elements )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_STATUS_SET_ELEMENTS ] =
            SILC_DefineRegion( "MPI_Status_set_elements", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TEST ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Test )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_TEST ] =
            SILC_DefineRegion( "MPI_Test", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Test_cancelled )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_TEST_CANCELLED ] =
            SILC_DefineRegion( "MPI_Test_cancelled", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TESTALL ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Testall )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_TESTALL ] =
            SILC_DefineRegion( "MPI_Testall", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TESTANY ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Testany )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_TESTANY ] =
            SILC_DefineRegion( "MPI_Testany", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Testsome )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_TESTSOME ] =
            SILC_DefineRegion( "MPI_Testsome", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TOPO )
    {
        silc_mpi_regid[ SILC__MPI_TOPO_TEST ] =
            SILC_DefineRegion( "MPI_Topo_test", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_C2F ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Type_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_MISC )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_C2F ] =
            SILC_DefineRegion( "MPI_Type_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_COMMIT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_commit )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_COMMIT ] =
            SILC_DefineRegion( "MPI_Type_commit", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CONTIGUOUS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_contiguous )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CONTIGUOUS ] =
            SILC_DefineRegion( "MPI_Type_contiguous", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_DARRAY ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_darray )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_DARRAY ] =
            SILC_DefineRegion( "MPI_Type_create_darray", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_COMPLEX ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_complex )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_F90_COMPLEX ] =
            SILC_DefineRegion( "MPI_Type_create_f90_complex", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_INTEGER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_integer )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_F90_INTEGER ] =
            SILC_DefineRegion( "MPI_Type_create_f90_integer", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_REAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_real )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_F90_REAL ] =
            SILC_DefineRegion( "MPI_Type_create_f90_real", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HINDEXED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_hindexed )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_HINDEXED ] =
            SILC_DefineRegion( "MPI_Type_create_hindexed", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HVECTOR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_hvector )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_HVECTOR ] =
            SILC_DefineRegion( "MPI_Type_create_hvector", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_INDEXED_BLOCK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_indexed_block )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_INDEXED_BLOCK ] =
            SILC_DefineRegion( "MPI_Type_create_indexed_block", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_create_keyval )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_KEYVAL ] =
            SILC_DefineRegion( "MPI_Type_create_keyval", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_RESIZED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_resized )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_RESIZED ] =
            SILC_DefineRegion( "MPI_Type_create_resized", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_STRUCT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_struct )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_STRUCT ] =
            SILC_DefineRegion( "MPI_Type_create_struct", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_SUBARRAY ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_create_subarray )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_CREATE_SUBARRAY ] =
            SILC_DefineRegion( "MPI_Type_create_subarray", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_DELETE_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_delete_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_DELETE_ATTR ] =
            SILC_DefineRegion( "MPI_Type_delete_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_DUP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_dup )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_DUP ] =
            SILC_DefineRegion( "MPI_Type_dup", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_EXTENT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_extent )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_EXTENT ] =
            SILC_DefineRegion( "MPI_Type_extent", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_F2C ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_MISC ) && !defined( MPI_Type_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_MISC )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_F2C ] =
            SILC_DefineRegion( "MPI_Type_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_FREE ] =
            SILC_DefineRegion( "MPI_Type_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_FREE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_free_keyval )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_FREE_KEYVAL ] =
            SILC_DefineRegion( "MPI_Type_free_keyval", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_get_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_GET_ATTR ] =
            SILC_DefineRegion( "MPI_Type_get_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_CONTENTS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_get_contents )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_GET_CONTENTS ] =
            SILC_DefineRegion( "MPI_Type_get_contents", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ENVELOPE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_get_envelope )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_GET_ENVELOPE ] =
            SILC_DefineRegion( "MPI_Type_get_envelope", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_EXTENT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_get_extent )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_GET_EXTENT ] =
            SILC_DefineRegion( "MPI_Type_get_extent", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_get_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_GET_NAME ] =
            SILC_DefineRegion( "MPI_Type_get_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_TRUE_EXTENT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_get_true_extent )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_GET_TRUE_EXTENT ] =
            SILC_DefineRegion( "MPI_Type_get_true_extent", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_HINDEXED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_hindexed )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_HINDEXED ] =
            SILC_DefineRegion( "MPI_Type_hindexed", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_HVECTOR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_hvector )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_HVECTOR ] =
            SILC_DefineRegion( "MPI_Type_hvector", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_INDEXED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_indexed )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_INDEXED ] =
            SILC_DefineRegion( "MPI_Type_indexed", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_LB ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_lb )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_LB ] =
            SILC_DefineRegion( "MPI_Type_lb", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_MATCH_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_match_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_MATCH_SIZE ] =
            SILC_DefineRegion( "MPI_Type_match_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_SET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_set_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_SET_ATTR ] =
            SILC_DefineRegion( "MPI_Type_set_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_SET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Type_set_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE_EXT )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_SET_NAME ] =
            SILC_DefineRegion( "MPI_Type_set_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_size )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_SIZE ] =
            SILC_DefineRegion( "MPI_Type_size", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_STRUCT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_struct )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_STRUCT ] =
            SILC_DefineRegion( "MPI_Type_struct", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_UB ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_ub )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_UB ] =
            SILC_DefineRegion( "MPI_Type_ub", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_VECTOR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Type_vector )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_TYPE_VECTOR ] =
            SILC_DefineRegion( "MPI_Type_vector", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_UNPACK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Unpack )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_UNPACK ] =
            SILC_DefineRegion( "MPI_Unpack", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_UNPACK_EXTERNAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( MPI_Unpack_external )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_TYPE )
    {
        silc_mpi_regid[ SILC__MPI_UNPACK_EXTERNAL ] =
            SILC_DefineRegion( "MPI_Unpack_external", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_UNPUBLISH_NAME ) && !defined( SILC_MPI_NO_SPAWN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Unpublish_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_SPAWN )
    {
        silc_mpi_regid[ SILC__MPI_UNPUBLISH_NAME ] =
            SILC_DefineRegion( "MPI_Unpublish_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WAIT ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Wait )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_WAIT ] =
            SILC_DefineRegion( "MPI_Wait", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WAITALL ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Waitall )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_WAITALL ] =
            SILC_DefineRegion( "MPI_Waitall", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WAITANY ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Waitany )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_WAITANY ] =
            SILC_DefineRegion( "MPI_Waitany", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SILC_MPI_NO_P2P ) && !defined( MPI_Waitsome )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_P2P )
    {
        silc_mpi_regid[ SILC__MPI_WAITSOME ] =
            SILC_DefineRegion( "MPI_Waitsome", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_C2F ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_MISC ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_c2f )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_MISC )
    {
        silc_mpi_regid[ SILC__MPI_WIN_C2F ] =
            SILC_DefineRegion( "MPI_Win_c2f", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CALL_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_call_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_ERR )
    {
        silc_mpi_regid[ SILC__MPI_WIN_CALL_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Win_call_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_COMPLETE ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_complete )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_COMPLETE ] =
            SILC_DefineRegion( "MPI_Win_complete", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CREATE ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_create )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_CREATE ] =
            SILC_DefineRegion( "MPI_Win_create", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_BARRIER );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CREATE_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_create_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_ERR )
    {
        silc_mpi_regid[ SILC__MPI_WIN_CREATE_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Win_create_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CREATE_KEYVAL ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_create_keyval )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_CREATE_KEYVAL ] =
            SILC_DefineRegion( "MPI_Win_create_keyval", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_DELETE_ATTR ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_delete_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_DELETE_ATTR ] =
            SILC_DefineRegion( "MPI_Win_delete_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_F2C ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_MISC ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_f2c )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_MISC )
    {
        silc_mpi_regid[ SILC__MPI_WIN_F2C ] =
            SILC_DefineRegion( "MPI_Win_f2c", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_FENCE ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_fence )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_FENCE ] =
            SILC_DefineRegion( "MPI_Win_fence", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_BARRIER );
    }
#endif
#if HAVE( DECL_PMPI_WIN_FREE ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_free )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_FREE ] =
            SILC_DefineRegion( "MPI_Win_free", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI_COLL_BARRIER );
    }
#endif
#if HAVE( DECL_PMPI_WIN_FREE_KEYVAL ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_free_keyval )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_FREE_KEYVAL ] =
            SILC_DefineRegion( "MPI_Win_free_keyval", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_ATTR ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_get_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_GET_ATTR ] =
            SILC_DefineRegion( "MPI_Win_get_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_get_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_ERR )
    {
        silc_mpi_regid[ SILC__MPI_WIN_GET_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Win_get_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_GROUP ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_get_group )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_GET_GROUP ] =
            SILC_DefineRegion( "MPI_Win_get_group", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_NAME ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_get_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_GET_NAME ] =
            SILC_DefineRegion( "MPI_Win_get_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_LOCK ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_lock )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_LOCK ] =
            SILC_DefineRegion( "MPI_Win_lock", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_POST ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_post )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_POST ] =
            SILC_DefineRegion( "MPI_Win_post", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_SET_ATTR ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Win_set_attr )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_SET_ATTR ] =
            SILC_DefineRegion( "MPI_Win_set_attr", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_SET_ERRHANDLER ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ERR ) && !defined( MPI_Win_set_errhandler )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_ERR )
    {
        silc_mpi_regid[ SILC__MPI_WIN_SET_ERRHANDLER ] =
            SILC_DefineRegion( "MPI_Win_set_errhandler", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_SET_NAME ) && !defined( SILC_MPI_NO_RMA ) && !defined( SILC_MPI_NO_EXT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( MPI_Win_set_name )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WIN_SET_NAME ] =
            SILC_DefineRegion( "MPI_Win_set_name", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_START ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_start )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_START ] =
            SILC_DefineRegion( "MPI_Win_start", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_TEST ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_test )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_TEST ] =
            SILC_DefineRegion( "MPI_Win_test", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_UNLOCK ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_unlock )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_UNLOCK ] =
            SILC_DefineRegion( "MPI_Win_unlock", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WIN_WAIT ) && !defined( SILC_MPI_NO_RMA ) && !defined( MPI_Win_wait )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_RMA )
    {
        silc_mpi_regid[ SILC__MPI_WIN_WAIT ] =
            SILC_DefineRegion( "MPI_Win_wait", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WTICK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MINI ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Wtick )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WTICK ] =
            SILC_DefineRegion( "MPI_Wtick", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif
#if HAVE( DECL_PMPI_WTIME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_MINI ) && !defined( SILC_MPI_NO_EXT ) && !defined( MPI_Wtime )
    if ( silc_mpi_enabled & SILC_MPI_ENABLED_EXT )
    {
        silc_mpi_regid[ SILC__MPI_WTIME ] =
            SILC_DefineRegion( "MPI_Wtime", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI );
    }
#endif

    /* Artificial root for MPI-only experiments when no user-code
     * instrumenation is available */
    silc_mpi_regid[ SILC_PARALLEL__MPI ] =
        SILC_DefineRegion( "PARALLEL", SILC_INVALID_SOURCE_FILE, SILC_INVALID_LINE_NO,
                           SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_UNKNOWN );
}

/**
 * Compare function for binary search
 * @param v1 string to checked
 * @param v2 pointer to \p silc_mpi_type structure
 * @return result is equivalent to \p strcasecmp result of two strings
 */
static int
silc_mpi_mycmp( const void* v1,
                const void* v2 )
{
    return strcasecmp( ( char* )v1, ( ( silc_mpi_type* )v2 )->name );
}

/**
 * Check if region is flagged \a collective
 * @param str Region name
 * @return type of function if collective, 0 otherwise.
 */
uint8_t
silc_is_mpi_collective( const char* str )
{
    silc_mpi_type* match =
        bsearch( str, ( silc_mpi_type* )silc_mpi_colls, sizeof( silc_mpi_colls ) / sizeof( silc_mpi_type ),
                 sizeof( silc_mpi_type ), silc_mpi_mycmp );

    return match ? match->type : 0;
}

/**
 * Check if region is flagged \a point-to-point
 * @param str Region name
 * @return type of function if point-to-point, 0 otherwise.
 */
uint8_t
silc_is_mpi_point2point( const char* str )
{
    silc_mpi_type* match =
        bsearch( str, ( silc_mpi_type* )silc_mpi_pt2pt, sizeof( silc_mpi_pt2pt ) / sizeof( silc_mpi_type ),
                 sizeof( silc_mpi_type ), silc_mpi_mycmp );

    return match ? match->type : SILC_MPI_TYPE__NONE;
}

/**
 * Check if region is flagged either \a point-to-point or \a collective
 * @param  str Region name
 * @return type of function, if point-to-point or collective. 0,
 *         otherwise.
 */
uint8_t
silc_mpi_eventtype( const char* str )
{
    uint8_t type = silc_is_mpi_point2point( str );
    if ( type != SILC_MPI_TYPE__NONE )
    {
        return type;
    }

    type = ( uint8_t )silc_is_mpi_collective( str );
    switch ( type )
    {
        case SILC_COLL_TYPE__BARRIER:
        case SILC_COLL_TYPE__ONE2ALL:
        case SILC_COLL_TYPE__ALL2ONE:
        case SILC_COLL_TYPE__ALL2ALL:
        case SILC_COLL_TYPE__PARTIAL:
            return SILC_MPI_TYPE__COLLECTIVE;
        default:
            break;
    }
    return SILC_MPI_TYPE__NONE;
}
