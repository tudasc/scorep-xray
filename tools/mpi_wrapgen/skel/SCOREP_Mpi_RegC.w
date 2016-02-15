${guard:start}
  if (scorep_mpi_enabled & SCOREP_MPI_ENABLED_${group|uppercase})
  {
    scorep_mpi_regions[SCOREP_MPI_REGION__${name|uppercase}] =
      SCOREP_Definitions_NewRegion("${name}",
                          NULL,
                          file_id,
                          SCOREP_INVALID_LINE_NO,
                          SCOREP_INVALID_LINE_NO,
                          SCOREP_PARADIGM_MPI,
                          SCOREP_REGION_${attribute(kind)});
  }
${guard:end}
