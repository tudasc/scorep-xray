${guard:start}
  if (scorep_mpi_enabled & SCOREP_MPI_ENABLED_${group|uppercase})
  {
    scorep_mpi_regid[SCOREP__${name|uppercase}] =
      SCOREP_DefineRegion("${name}", NULL, file_id, SCOREP_INVALID_LINE_NO, SCOREP_INVALID_LINE_NO, SCOREP_ADAPTER_MPI, SCOREP_REGION_MPI${mpi:multiplicity});
  }
${guard:end}
