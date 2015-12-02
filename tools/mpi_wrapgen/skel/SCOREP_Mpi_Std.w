${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * Triggers an enter and exit event.
 * It wraps the ${name} call with enter and exit events.
 */
${proto:c}
{
  SCOREP_IN_MEASUREMENT_INCREMENT();
  ${rtype} return_val;

  if (SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase}))
  {
    ${decl}
    ${init}

    SCOREP_MPI_EVENT_GEN_OFF();
    SCOREP_EnterWrappedRegion(scorep_mpi_regid[SCOREP__${name|uppercase}], ( intptr_t )P${name});

    ${xblock}
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = ${call:pmpi};
    SCOREP_EXIT_WRAPPED_REGION();

    SCOREP_ExitRegion(scorep_mpi_regid[SCOREP__${name|uppercase}]);
    SCOREP_MPI_EVENT_GEN_ON();
  }
  else
  {
      return_val = ${call:pmpi};
  }
  SCOREP_IN_MEASUREMENT_DECREMENT();

    return return_val;
}
${guard:end}

