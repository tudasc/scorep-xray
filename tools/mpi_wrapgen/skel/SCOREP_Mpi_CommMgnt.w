${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_DefineMPICommunicator is called.
 */
${proto:c}
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR(SCOREP_MPI_ENABLED_${group|uppercase});
    ${rtype} return_val;

    if (event_gen_active)
    {
        SCOREP_MPI_EVENT_GEN_OFF ();
        SCOREP_EnterRegion (scorep_mpi_regid[SCOREP__${name|uppercase}]);

    }

    return_val = ${call:pmpi};
    if (*newcomm != MPI_COMM_NULL)
    {
        scorep_mpi_comm_create(*newcomm);
    }

    if (event_gen_active)
    {
        SCOREP_ExitRegion (scorep_mpi_regid[SCOREP__${name|uppercase}]);
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
${guard:end}
