cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetProgramSpecializationConstant )( cl_program  program,
                                                                cl_uint     specId,
                                                                size_t      specSize,
                                                                const void* specValue )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetProgramSpecializationConstant );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetProgramSpecializationConstant,
        ( program, specId, specSize, specValue ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetProgramSpecializationConstant );

    return ret;
}



cl_int
SCOREP_LIBWRAP_FUNC_NAME( clSetProgramReleaseCallback )( cl_program program,
                                                         void( CL_CALLBACK * pfn_notify )( cl_program program, void* user_data ),
                                                         void* userData )
{
    SCOREP_OPENCL_FUNC_ENTER( clSetProgramReleaseCallback );

    SCOREP_OPENCL_WRAP_ENTER();
    cl_int ret = SCOREP_LIBWRAP_FUNC_CALL(
        clSetProgramReleaseCallback,
        ( program, pfn_notify, userData ) );
    SCOREP_OPENCL_WRAP_EXIT();

    SCOREP_OPENCL_FUNC_EXIT( clSetProgramReleaseCallback );

    return ret;
}
