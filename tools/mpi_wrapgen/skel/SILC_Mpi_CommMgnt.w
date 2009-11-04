${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: coll_mgnt.w
 * @note C interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 */
${proto:c}
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR(SILC_MPI_ENABLED_${group|uppercase});
    ${rtype} return_val;

    if (event_gen_active)
    {
        EVENT_GEN_OFF ();
        SILC_EnterRegion (silc_mpi_regid[SILC__${name|uppercase}]);

    }

    return_val = ${call:pmpi};
    if (*newcomm != MPI_COMM_NULL)
    {
        silc_mpi_comm_create(*newcomm);
    }

    if (event_gen_active)
    {
        SILC_ExitRegion (silc_mpi_regid[SILC__${name|uppercase}]);
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
${guard:end}

