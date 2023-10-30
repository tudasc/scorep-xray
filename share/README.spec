STATUS
------

 * The file 'scorep.spec.in' has following changes:
    - All <cubeplinit> parts have been moved to the top to
      have only one loop over all callpaths and benefit from
      merging some conditions.
    - Some conditions have been rewritten to use regexes
      rather than explicit string comparison.
    - OpenCL kernels have been moved out of the OpenCL sub-
      hierarchy to a subnode of a new 'Computation' metric.
    - Added subhierarchy for CUDA.

 * The file 'scorep_alt.spec' generates an alternative (not
   paradigm-centric) metric hierarchy. It focuses on global
   topics like synchonization, communication, management and
   file I/O which break down to paradigm specific level. It
   should be mostly equivalent to the old hierarchy (in terms
   of available time sub-metrics), except for a new metric for
   communicator management (as an example for stuff we
   currently don't break out into a separate metric).


LIMITATIONS
-----------

 * The alternative hierarchy currently only addresses the 'Time'
   hierarchy. There is no handling for the other metric trees
   available in the original hierarchy, yet.

 * The alternative hierarchy currently ignores the OpenMP idle
   threads metric

 * CUDA and OpenCL subhierarchies are experimental. Need to be
   examined with the GPU experts.
