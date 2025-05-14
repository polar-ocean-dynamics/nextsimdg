.. Copyright (c) 2025, Nansen Environmental and Remote Sensing Center

Using XIOS for reading and writing files
========================================

Introduction
------------

Section under construction.

XIOS concepts
-------------

Section under construction.

Configuration
-------------

XIOS is configured automatically from files with `.cfg` extension that are
passed to the `nextsim` executable. There are several configuration sections
that are relevant to XIOS.

The `xios` section contains a single entry, which determines whether or not to
build nextSIM-DG with XIOS as the I/O driver.

.. code-block::
  [xios]
  enable = true

The `model` section, which is used elsewhere in nextSIM-DG, contains two entries
relevant to XIOS: `start` and `time_step`. These are used to configure the
calendar used by XIOS and take the following default values.

.. code-block::
  [model]
  start = 1970-01-01T00:00:00Z
  time_step = P0-0T01:00:00

Files and fields to be read and written to files are configured via `XiosInput`
and `XiosOutut` sections, which accept the same entries. For example, we could
set the following values, which would specify two fields labelled `field_A` and
`field_B`, which are written to file `my_output_file.nc` every two (simulated)
hours.

.. code-block::
  [XiosOutput]
  period = P0-0T02:00:00
  filename = my_output_file.nc
  fields = field_A,field_B
