${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
${proto:c}
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
    ${rtype} return_val;

    if (event_gen_active)
    {
        SCOREP_MPI_EVENT_GEN_OFF ();
        SCOREP_EnterWrappedRegion(scorep_mpi_regid[SCOREP__${name|uppercase}], ( intptr_t )P${name});

    }

    if (event_gen_active)
    {
      SCOREP_ENTER_WRAPPED_REGION();
    }
    return_val = ${call:pmpi};
    if (event_gen_active)
    {
      SCOREP_EXIT_WRAPPED_REGION();
    }
    if (*${comm:new} != MPI_COMM_NULL)
    {
        scorep_mpi_comm_create(*${comm:new}, ${comm:parent});
    }

    if (event_gen_active)
    {
        SCOREP_ExitRegion (scorep_mpi_regid[SCOREP__${name|uppercase}]);
        SCOREP_MPI_EVENT_GEN_ON();
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();

    return return_val;
}
${guard:end}
