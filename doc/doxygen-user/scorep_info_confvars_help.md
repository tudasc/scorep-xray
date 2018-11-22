Score-P allows to configure several measurement parameters via environment
variables.  After the measurement run you can find a 'scorep.cfg' file in
your experiment directory which contains the configuration of the measurement
run.  If you did not set any configuration values explicitly, this file will
contain the default values.  This file is safe to be used as input for a POSIX
shell.  For example, if you want to reuse the same configuration from a previous
measurement run, do something like this:

    $ set -a
    $ . scorep.cfg
    $ set +a

Measurement configuration variables have a specific type which accepts certain
values.

Supported Types
===============

String
------

An arbitrary character sequence, no white space trimming is done.

Path
----

Like String but a path is expected.  Though no validation is performed.

Boolean
-------

A Boolean value, no white space trimming is done.
Accepted Boolean values for true are case insensitive and the following:

 - 'true'
 - 'yes'
 - 'on'
 - any positive decimal number

Everything else is interpreted as the Boolean value false.

Number
------

A decimal number, white space trimming is done.

Number with size suffixes
-------------------------

Like Number, but also accepts unit case insensitive suffixes after optional
white space:

 - 'B', 'Kb', 'Mb', 'Gb', 'Tb', 'Pb', 'Eb'

The 'b' suffix can be omitted.

Set
---

A symbolic set.  Accepted members are listed in the documentation of the
variable.  Multiple values are allowed, are case insensitive, and are
subject to white space trimming.  They can be separated with one of the
following characters:

 - ' ' - space
 - ',' - comma
 - ':' - colon
 - ';' - semicolon

Acceptable values can also have aliases, which are listed in the
documentation and separated by '/'. Values can be negated by preceeding it
with '~'. Order of evaluation is from left to right.

Option
------

Like Set, but only one value allowed to be selected.
