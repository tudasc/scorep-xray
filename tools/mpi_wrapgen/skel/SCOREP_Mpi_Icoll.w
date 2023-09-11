${guard:start}
/**
 * Declaration of PMPI-symbol for ${name}
 */
${proto:c(P)};

/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Icoll.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * It wraps the mpi call with an enter and exit event. Additionally, a collective
 * event is generated in between the enter and exit event after the PMPI call.
 */
${proto:c}
{
  SCOREP_IN_MEASUREMENT_INCREMENT();
  const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
  const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
  ${rtype} return_val;
  SCOREP_MpiRequestId reqid;

  uint64_t sendbytes = 0, recvbytes = 0;

  if (event_gen_active)
    {
      reqid = scorep_mpi_get_request_id();
      SCOREP_MPI_EVENT_GEN_OFF();
      if (event_gen_active_for_group)
        {
          ${mpi:sendrecvbytes}

          SCOREP_EnterWrappedRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
          SCOREP_MpiNonBlockingCollectiveRequest(reqid);
        }
      else if ( SCOREP_IsUnwindingEnabled() )
        {
          SCOREP_EnterWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
        }
    }

  SCOREP_ENTER_WRAPPED_REGION();
  return_val = ${call:pmpi};
  SCOREP_EXIT_WRAPPED_REGION();

  if (event_gen_active)
    {
      if (event_gen_active_for_group)
        {
          if (return_val == MPI_SUCCESS)
          {
            scorep_mpi_request_icoll_create(*request, SCOREP_MPI_REQUEST_FLAG_NONE, SCOREP_MPI_COLLECTIVE__${name|uppercase},
                                ${root}, sendbytes, recvbytes, comm, reqid);
          }
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
