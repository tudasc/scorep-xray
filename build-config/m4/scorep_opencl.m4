## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014-2015, 2017,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_opencl.m4

dnl ----------------------------------------------------------------------------

dnl Deprecated OpenCL APIs
dnl
dnl Marked as deprecated since OpenCL 1.1
dnl            clCreateImage2D,
dnl            clCreateImage3D,
dnl            clEnqueueMarker,
dnl            clEnqueueWaitForEvents,
dnl            clEnqueueBarrier,
dnl            clUnloadCompiler,
dnl            clGetExtensionFunctionAddress
dnl
dnl Marked as deprecated since OpenCL 2.0
dnl            clCreateCommandQueue,
dnl            clCreateSampler,
dnl            clEnqueueTask

AC_DEFUN([_SCOREP_OPENCL_1_0_ADD_SYMBOLS], [
m4_foreach([func],
           [clGetPlatformIDs,
            clGetPlatformInfo,
            clGetDeviceIDs,
            clGetDeviceInfo,
            clCreateContext,
            clCreateContextFromType,
            clRetainContext,
            clReleaseContext,
            clGetContextInfo,
            clRetainCommandQueue,
            clReleaseCommandQueue,
            clGetCommandQueueInfo,
            clSetCommandQueueProperty,
            clCreateBuffer,
            clRetainMemObject,
            clReleaseMemObject,
            clGetSupportedImageFormats,
            clGetMemObjectInfo,
            clGetImageInfo,
            clRetainSampler,
            clReleaseSampler,
            clGetSamplerInfo,
            clCreateProgramWithSource,
            clCreateProgramWithBinary,
            clRetainProgram,
            clReleaseProgram,
            clBuildProgram,
            clGetProgramInfo,
            clGetProgramBuildInfo,
            clCreateKernel,
            clCreateKernelsInProgram,
            clRetainKernel,
            clReleaseKernel,
            clSetKernelArg,
            clGetKernelInfo,
            clGetKernelWorkGroupInfo,
            clWaitForEvents,
            clGetEventInfo,
            clRetainEvent,
            clReleaseEvent,
            clGetEventProfilingInfo,
            clFlush,
            clFinish,
            clEnqueueReadBuffer,
            clEnqueueWriteBuffer,
            clEnqueueCopyBuffer,
            clEnqueueReadImage,
            clEnqueueWriteImage,
            clEnqueueCopyImage,
            clEnqueueCopyImageToBuffer,
            clEnqueueCopyBufferToImage,
            clEnqueueMapBuffer,
            clEnqueueMapImage,
            clEnqueueUnmapMemObject,
            clEnqueueNDRangeKernel,
            clEnqueueNativeKernel,
            clCreateImage2D,
            clCreateImage3D,
            clEnqueueMarker,
            clEnqueueWaitForEvents,
            clEnqueueBarrier,
            clUnloadCompiler,
            clGetExtensionFunctionAddress,
            clCreateCommandQueue,
            clCreateSampler,
            clEnqueueTask],
           [AS_VAR_APPEND([$1], [" func"]);])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_OPENCL_1_1_ADD_SYMBOLS], [
m4_foreach([func],
           [clCreateSubBuffer,
            clSetMemObjectDestructorCallback,
            clCreateUserEvent,
            clSetUserEventStatus,
            clSetEventCallback,
            clEnqueueReadBufferRect,
            clEnqueueWriteBufferRect,
            clEnqueueCopyBufferRect],
           [AS_VAR_APPEND([$1], [" func"]);])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_OPENCL_1_2_ADD_SYMBOLS], [
m4_foreach([func],
           [clCreateSubDevices,
            clRetainDevice,
            clReleaseDevice,
            clCreateImage,
            clCreateProgramWithBuiltInKernels,
            clCompileProgram,
            clLinkProgram,
            clUnloadPlatformCompiler,
            clGetKernelArgInfo,
            clEnqueueFillBuffer,
            clEnqueueFillImage,
            clEnqueueMigrateMemObjects,
            clEnqueueMarkerWithWaitList,
            clEnqueueBarrierWithWaitList,
            clGetExtensionFunctionAddressForPlatform],
           [AS_VAR_APPEND([$1], [" func"]);])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_OPENCL_2_0_ADD_SYMBOLS], [
m4_foreach([func],
           [clCreateCommandQueueWithProperties,
            clCreatePipe,
            clGetPipeInfo,
            clSVMAlloc,
            clSVMFree,
            clCreateSamplerWithProperties,
            clSetKernelArgSVMPointer,
            clSetKernelExecInfo,
            clEnqueueSVMFree,
            clEnqueueSVMMemcpy,
            clEnqueueSVMMemFill,
            clEnqueueSVMMap,
            clEnqueueSVMUnmap],
           [AS_VAR_APPEND([$1], [" func"]);])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([SCOREP_OPENCL], [
AC_REQUIRE([SCOREP_LIBRARY_WRAPPING])dnl

AFS_SUMMARY_PUSH

AM_COND_IF([HAVE_LIBWRAP_SUPPORT],
      [AC_SCOREP_BACKEND_LIB([libOpenCL], [CL/cl.h])
       AS_IF([test "x$scorep_opencl_error" = "xyes"],
             [AS_UNSET([ac_cv_search_clGetPlatformIDs])
              AC_SCOREP_BACKEND_LIB([libOpenCL], [OpenCL/opencl.h])])],
      [scorep_opencl_error="yes"
       AM_CONDITIONAL(HAVE_LIBOPENCL, [test 1 -eq 0])
       AC_MSG_NOTICE([OpenCL instrumentation disabled, no library wrapping available])])

scorep_opencl_wrap_symbols=""
AC_SCOREP_COND_HAVE([OPENCL_VERSION_1_0_SUPPORT],
                    [test "x${scorep_opencl_version_1_0}" = "xyes" && \
                     test "x${scorep_opencl_error}" = "xno" ],
                    [Defined if OpenCL API version 1.0 is supported.],
                    [_SCOREP_OPENCL_1_0_ADD_SYMBOLS([scorep_opencl_wrap_symbols])])
AC_SCOREP_COND_HAVE([OPENCL_VERSION_1_1_SUPPORT],
                    [test "x${scorep_opencl_version_1_1}" = "xyes" && \
                     test "x${scorep_opencl_error}" = "xno" ],
                    [Defined if OpenCL API version 1.1 is supported.],
                    [_SCOREP_OPENCL_1_1_ADD_SYMBOLS([scorep_opencl_wrap_symbols])])
AC_SCOREP_COND_HAVE([OPENCL_VERSION_1_2_SUPPORT],
                    [test "x${scorep_opencl_version_1_2}" = "xyes" && \
                     test "x${scorep_opencl_error}" = "xno" ],
                    [Defined if OpenCL API version 1.2 is supported.],
                    [_SCOREP_OPENCL_1_2_ADD_SYMBOLS([scorep_opencl_wrap_symbols])])
AC_SCOREP_COND_HAVE([OPENCL_VERSION_2_0_SUPPORT],
                    [test "x${scorep_opencl_version_2_0}" = "xyes" && \
                     test "x${scorep_opencl_error}" = "xno" ],
                    [Defined if OpenCL API version 2.0 is supported.],
                    [_SCOREP_OPENCL_2_0_ADD_SYMBOLS([scorep_opencl_wrap_symbols])])
AC_SCOREP_COND_HAVE([OPENCL_SUPPORT],
                    [test "x${scorep_opencl_error}" = "xno"],
                    [Defined if OpenCL is available.],
                    [AC_SUBST(OPENCL_CPPFLAGS,          ["${with_libOpenCL_cppflags}"])
                     AC_SUBST(OPENCL_LDFLAGS,           ["${with_libOpenCL_ldflags} ${with_libOpenCL_rpathflag}"])
                     AC_SUBST(OPENCL_LDFLAGS_FOR_TESTS, ["${with_libOpenCL_ldflags}"])
                     AC_SUBST(OPENCL_LIBS,              ["${with_libOpenCL_libs}"])
                     scorep_opencl_summary="yes"],
                    [AC_SUBST(OPENCL_CPPFLAGS,          [""])
                     AC_SUBST(OPENCL_LDFLAGS,           [""])
                     AC_SUBST(OPENCL_LDFLAGS_FOR_TESTS, [""])
                     AC_SUBST(OPENCL_LIBS,              [""])
                     scorep_opencl_summary="no"])

AFS_SUMMARY_POP([OpenCL support], [${scorep_opencl_summary}])
])

dnl --------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBOPENCL_LIB_CHECK], [
scorep_opencl_lib_name="OpenCL"
scorep_opencl_error="no"

dnl checking for OPENCL library
AS_IF([test "x$scorep_opencl_error" = "xno"],
      [AC_SEARCH_LIBS([clGetPlatformIDs],
                      [$scorep_opencl_lib_name],
                      [],
                      [AS_IF([test "x${with_libopencl}" != xnot_set || test "x${with_libopencl_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no libOpenCL found; check path to OpenCL library ...])])
                       scorep_opencl_error="yes"])])

scorep_opencl_version_1_0="no"
scorep_opencl_version_1_1="no"
scorep_opencl_version_1_2="no"
scorep_opencl_version_2_0="no"

dnl check the version of the OPENCL API
AS_IF([test "x$scorep_opencl_error" = "xno"],
      [# check for OPENCL API version 1.0
       AC_MSG_NOTICE([Checking OpenCL API version])
       AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
       [[
#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif
        ]],
        [[
#ifndef CL_VERSION_1_0
#  ups__cl_version_1_0_not_defined
#endif
        ]])],
        [AC_MSG_NOTICE([OpenCL API version 1.0])
         scorep_opencl_version_1_0="yes"])

       # check for OpenCL API version 1.1
       AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
       [[
#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif
        ]],
        [[
#ifndef CL_VERSION_1_1
#  ups__cl_version_1_1_not_defined
#endif
        ]])],
        [AC_MSG_NOTICE([OpenCL API version 1.1])
         scorep_opencl_version_1_1="yes"])

       # check for OpenCL API version 1.0
       AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
       [[
#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif
        ]],
        [[
#ifndef CL_VERSION_1_2
#  ups__cl_version_1_2_not_defined
#endif
        ]])],
        [AC_MSG_NOTICE([OpenCL API version 1.2])
         scorep_opencl_version_1_2="yes"])

       # check for OpenCL API version 2.0
       AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
       [[
#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif
        ]],
        [[
#ifndef CL_VERSION_2_0
#  ups__cl_version_2_0_not_defined
#endif
        ]])],
        [AC_MSG_NOTICE([OpenCL API version 2.0])
         scorep_opencl_version_2_0="yes"])

         ])

AS_IF([ test "x${scorep_opencl_version_1_0}" = "xno" && \
        test "x${scorep_opencl_version_1_1}" = "xno" && \
        test "x${scorep_opencl_version_1_2}" = "xno" && \
        test "x${scorep_opencl_version_2_0}" = "xno"],
    [AC_MSG_NOTICE([OpenCL API version could not be determined. See 'config.log' for more details.])
     scorep_opencl_error="yes" ])

dnl final check for errors
if test "x${scorep_opencl_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_opencl_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
