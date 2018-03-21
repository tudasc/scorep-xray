${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_RmaPut.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * Triggers an enter and exit event.
 */
${proto:c}
{
  SCOREP_IN_MEASUREMENT_INCREMENT();
  const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
  const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
  ${rtype} return_val;
  ${decl}

  if (event_gen_active)
  {
/* One-sided communication not handled in first version
    int     sendsz;
    elg_ui4 dpid;
    elg_ui4 wid;
*/
    SCOREP_MPI_EVENT_GEN_OFF();
    if (event_gen_active_for_group)
    {
      SCOREP_EnterWrappedRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);

/* One-sided communication not handled in first version
      dpid = scorep_mpi_win_rank_to_pe( target_rank, win );
      wid = scorep_mpi_win_rank_id( win );

      PMPI_Type_size(origin_datatype, &sendsz);
      esd_mpi_put_1ts( dpid, wid, SCOREP_NEXT_RMA_ID, origin_count * sendsz);
*/
    }
    else
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
/* One-sided communication not handled in first version
      esd_mpi_put_1te_remote(dpid, wid, SCOREP_CURR_RMA_ID);
*/

      SCOREP_ExitRegion(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
    }
    else
    {
      SCOREP_ExitWrapper(scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}]);
    }
    SCOREP_MPI_EVENT_GEN_ON();
  }

  SCOREP_IN_MEASUREMENT_DECREMENT();
  return return_val;
}
${guard:end}
