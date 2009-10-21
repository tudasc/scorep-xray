${guard:start}
  if (epk_mpi_enabled & EPK_MPI_ENABLED_${group|uppercase})
  {
    epk_mpi_regid[EPK__${name|uppercase}] =
      esd_def_region("${name}", fid, ELG_NO_LNO, ELG_NO_LNO, "MPI", ELG_FUNCTION${mpi:multiplicity});
  }
${guard:end}
