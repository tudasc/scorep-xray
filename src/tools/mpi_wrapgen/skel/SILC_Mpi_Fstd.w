${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 */
${proto:fortran} {
  ${decl:fortran}
  ${init:fortran}

  *ierr = ${call:fortran}
  
  ${cleanup:fortran}
}
${guard:end}
