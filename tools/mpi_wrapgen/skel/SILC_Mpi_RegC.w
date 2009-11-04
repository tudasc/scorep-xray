${guard:start}
  if (silc_mpi_enabled & SILC_MPI_ENABLED_${group|uppercase})
  {
    silc_mpi_regid[SILC__${name|uppercase}] =
      SILC_DefineRegion("${name}", file_id, SILC_INVALID_LINE_NO, SILC_INVALID_LINE_NO, SILC_ADAPTER_MPI, SILC_REGION_MPI${mpi:multiplicity});
  }
${guard:end}
