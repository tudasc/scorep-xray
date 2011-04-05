#if HAVE( ${name|uppercase}_COMPLIANT )
${guard:start}
/**
 * Measurement wrapper for ${name}
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f_Compl.w
 * @note Fortran interface
 * @note Introduced with MPI-${mpi:version}
 * @ingroup ${group|lowercase}
 * For the order of events see @ref ${name}
 */
${proto:f2c_c2f} 
{
    ${decl:f2c_c2f}
    ${init:f2c_c2f}

    ${xblock:f2c_c2f}

    *ierr = ${call:f2c_c2f}

    ${cleanup:f2c_c2f}
}
${guard:end}
#endif
