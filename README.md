# Score-P

The Score-P (Scalable Performance Measurement Infrastructure for
Parallel Codes) measurement infrastructure is a highly scalable and
easy-to-use tool suite for profiling and event trace recording of
HPC applications.

You can build and install Score-P either using ready-to-use tarballs
from https://score-p.org/ or using a Git checkout.

## Build from a downloaded source package

Download a tarball and just do the following:

   ```console
   $ tar xf scorep-<version>.tar.gz
   $ mkdir scorep-<version>/_build
   $ cd scorep-<version>/_build
   $ ../configure --prefix=PREFIX <further options, see INSTALL>
   $ make
   $ make install
   ```

If not already in `$PATH`, this will also install the Score-P
dependencies CubeW, CubeLib, OTF2, and OPARI2 under PREFIX. Add
`PREFIX/bin` to `$PATH`.

The configure output, also accessible via the file `scorep.summary`,
will provide information about compilers used and components
found. Rerun configure with different options until satisfied.

After `make install` you will find html and pdf documentation as well
as usage examples under `PREFIX/share/doc/scorep`.

## Build from Git checkout

If building and installing from the Git checkout, the following
requirements are needed (assuming `PREFIX/bin` is in `$PATH`):

 - AfS-dev 05 (patched versions of Autotools):

   Install via

   ```console
   $ wget https://perftools.pages.jsc.fz-juelich.de/utils/afs-dev/afs-dev-05.tar.gz
   $ tar xf afs-dev-05.tar.gz
   $ cd afs-dev-05
   $ ./install-afs-dev.sh --continue-after-download --prefix=PREFIX
   ```

 - Perftools-dev 08 (code beautification and documentation generation):

   Install via

   ```console
   $ wget https://perftools.pages.jsc.fz-juelich.de/utils/perftools-dev/perftools-dev-08.tar.gz
   $ tar xf perftools-dev-08.tar.gz
   $ cd perftools-dev-08
   $ ./install-perftools-dev.sh --continue-after-download --prefix=PREFIX
   ```

 - OPARI2, OTF2, CubeW, CubeLib

   Download from https://score-p.org/, extract, and install into `PREFIX`.

Once the requirements are set up, build and install like this:

   ```console
   $ ./bootstrap
   $ mkdir _build
   $ cd _build
   $ ../configure --prefix=PREFIX <further options, see INSTALL>
   $ make
   $ make install
   ```

Please have a look at [INSTALL](INSTALL) for a complete list of build options.
Please consult [OPEN_ISSUES](OPEN_ISSUES) for known deficiencies.

## Build using containers

Docker images are provided to ease development for common frameworks.

Images are available for the following base images:

 - Ubuntu: scorep/devel-gcc and scorep/devel-clang
 - NVIDIA HPC SDK: scorep/devel-nvhpc
 - Intel oneAPI: scorep/devel-oneapi
 - AMD ROCm: scorep/devel-rocm

Images tagged as `latest` are for use with the main branch. For releases use
as tag the `major.minor` version of Score-P. I.e., `scorep/devel-gcc:8.0`
to build any Score-P 8.0.x release in the GCC container.

Start the container with:

   ```console
   [host]$ docker run [--rm] -it \
               --cap-add SYS_PTRACE \
               --cap-add SYS_ADMIN \
               -u $(id -u):$(id -g) \
               --volume $PWD:/workspace \
               scorep/devel-gcc

   [container]$ ./bootstrap
   [container]$ mkdir _build
   [container]$ cd _build
   [container]$ ../configure
   [container]$ make
   ```

## Citing

Please refer to the Score-P measurement infrastructure by citing the
overview article
_[Score-P: A Joint Performance Measurement Run-Time Infrastructure for Periscope, Scalasca, TAU, and Vampir](https://link.springer.com/chapter/10.1007/978-3-642-31476-6_7)_

Version-specific DOIs of the software can be found on
[Score-P's Zenodo page](https://zenodo.org/record/1240731).

See also CITATION.cff for machine-readable citation information.

Have fun!

Please report bugs to <support@score-p.org>.
