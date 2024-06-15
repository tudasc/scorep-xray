${guard:start}
/**
 * Declaration of PMPI-symbol for ${name}
 */
${proto:c(P)};

/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgntSubset.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
${proto:c}
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
    SCOREP_InterimCommunicatorHandle new_comm_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;
    ${rtype} return_val;

    if (event_gen_active)
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if (event_gen_active_for_group)
        {
            SCOREP_EnterWrappedRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
            SCOREP_MpiCollectiveBegin();
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = ${call:pmpi};
    SCOREP_EXIT_WRAPPED_REGION();
    if (*${comm:new} != MPI_COMM_NULL)
    {
        new_comm_handle = scorep_mpi_comm_create(*${comm:new}, ${comm:parent});
    }

    if (event_gen_active)
    {
        if (event_gen_active_for_group)
        {
            if ( new_comm_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
            {
              SCOREP_CommCreate(new_comm_handle);
            }
            else
            {
              /* The communicator creation was a local operation, hence we use MPI_COMM_SELF for the collective */
              new_comm_handle = SCOREP_MPI_COMM_HANDLE(MPI_COMM_SELF);
            }
            SCOREP_MpiCollectiveEnd(new_comm_handle,
                                    SCOREP_INVALID_ROOT_RANK,
                                    SCOREP_COLLECTIVE_CREATE_HANDLE,
                                    0,
                                    0);
            SCOREP_ExitRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
${guard:end}