Several components in Score-P rely on mapping addresses to source-code
locations like filename and line number. Score-P uses the BFD library
for this mapping and takes addresses within the instrumented binary as
well as within shared objects loaded at program startup into account.

If, however, the binary loads additional shared objects during runtime
via `dlopen`, addresses from within these shared objects are by
default, not considered for lookup.

To change this behavior, Score-P can intercept `dlopen`/`dlclose` calls
using the dynamic linker's auditing API (man 7 rtld-audit), if
desired. To do so, prefix only the application command with
`LD_AUDIT=<scorep_prefix>/lib/libscorep_rtld_audit.so`. For an MPI
application started with `mpiexec` where environment variables are
exported to the application via `-x`, it would, e.g., look like this:

    $ mpiexec -np 4 -x LD_AUDIT=/opt/scorep/lib/libscorep_rtld_audit.so ./jacobi

Note that you must not *export* `LD_AUDIT` or put it in front of
launchers like `mpiexec` or `srun`; linker auditing takes place
immediately after `LD_AUDIT` is set and results in error messages
about missing symbols if not used together with Score-P instrumented
binaries.
